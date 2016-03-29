/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#define _POSIX_
#include "qplatformdefs.h"
#include "qabstractfileengine.h"
#include "private/qfsfileengine_p.h"
#include <qdebug.h>

#include "qfile.h"
#include "qdir.h"
#include "private/qmutexpool_p.h"
#include "qvarlengtharray.h"
#include "qdatetime.h"
#include "qt_windows.h"

#if !defined(Q_OS_WINCE)
#  include <sys/types.h>
#  include <direct.h>
#  include <winioctl.h>
#else
#  include <types.h>
#endif
#include <objbase.h>
#include <shlobj.h>
#include <initguid.h>
#include <accctrl.h>
#include <ctype.h>
#include <limits.h>
#define SECURITY_WIN32
#ifdef Q_CC_MINGW
// A workaround for a certain version of MinGW, the define UNICODE_STRING.
#include <subauth.h>
#endif
#include <security.h>

#ifndef _INTPTR_T_DEFINED
#ifdef  _WIN64
typedef __int64             intptr_t;
#else
#ifdef _W64
typedef _W64 int            intptr_t;
#else
typedef INT_PTR intptr_t;
#endif
#endif
#define _INTPTR_T_DEFINED
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#  define INVALID_FILE_ATTRIBUTES (DWORD (-1))
#endif

#if !defined(Q_OS_WINCE)
#  if !defined(REPARSE_DATA_BUFFER_HEADER_SIZE)
typedef struct _REPARSE_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG  Flags;
            WCHAR  PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR  PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR  DataBuffer[1];
        } GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#    define REPARSE_DATA_BUFFER_HEADER_SIZE  FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)
#  endif // !defined(REPARSE_DATA_BUFFER_HEADER_SIZE)

#  ifndef MAXIMUM_REPARSE_DATA_BUFFER_SIZE
#    define MAXIMUM_REPARSE_DATA_BUFFER_SIZE 16384
#  endif
#  ifndef IO_REPARSE_TAG_SYMLINK
#    define IO_REPARSE_TAG_SYMLINK (0xA000000CL)
#  endif
#  ifndef FSCTL_GET_REPARSE_POINT
#    define FSCTL_GET_REPARSE_POINT CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#  endif
#endif // !defined(Q_OS_WINCE)

QT_BEGIN_NAMESPACE

static QString readLink(const QString &link);

Q_CORE_EXPORT int qt_ntfs_permission_lookup = 0;

#if defined(Q_OS_WINCE)
static QString qfsPrivateCurrentDir = QLatin1String("");
// As none of the functions we try to resolve do exist on Windows CE
// we use QT_NO_LIBRARY to shorten everything up a little bit.
#define QT_NO_LIBRARY 1
#endif

#if !defined(QT_NO_LIBRARY)
QT_BEGIN_INCLUDE_NAMESPACE
typedef DWORD (WINAPI *PtrGetNamedSecurityInfoW)(LPWSTR, SE_OBJECT_TYPE, SECURITY_INFORMATION, PSID*, PSID*, PACL*, PACL*, PSECURITY_DESCRIPTOR*);
static PtrGetNamedSecurityInfoW ptrGetNamedSecurityInfoW = 0;
typedef BOOL (WINAPI *PtrLookupAccountSidW)(LPCWSTR, PSID, LPWSTR, LPDWORD, LPWSTR, LPDWORD, PSID_NAME_USE);
static PtrLookupAccountSidW ptrLookupAccountSidW = 0;
typedef VOID (WINAPI *PtrBuildTrusteeWithSidW)(PTRUSTEE_W, PSID);
static PtrBuildTrusteeWithSidW ptrBuildTrusteeWithSidW = 0;
typedef DWORD (WINAPI *PtrGetEffectiveRightsFromAclW)(PACL, PTRUSTEE_W, OUT PACCESS_MASK);
static PtrGetEffectiveRightsFromAclW ptrGetEffectiveRightsFromAclW = 0;
static TRUSTEE_W currentUserTrusteeW;
static TRUSTEE_W worldTrusteeW;

typedef BOOL (WINAPI *PtrGetUserProfileDirectoryW)(HANDLE, LPWSTR, LPDWORD);
static PtrGetUserProfileDirectoryW ptrGetUserProfileDirectoryW = 0;
QT_END_INCLUDE_NAMESPACE


void QFSFileEnginePrivate::resolveLibs()
{
    static bool triedResolve = false;
    if (!triedResolve) {
        // need to resolve the security info functions

        // protect initialization
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        // check triedResolve again, since another thread may have already
        // done the initialization
        if (triedResolve) {
            // another thread did initialize the security function pointers,
            // so we shouldn't do it again.
            return;
        }
#endif

        triedResolve = true;
#if !defined(Q_OS_WINCE)
        HINSTANCE advapiHnd = LoadLibraryW(L"advapi32");
        if (advapiHnd) {
            ptrGetNamedSecurityInfoW = (PtrGetNamedSecurityInfoW)GetProcAddress(advapiHnd, "GetNamedSecurityInfoW");
            ptrLookupAccountSidW = (PtrLookupAccountSidW)GetProcAddress(advapiHnd, "LookupAccountSidW");
            ptrBuildTrusteeWithSidW = (PtrBuildTrusteeWithSidW)GetProcAddress(advapiHnd, "BuildTrusteeWithSidW");
            ptrGetEffectiveRightsFromAclW = (PtrGetEffectiveRightsFromAclW)GetProcAddress(advapiHnd, "GetEffectiveRightsFromAclW");
        }
        if (ptrBuildTrusteeWithSidW) {
            // Create TRUSTEE for current user
            HANDLE hnd = ::GetCurrentProcess();
            HANDLE token = 0;
            if (::OpenProcessToken(hnd, TOKEN_QUERY, &token)) {
                TOKEN_USER tu;
                DWORD retsize;
                if (::GetTokenInformation(token, TokenUser, &tu, sizeof(tu), &retsize))
                    ptrBuildTrusteeWithSidW(&currentUserTrusteeW, tu.User.Sid);
                ::CloseHandle(token);
            }

            typedef BOOL (WINAPI *PtrAllocateAndInitializeSid)(PSID_IDENTIFIER_AUTHORITY, BYTE, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, PSID*);
            PtrAllocateAndInitializeSid ptrAllocateAndInitializeSid = (PtrAllocateAndInitializeSid)GetProcAddress(advapiHnd, "AllocateAndInitializeSid");
            typedef PVOID (WINAPI *PtrFreeSid)(PSID);
            PtrFreeSid ptrFreeSid = (PtrFreeSid)GetProcAddress(advapiHnd, "FreeSid");
            if (ptrAllocateAndInitializeSid && ptrFreeSid) {
                // Create TRUSTEE for Everyone (World)
                SID_IDENTIFIER_AUTHORITY worldAuth = { SECURITY_WORLD_SID_AUTHORITY };
                PSID pWorld = 0;
                if (ptrAllocateAndInitializeSid(&worldAuth, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pWorld))
                    ptrBuildTrusteeWithSidW(&worldTrusteeW, pWorld);
                ptrFreeSid(pWorld);
            }
        }
        HINSTANCE userenvHnd = LoadLibraryW(L"userenv");
        if (userenvHnd)
            ptrGetUserProfileDirectoryW = (PtrGetUserProfileDirectoryW)GetProcAddress(userenvHnd, "GetUserProfileDirectoryW");
#endif
    }
}
#endif // QT_NO_LIBRARY

// UNC functions NT
typedef DWORD (WINAPI *PtrNetShareEnum)(LPWSTR, DWORD, LPBYTE*, DWORD, LPDWORD, LPDWORD, LPDWORD);
static PtrNetShareEnum ptrNetShareEnum = 0;
typedef DWORD (WINAPI *PtrNetApiBufferFree)(LPVOID);
static PtrNetApiBufferFree ptrNetApiBufferFree = 0;
typedef struct _SHARE_INFO_1 {
    LPWSTR shi1_netname;
    DWORD shi1_type;
    LPWSTR shi1_remark;
} SHARE_INFO_1;


bool QFSFileEnginePrivate::resolveUNCLibs()
{
    static bool triedResolve = false;
    if (!triedResolve) {
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        if (triedResolve) {
            return ptrNetShareEnum && ptrNetApiBufferFree;
        }
#endif
        triedResolve = true;
#if !defined(Q_OS_WINCE)
        HINSTANCE hLib = LoadLibraryW(L"Netapi32");
        if (hLib) {
            ptrNetShareEnum = (PtrNetShareEnum)GetProcAddress(hLib, "NetShareEnum");
            if (ptrNetShareEnum)
                ptrNetApiBufferFree = (PtrNetApiBufferFree)GetProcAddress(hLib, "NetApiBufferFree");
        }
#endif
    }
    return ptrNetShareEnum && ptrNetApiBufferFree;
}

bool QFSFileEnginePrivate::uncListSharesOnServer(const QString &server, QStringList *list)
{
    if (resolveUNCLibs()) {
        SHARE_INFO_1 *BufPtr, *p;
        DWORD res;
        DWORD er = 0, tr = 0, resume = 0, i;
        do {
            res = ptrNetShareEnum((wchar_t*)server.utf16(), 1, (LPBYTE *)&BufPtr, DWORD(-1), &er, &tr, &resume);
            if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA) {
                p = BufPtr;
                for (i = 1; i <= er; ++i) {
                    if (list && p->shi1_type == 0)
                        list->append(QString::fromWCharArray(p->shi1_netname));
                    p++;
                }
            }
            ptrNetApiBufferFree(BufPtr);
        } while (res == ERROR_MORE_DATA);
        return res == ERROR_SUCCESS;
    }
    return false;
}

static bool isUncRoot(const QString &server)
{
    QString localPath = QDir::toNativeSeparators(server);
    if (!localPath.startsWith(QLatin1String("\\\\")))
        return false;

    int idx = localPath.indexOf(QLatin1Char('\\'), 2);
    if (idx == -1 || idx + 1 == localPath.length())
        return true;

    localPath = localPath.right(localPath.length() - idx - 1).trimmed();
    return localPath.isEmpty();
}

#if !defined(Q_OS_WINCE)
static inline bool isUncPath(const QString &path)
{
    // Starts with \\, but not \\.
    return (path.startsWith(QLatin1String("\\\\"))
            && path.size() > 2 && path.at(2) != QLatin1Char('.'));
}
#endif

static inline bool isRelativePath(const QString &path)
{
    // drive, e.g. "a:", or UNC root, e.q. "//"
    return !(path.startsWith(QLatin1Char('/'))
           || (path.length() >= 2
           && ((path.at(0).isLetter() && path.at(1) == QLatin1Char(':'))
           || (path.at(0) == QLatin1Char('/') && path.at(1) == QLatin1Char('/')))));
}

static QString fixIfRelativeUncPath(const QString &path)
{
    if (isRelativePath(path)) {
        QString currentPath = QDir::currentPath() + QLatin1Char('/');
        if (currentPath.startsWith(QLatin1String("//")))
            return QString(path).prepend(currentPath);
    }
    return path;
}

// can be //server or //server/share
static bool uncShareExists(const QString &server)
{
    QStringList parts = server.split(QLatin1Char('\\'), QString::SkipEmptyParts);
    if (parts.count()) {
        QStringList shares;
        if (QFSFileEnginePrivate::uncListSharesOnServer(QLatin1String("\\\\") + parts.at(0), &shares))
            return parts.count() >= 2 ? shares.contains(parts.at(1), Qt::CaseInsensitive) : true;
    }
    return false;
}

static inline bool isDriveRoot(const QString &path)
{
    return (path.length() == 3
           && path.at(0).isLetter() && path.at(1) == QLatin1Char(':')
           && path.at(2) == QLatin1Char('/'));
}

static QString nativeAbsoluteFilePath(const QString &path)
{
    QString absPath;
#if !defined(Q_OS_WINCE)
    QVarLengthArray<wchar_t, MAX_PATH> buf(qMax(MAX_PATH, path.size() + 1));
    wchar_t *fileName = 0;
    DWORD retLen = GetFullPathName((wchar_t*)path.utf16(), buf.size(), buf.data(), &fileName);
    if (retLen > (DWORD)buf.size()) {
        buf.resize(retLen);
        retLen = GetFullPathName((wchar_t*)path.utf16(), buf.size(), buf.data(), &fileName);
    }
    if (retLen != 0)
        absPath = QString::fromWCharArray(buf.data(), retLen);
#else
    if (path.startsWith(QLatin1Char('/')) || path.startsWith(QLatin1Char('\\')))
        absPath = QDir::toNativeSeparators(path);
    else
        absPath = QDir::toNativeSeparators(QDir::cleanPath(qfsPrivateCurrentDir + QLatin1Char('/') + path));
#endif
    // This is really ugly, but GetFullPathName strips off whitespace at the end.
    // If you for instance write ". " in the lineedit of QFileDialog,
    // (which is an invalid filename) this function will strip the space off and viola,
    // the file is later reported as existing. Therefore, we re-add the whitespace that
    // was at the end of path in order to keep the filename invalid.
    if (!path.isEmpty() && path.at(path.size() - 1) == QLatin1Char(' '))
        absPath.append(QLatin1Char(' '));
    return absPath;
}

/*!
    \internal
*/
QString QFSFileEnginePrivate::longFileName(const QString &path)
{
    if (path.startsWith(QLatin1String("\\\\.\\")))
        return path;

    QString absPath = nativeAbsoluteFilePath(path);
#if !defined(Q_OS_WINCE)
    QString prefix = QLatin1String("\\\\?\\");
    if (isUncPath(absPath)) {
        prefix.append(QLatin1String("UNC\\")); // "\\\\?\\UNC\\"
        absPath.remove(0, 2);
    }
    return prefix + absPath;
#else
    return absPath;
#endif
}

/*
    \internal
*/
void QFSFileEnginePrivate::nativeInitFileName()
{
    QString path = longFileName(QDir::toNativeSeparators(fixIfRelativeUncPath(filePath)));
    nativeFilePath = QByteArray((const char *)path.utf16(), path.size() * 2 + 1);
}

/*
    \internal
*/
bool QFSFileEnginePrivate::nativeOpen(QIODevice::OpenMode openMode)
{
    Q_Q(QFSFileEngine);

    // All files are opened in share mode (both read and write).
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    int accessRights = 0;
    if (openMode & QIODevice::ReadOnly)
        accessRights |= GENERIC_READ;
    if (openMode & QIODevice::WriteOnly)
        accessRights |= GENERIC_WRITE;

    SECURITY_ATTRIBUTES securityAtts = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };

    // WriteOnly can create files, ReadOnly cannot.
    DWORD creationDisp = (openMode & QIODevice::WriteOnly) ? OPEN_ALWAYS : OPEN_EXISTING;

    // Create the file handle.
    fileHandle = CreateFile((const wchar_t*)nativeFilePath.constData(),
                            accessRights,
                            shareMode,
                            &securityAtts,
                            creationDisp,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    // Bail out on error.
    if (fileHandle == INVALID_HANDLE_VALUE) {
        q->setError(QFile::OpenError, qt_error_string());
        return false;
    }

    // Truncate the file after successfully opening it if Truncate is passed.
    if (openMode & QIODevice::Truncate)
        q->setSize(0);

    return true;
}

/*
    \internal
*/
bool QFSFileEnginePrivate::nativeClose()
{
    Q_Q(QFSFileEngine);
    if (fh || fd != -1) {
        // stdlib / stdio mode.
        return closeFdFh();
    }

    // Windows native mode.
    bool ok = true;
    if ((fileHandle == INVALID_HANDLE_VALUE || !::CloseHandle(fileHandle))) {
        q->setError(QFile::UnspecifiedError, qt_error_string());
        ok = false;
    }
    fileHandle = INVALID_HANDLE_VALUE;
    cachedFd = -1;              // gets closed by CloseHandle above

    return ok;
}

/*
    \internal
*/
bool QFSFileEnginePrivate::nativeFlush()
{
    if (fh) {
        // Buffered stdlib mode.
        return flushFh();
    }
    if (fd != -1) {
        // Unbuffered stdio mode; always succeeds (no buffer).
        return true;
    }

    // Windows native mode; flushing is
    // unnecessary. FlushFileBuffers(), the equivalent of sync() or
    // fsync() on Unix, does a low-level flush to the disk, and we
    // don't expose an API for this.
    return true;
}

/*
    \internal
*/
qint64 QFSFileEnginePrivate::nativeSize() const
{
    Q_Q(const QFSFileEngine);
    QFSFileEngine *thatQ = const_cast<QFSFileEngine *>(q);

    // ### Don't flush; for buffered files, we should get away with ftell.
    thatQ->flush();

#if !defined(Q_OS_WINCE)
    // stdlib/stdio mode.
    if (fh || fd != -1) {
        qint64 fileSize = _filelengthi64(fh ? QT_FILENO(fh) : fd);
        if (fileSize == -1) {
            fileSize = 0;
            thatQ->setError(QFile::UnspecifiedError, qt_error_string(errno));
        }
        return fileSize;
    }
#else // Q_OS_WINCE
    // Buffered stdlib mode.
    if (fh) {
        QT_OFF_T oldPos = QT_FTELL(fh);
        QT_FSEEK(fh, 0, SEEK_END);
        qint64 fileSize = (qint64)QT_FTELL(fh);
        QT_FSEEK(fh, oldPos, SEEK_SET);
        if (fileSize == -1) {
            fileSize = 0;
            thatQ->setError(QFile::UnspecifiedError, qt_error_string(errno));
        }
        return fileSize;
    }
#endif

    // Not-open mode, where the file name is known: We'll check the
    // file system directly.
    if (openMode == QIODevice::NotOpen && !nativeFilePath.isEmpty()) {
        WIN32_FILE_ATTRIBUTE_DATA attribData;
        bool ok = ::GetFileAttributesEx((const wchar_t*)nativeFilePath.constData(),
                                        GetFileExInfoStandard, &attribData);
        if (!ok) {
            int errorCode = GetLastError();
            if (errorCode == ERROR_ACCESS_DENIED || errorCode == ERROR_SHARING_VIOLATION) {
                QByteArray path = nativeFilePath;
                // path for the FindFirstFile should not end with a trailing slash
                while (path.endsWith('\\'))
                    path.chop(1);

                // FindFirstFile can not handle drives
                if (!path.endsWith(':')) {
                    WIN32_FIND_DATA findData;
                    HANDLE hFind = ::FindFirstFile((const wchar_t*)path.constData(),
                                                   &findData);
                    if (hFind != INVALID_HANDLE_VALUE) {
                        ::FindClose(hFind);
                        ok = true;
                        attribData.nFileSizeHigh = findData.nFileSizeHigh;
                        attribData.nFileSizeLow = findData.nFileSizeLow;
                    }
                }
            }
        }
        if (ok) {
            qint64 size = attribData.nFileSizeHigh;
            size <<= 32;
            size += attribData.nFileSizeLow;
            return size;
        }
        thatQ->setError(QFile::UnspecifiedError, qt_error_string());
        return 0;
    }

#if defined(Q_OS_WINCE)
    // Unbuffed stdio mode
    if (fd != -1) {
        thatQ->setError(QFile::UnspecifiedError, QLatin1String("Not implemented!"));
        return 0;
    }
#endif

    // Windows native mode.
    if (fileHandle == INVALID_HANDLE_VALUE)
        return 0;

    BY_HANDLE_FILE_INFORMATION fileInfo;
    if (!GetFileInformationByHandle(fileHandle, &fileInfo)) {
        thatQ->setError(QFile::UnspecifiedError, qt_error_string());
        return 0;
    }

    qint64 size = fileInfo.nFileSizeHigh;
    size <<= 32;
    size += fileInfo.nFileSizeLow;
    return size;
}

/*
    \internal
*/
qint64 QFSFileEnginePrivate::nativePos() const
{
    Q_Q(const QFSFileEngine);
    QFSFileEngine *thatQ = const_cast<QFSFileEngine *>(q);

    if (fh || fd != -1) {
        // stdlib / stido mode.
        return posFdFh();
    }

    // Windows native mode.
    if (fileHandle == INVALID_HANDLE_VALUE)
        return 0;

#if !defined(Q_OS_WINCE)
    LARGE_INTEGER currentFilePos;
    LARGE_INTEGER offset;
    offset.QuadPart = 0;
    if (!::SetFilePointerEx(fileHandle, offset, &currentFilePos, FILE_CURRENT)) {
        thatQ->setError(QFile::UnspecifiedError, qt_error_string());
        return 0;
    }

    return qint64(currentFilePos.QuadPart);
#else
    LARGE_INTEGER filepos;
    filepos.HighPart = 0;
    DWORD newFilePointer = SetFilePointer(fileHandle, 0, &filepos.HighPart, FILE_CURRENT);
    if (newFilePointer == 0xFFFFFFFF && GetLastError() != NO_ERROR) {
        thatQ->setError(QFile::UnspecifiedError, qt_error_string());
        return 0;
    }

    filepos.LowPart = newFilePointer;
    return filepos.QuadPart;
#endif
}

/*
    \internal
*/
bool QFSFileEnginePrivate::nativeSeek(qint64 pos)
{
    Q_Q(QFSFileEngine);

    if (fh || fd != -1) {
        // stdlib / stdio mode.
        return seekFdFh(pos);
    }

#if !defined(Q_OS_WINCE)
    LARGE_INTEGER currentFilePos;
    LARGE_INTEGER offset;
    offset.QuadPart = pos;
    if (!::SetFilePointerEx(fileHandle, offset, &currentFilePos, FILE_BEGIN)) {
        q->setError(QFile::UnspecifiedError, qt_error_string());
        return false;
    }

    return true;
#else
    DWORD newFilePointer;
    LARGE_INTEGER *li = reinterpret_cast<LARGE_INTEGER*>(&pos);
    newFilePointer = SetFilePointer(fileHandle, li->LowPart, &li->HighPart, FILE_BEGIN);
    if (newFilePointer == 0xFFFFFFFF && GetLastError() != NO_ERROR) {
        q->setError(QFile::PositionError, qt_error_string());
        return false;
    }

    return true;
#endif
}

/*
    \internal
*/
qint64 QFSFileEnginePrivate::nativeRead(char *data, qint64 maxlen)
{
    Q_Q(QFSFileEngine);

    if (fh || fd != -1) {
        // stdio / stdlib mode.
        if (fh && nativeIsSequential() && feof(fh)) {
            q->setError(QFile::ReadError, qt_error_string(int(errno)));
            return -1;
        }

        return readFdFh(data, maxlen);
    }

    // Windows native mode.
    if (fileHandle == INVALID_HANDLE_VALUE)
        return -1;

    DWORD bytesToRead = DWORD(maxlen); // <- lossy

    // Reading on Windows fails with ERROR_NO_SYSTEM_RESOURCES when
    // the chunks are too large, so we limit the block size to 32MB.
    static const DWORD maxBlockSize = 32 * 1024 * 1024;

    qint64 totalRead = 0;
    do {
        DWORD blockSize = qMin<DWORD>(bytesToRead, maxBlockSize);
        DWORD bytesRead;
        if (!ReadFile(fileHandle, data + totalRead, blockSize, &bytesRead, NULL)) {
            if (totalRead == 0) {
                // Note: only return failure if the first ReadFile fails.
                q->setError(QFile::ReadError, qt_error_string());
                return -1;
            }
            break;
        }
        if (bytesRead == 0)
            break;
        totalRead += bytesRead;
        bytesToRead -= bytesRead;
    } while (totalRead < maxlen);
    return qint64(totalRead);
}

/*
    \internal
*/
qint64 QFSFileEnginePrivate::nativeReadLine(char *data, qint64 maxlen)
{
    Q_Q(QFSFileEngine);

    if (fh || fd != -1) {
        // stdio / stdlib mode.
        return readLineFdFh(data, maxlen);
    }

    // Windows native mode.
    if (fileHandle == INVALID_HANDLE_VALUE)
        return -1;

    // ### No equivalent in Win32?
    return q->QAbstractFileEngine::readLine(data, maxlen);
}

/*
    \internal
*/
qint64 QFSFileEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    Q_Q(QFSFileEngine);

    if (fh || fd != -1) {
        // stdio / stdlib mode.
        return writeFdFh(data, len);
    }

    // Windows native mode.
    if (fileHandle == INVALID_HANDLE_VALUE)
        return -1;

    qint64 bytesToWrite = DWORD(len); // <- lossy

    // Writing on Windows fails with ERROR_NO_SYSTEM_RESOURCES when
    // the chunks are too large, so we limit the block size to 32MB.
    static const DWORD maxBlockSize = 32 * 1024 * 1024;

    qint64 totalWritten = 0;
    do {
        DWORD blockSize = qMin<DWORD>(bytesToWrite, maxBlockSize);
        DWORD bytesWritten;
        if (!WriteFile(fileHandle, data + totalWritten, blockSize, &bytesWritten, NULL)) {
            if (totalWritten == 0) {
                // Note: Only return error if the first WriteFile failed.
                q->setError(QFile::WriteError, qt_error_string());
                return -1;
            }
            break;
        }
        if (bytesWritten == 0)
            break;
        totalWritten += bytesWritten;
        bytesToWrite -= bytesWritten;
    } while (totalWritten < len);
    return qint64(totalWritten);
}

/*
    \internal
*/
int QFSFileEnginePrivate::nativeHandle() const
{
    if (fh || fd != -1)
        return fh ? QT_FILENO(fh) : fd;
#ifndef Q_OS_WINCE
    if (cachedFd != -1)
        return cachedFd;

    int flags = 0;
    if (openMode & QIODevice::Append)
        flags |= _O_APPEND;
    if (!(openMode & QIODevice::WriteOnly))
        flags |= _O_RDONLY;
    cachedFd = _open_osfhandle((intptr_t) fileHandle, flags);
    return cachedFd;
#else
    return -1;
#endif
}

/*
    \internal
*/
bool QFSFileEnginePrivate::nativeIsSequential() const
{
#if !defined(Q_OS_WINCE)
    HANDLE handle = fileHandle;
    if (fh || fd != -1)
        handle = (HANDLE)_get_osfhandle(fh ? QT_FILENO(fh) : fd);
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    DWORD fileType = GetFileType(handle);
    return (fileType == FILE_TYPE_CHAR)
            || (fileType == FILE_TYPE_PIPE);
#else
    return false;
#endif
}

bool QFSFileEngine::remove()
{
    Q_D(QFSFileEngine);
    bool ret = ::DeleteFile((wchar_t*)QFSFileEnginePrivate::longFileName(d->filePath).utf16()) != 0;
    if (!ret)
        setError(QFile::RemoveError, qt_error_string());
    return ret;
}

bool QFSFileEngine::copy(const QString &copyName)
{
    Q_D(QFSFileEngine);
    bool ret = ::CopyFile((wchar_t*)QFSFileEnginePrivate::longFileName(d->filePath).utf16(),
                          (wchar_t*)QFSFileEnginePrivate::longFileName(copyName).utf16(), true) != 0;
    if (!ret)
        setError(QFile::CopyError, qt_error_string());
    return ret;
}

bool QFSFileEngine::rename(const QString &newName)
{
    Q_D(QFSFileEngine);
    bool ret = ::MoveFile((wchar_t*)QFSFileEnginePrivate::longFileName(d->filePath).utf16(),
                          (wchar_t*)QFSFileEnginePrivate::longFileName(newName).utf16()) != 0;
    if (!ret)
        setError(QFile::RenameError, qt_error_string());
    return ret;
}

static inline bool mkDir(const QString &path)
{
#if defined(Q_OS_WINCE)
    // Unfortunately CreateDirectory returns true for paths longer than
    // 256, but does not create a directory. It starts to fail, when
    // path length > MAX_PATH, which is 260 usually on CE.
    // This only happens on a Windows Mobile device. Windows CE seems
    // not to be affected by this.
    static int platformId = 0;
    if (platformId == 0) {
        wchar_t platformString[64];
        if (SystemParametersInfo(SPI_GETPLATFORMTYPE, sizeof(platformString)/sizeof(*platformString),platformString,0)) {
            if (0 == wcscmp(platformString, L"PocketPC") || 0 == wcscmp(platformString, L"Smartphone"))
                platformId = 1;
            else
                platformId = 2;
        }
    }
    if (platformId == 1 && QFSFileEnginePrivate::longFileName(path).size() > 256)
        return false;
#endif
    return ::CreateDirectory((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16(), 0);
}

static inline bool rmDir(const QString &path)
{
    return ::RemoveDirectory((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16());
}

static bool isDirPath(const QString &dirPath, bool *existed)
{
    QString path = dirPath;
    if (path.length() == 2 && path.at(1) == QLatin1Char(':'))
        path += QLatin1Char('\\');

    DWORD fileAttrib = ::GetFileAttributes((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16());
    if (fileAttrib == INVALID_FILE_ATTRIBUTES) {
        int errorCode = GetLastError();
        if (errorCode == ERROR_ACCESS_DENIED || errorCode == ERROR_SHARING_VIOLATION) {
            // path for the FindFirstFile should not end with a trailing slash
            while (path.endsWith(QLatin1Char('\\')))
                path.chop(1);

            // FindFirstFile can not handle drives
            if (!path.endsWith(QLatin1Char(':'))) {
                WIN32_FIND_DATA findData;
                HANDLE hFind = ::FindFirstFile((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16(),
                                               &findData);
                if (hFind != INVALID_HANDLE_VALUE) {
                    ::FindClose(hFind);
                    fileAttrib = findData.dwFileAttributes;
                }
            }
        }
    }

    if (existed)
        *existed = fileAttrib != INVALID_FILE_ATTRIBUTES;

    if (fileAttrib == INVALID_FILE_ATTRIBUTES)
        return false;

    return fileAttrib & FILE_ATTRIBUTE_DIRECTORY;
}

bool QFSFileEngine::mkdir(const QString &name, bool createParentDirectories) const
{
    QString dirName = name;
    if (createParentDirectories) {
        dirName = QDir::toNativeSeparators(QDir::cleanPath(dirName));
        // We spefically search for / so \ would break it..
        int oldslash = -1;
        if (dirName.startsWith(QLatin1String("\\\\"))) {
            // Don't try to create the root path of a UNC path;
            // CreateDirectory() will just return ERROR_INVALID_NAME.
            for (int i = 0; i < dirName.size(); ++i) {
                if (dirName.at(i) != QDir::separator()) {
                    oldslash = i;
                    break;
                }
            }
            if (oldslash != -1)
                oldslash = dirName.indexOf(QDir::separator(), oldslash);
        }
        for (int slash=0; slash != -1; oldslash = slash) {
            slash = dirName.indexOf(QDir::separator(), oldslash+1);
            if (slash == -1) {
                if (oldslash == dirName.length())
                    break;
                slash = dirName.length();
            }
            if (slash) {
                QString chunk = dirName.left(slash);
                bool existed = false;
                if (!isDirPath(chunk, &existed)) {
                    if (!existed) {
                        if (!mkDir(chunk))
                            return false;
                    } else {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    return mkDir(name);
}

bool QFSFileEngine::rmdir(const QString &name, bool recurseParentDirectories) const
{
    QString dirName = name;
    if (recurseParentDirectories) {
        dirName = QDir::toNativeSeparators(QDir::cleanPath(dirName));
        for (int oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash) {
            QString chunk = dirName.left(slash);
            if (chunk.length() == 2 && chunk.at(0).isLetter() && chunk.at(1) == QLatin1Char(':'))
                break;
            if (!isDirPath(chunk, 0))
                return false;
            if (!rmDir(chunk))
                return oldslash != 0;
            slash = dirName.lastIndexOf(QDir::separator(), oldslash-1);
        }
        return true;
    }
    return rmDir(name);
}

bool QFSFileEngine::caseSensitive() const
{
    return false;
}

bool QFSFileEngine::setCurrentPath(const QString &path)
{
    if (!QDir(path).exists())
        return false;

#if !defined(Q_OS_WINCE)
    return ::SetCurrentDirectory((wchar_t*)path.utf16()) != 0;
#else
    qfsPrivateCurrentDir = QFSFileEnginePrivate::longFileName(path);
    return true;
#endif
}

QString QFSFileEngine::currentPath(const QString &fileName)
{
#if !defined(Q_OS_WINCE)
    QString ret;
    //if filename is a drive: then get the pwd of that drive
    if (fileName.length() >= 2 &&
        fileName.at(0).isLetter() && fileName.at(1) == QLatin1Char(':')) {
        int drv = fileName.toUpper().at(0).toLatin1() - 'A' + 1;
        if (_getdrive() != drv) {
            wchar_t buf[PATH_MAX];
            ::_wgetdcwd(drv, buf, PATH_MAX);
            ret = QString::fromWCharArray(buf);
        }
    }
    if (ret.isEmpty()) {
        //just the pwd
        DWORD size = 0;
        wchar_t currentName[PATH_MAX];
        size = ::GetCurrentDirectory(PATH_MAX, currentName);
        if (size != 0) {
            if (size > PATH_MAX) {
                wchar_t *newCurrentName = new wchar_t[size];
                if (::GetCurrentDirectory(PATH_MAX, newCurrentName) != 0)
                    ret = QString::fromWCharArray(newCurrentName);
                delete [] newCurrentName;
            } else {
                ret = QString::fromWCharArray(currentName);
            }
        }
    }
    if (ret.length() >= 2 && ret[1] == QLatin1Char(':'))
        ret[0] = ret.at(0).toUpper(); // Force uppercase drive letters.
    return QDir::fromNativeSeparators(ret);
#else
    Q_UNUSED(fileName);
    if (qfsPrivateCurrentDir.isEmpty())
        qfsPrivateCurrentDir = QCoreApplication::applicationDirPath();

    return QDir::fromNativeSeparators(qfsPrivateCurrentDir);
#endif
}

QString QFSFileEngine::homePath()
{
    QString ret;
#if !defined(QT_NO_LIBRARY)
    QFSFileEnginePrivate::resolveLibs();
    if (ptrGetUserProfileDirectoryW) {
        HANDLE hnd = ::GetCurrentProcess();
        HANDLE token = 0;
        BOOL ok = ::OpenProcessToken(hnd, TOKEN_QUERY, &token);
        if (ok) {
            DWORD dwBufferSize = 0;
            // First call, to determine size of the strings (with '\0').
            ok = ::ptrGetUserProfileDirectoryW(token, NULL, &dwBufferSize);
            if (!ok && dwBufferSize != 0) {        // We got the required buffer size
                wchar_t *userDirectory = new wchar_t[dwBufferSize];
                // Second call, now we can fill the allocated buffer.
                ok = ::ptrGetUserProfileDirectoryW(token, userDirectory, &dwBufferSize);
                if (ok)
                    ret = QString::fromWCharArray(userDirectory);

                delete [] userDirectory;
            }
            ::CloseHandle(token);
        }
                }
#endif
    if (ret.isEmpty() || !QFile::exists(ret)) {
        ret = QString::fromLocal8Bit(qgetenv("USERPROFILE").constData());
        if (ret.isEmpty() || !QFile::exists(ret)) {
            ret = QString::fromLocal8Bit(qgetenv("HOMEDRIVE").constData()) + QString::fromLocal8Bit(qgetenv("HOMEPATH").constData());
            if (ret.isEmpty() || !QFile::exists(ret)) {
                ret = QString::fromLocal8Bit(qgetenv("HOME").constData());
                if (ret.isEmpty() || !QFile::exists(ret)) {
#if defined(Q_OS_WINCE)
                    ret = QLatin1String("\\My Documents");
                    if (!QFile::exists(ret))
#endif
                    ret = rootPath();
                }
            }
        }
    }
    return QDir::fromNativeSeparators(ret);
}

QString QFSFileEngine::rootPath()
{
#if defined(Q_OS_WINCE)
    QString ret = QLatin1String("/");
#elif defined(Q_FS_FAT)
    QString ret = QString::fromLatin1(qgetenv("SystemDrive").constData());
    if (ret.isEmpty())
        ret = QLatin1String("c:");
    ret.append(QLatin1Char('/'));
#elif defined(Q_OS_OS2EMX)
    char dir[4];
    _abspath(dir, QLatin1String("/"), _MAX_PATH);
    QString ret(dir);
#endif
    return ret;
}

QString QFSFileEngine::tempPath()
{
    QString ret;
    {
        wchar_t tempPath[MAX_PATH];
        if (GetTempPath(MAX_PATH, tempPath))
            ret = QString::fromWCharArray(tempPath);
        if (!ret.isEmpty()) {
            while (ret.endsWith(QLatin1Char('\\')))
                ret.chop(1);
            ret = QDir::fromNativeSeparators(ret);
        }
    }
    if (ret.isEmpty()) {
#if !defined(Q_OS_WINCE)
        ret = QLatin1String("c:/tmp");
#else
        ret = QLatin1String("/Temp");
#endif
    }
    return ret;
}

QFileInfoList QFSFileEngine::drives()
{
    QFileInfoList ret;
#if !defined(Q_OS_WINCE)
#if defined(Q_OS_WIN32)
    quint32 driveBits = (quint32) GetLogicalDrives() & 0x3ffffff;
#elif defined(Q_OS_OS2EMX)
    quint32 driveBits, cur;
    if (DosQueryCurrentDisk(&cur, &driveBits) != NO_ERROR)
        exit(1);
    driveBits &= 0x3ffffff;
#endif
    char driveName[] = "A:/";

    while (driveBits) {
        if (driveBits & 1)
            ret.append(QFileInfo(QLatin1String(driveName)));
        driveName[0]++;
        driveBits = driveBits >> 1;
    }
    return ret;
#else
    ret.append(QFileInfo(QLatin1String("/")));
    return ret;
#endif
}

bool QFSFileEnginePrivate::doStat() const
{
    if (!tried_stat) {
        tried_stat = true;
        could_stat = false;

        if (filePath.isEmpty())
            return could_stat;

        QString fname = filePath.endsWith(QLatin1String(".lnk")) ? readLink(filePath) : filePath;
        fname = fixIfRelativeUncPath(fname);

        UINT oldmode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

        if (fd != -1) {
#if !defined(Q_OS_WINCE)
            HANDLE fh = (HANDLE)_get_osfhandle(fd);
            if (fh != INVALID_HANDLE_VALUE) {
                BY_HANDLE_FILE_INFORMATION fileInfo;
                if (GetFileInformationByHandle(fh, &fileInfo)) {
                    could_stat = true;
                    fileAttrib = fileInfo.dwFileAttributes;
                }
            }
#else
            DWORD tmpAttributes = GetFileAttributes((wchar_t*)QFSFileEnginePrivate::longFileName(fname).utf16());
            if (tmpAttributes != -1) {
                fileAttrib = tmpAttributes;
                could_stat = true;
            }
#endif
        } else {
            fileAttrib = GetFileAttributes((wchar_t*)QFSFileEnginePrivate::longFileName(fname).utf16());
            if (fileAttrib == INVALID_FILE_ATTRIBUTES) {
                int errorCode = GetLastError();
                if (errorCode == ERROR_ACCESS_DENIED || errorCode == ERROR_SHARING_VIOLATION) {
                    QString path = QDir::toNativeSeparators(fname);
                    // path for the FindFirstFile should not end with a trailing slash
                    while (path.endsWith(QLatin1Char('\\')))
                        path.chop(1);

                    // FindFirstFile can not handle drives
                    if (!path.endsWith(QLatin1Char(':'))) {
                        WIN32_FIND_DATA findData;
                        HANDLE hFind = ::FindFirstFile((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16(),
                                                       &findData);
                        if (hFind != INVALID_HANDLE_VALUE) {
                            ::FindClose(hFind);
                            fileAttrib = findData.dwFileAttributes;
                        }
                    }
                }
            }
            could_stat = fileAttrib != INVALID_FILE_ATTRIBUTES;
            if (!could_stat) {
#if !defined(Q_OS_WINCE)
                if (isDriveRoot(fname)) {
                    // a valid drive ??
                    DWORD drivesBitmask = ::GetLogicalDrives();
                    int drivebit = 1 << (fname.at(0).toUpper().unicode() - QLatin1Char('A').unicode());
                    if (drivesBitmask & drivebit) {
                        fileAttrib = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM;
                        could_stat = true;
                    }
                } else {
#endif
                    QString path = QDir::toNativeSeparators(fname);
                    bool is_dir = false;
                    if (path.startsWith(QLatin1String("\\\\"))) {
                        // UNC - stat doesn't work for all cases (Windows bug)
                        int s = path.indexOf(path.at(0),2);
                        if (s > 0) {
                            // "\\server\..."
                            s = path.indexOf(path.at(0),s+1);
                            if (s > 0) {
                                // "\\server\share\..."
                                if (s == path.size() - 1) {
                                    // "\\server\share\"
                                    is_dir = true;
                                } else {
                                    // "\\server\share\notfound"
                                }
                            } else {
                                // "\\server\share"
                                is_dir = true;
                            }
                        } else {
                            // "\\server"
                            is_dir = true;
                        }
                    }
                    if (is_dir && uncShareExists(path)) {
                        // looks like a UNC dir, is a dir.
                        fileAttrib = FILE_ATTRIBUTE_DIRECTORY;
                        could_stat = true;
                    }
#if !defined(Q_OS_WINCE)
                }
#endif
            }
        }

        SetErrorMode(oldmode);
    }
    return could_stat;
}


static QString readSymLink(const QString &link)
{
    QString result;
#if !defined(Q_OS_WINCE)
    HANDLE handle = CreateFile((wchar_t*)QFSFileEnginePrivate::longFileName(link).utf16(),
                               FILE_READ_EA,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               0,
                               OPEN_EXISTING,
                               FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
                               0);
    if (handle != INVALID_HANDLE_VALUE) {
        DWORD bufsize = MAXIMUM_REPARSE_DATA_BUFFER_SIZE;
        REPARSE_DATA_BUFFER *rdb = (REPARSE_DATA_BUFFER*)qMalloc(bufsize);
        DWORD retsize = 0;
        if (::DeviceIoControl(handle, FSCTL_GET_REPARSE_POINT, 0, 0, rdb, bufsize, &retsize, 0)) {
            if (rdb->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
                int length = rdb->MountPointReparseBuffer.SubstituteNameLength / sizeof(wchar_t);
                int offset = rdb->MountPointReparseBuffer.SubstituteNameOffset / sizeof(wchar_t);
                const wchar_t* PathBuffer = &rdb->MountPointReparseBuffer.PathBuffer[offset];
                result = QString::fromWCharArray(PathBuffer, length);
            } else {
                int length = rdb->SymbolicLinkReparseBuffer.SubstituteNameLength / sizeof(wchar_t);
                int offset = rdb->SymbolicLinkReparseBuffer.SubstituteNameOffset / sizeof(wchar_t);
                const wchar_t* PathBuffer = &rdb->SymbolicLinkReparseBuffer.PathBuffer[offset];
                result = QString::fromWCharArray(PathBuffer, length);
            }
            // cut-off "//?/" and "/??/"
            if (result.size() > 4 && result.at(0) == QLatin1Char('\\') && result.at(2) == QLatin1Char('?') && result.at(3) == QLatin1Char('\\'))
                result = result.mid(4);
        }
        qFree(rdb);
        CloseHandle(handle);
    }
#else
    Q_UNUSED(link);
#endif // Q_OS_WINCE
    return result;
}

static QString readLink(const QString &link)
{
#if !defined(Q_OS_WINCE)
#if !defined(QT_NO_LIBRARY) && !defined(Q_CC_MWERKS)
    QString ret;

    bool neededCoInit = false;
    IShellLink *psl;                            // pointer to IShellLink i/f
    WIN32_FIND_DATA wfd;
    wchar_t szGotPath[MAX_PATH];

    // Get pointer to the IShellLink interface.
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);

    if (hres == CO_E_NOTINITIALIZED) { // COM was not initialized
        neededCoInit = true;
        CoInitialize(NULL);
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                    IID_IShellLink, (LPVOID *)&psl);
    }
    if (SUCCEEDED(hres)) {    // Get pointer to the IPersistFile interface.
        IPersistFile *ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);
        if (SUCCEEDED(hres))  {
            hres = ppf->Load((LPOLESTR)link.utf16(), STGM_READ);
            //The original path of the link is retrieved. If the file/folder
            //was moved, the return value still have the old path.
            if (SUCCEEDED(hres)) {
                if (psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_UNCPRIORITY) == NOERROR)
                    ret = QString::fromWCharArray(szGotPath);
            }
            ppf->Release();
        }
        psl->Release();
    }
    if (neededCoInit)
        CoUninitialize();

    return ret;
#else
    Q_UNUSED(link);
    return QString();
#endif // QT_NO_LIBRARY
#else
    wchar_t target[MAX_PATH];
    QString result;
    if (SHGetShortcutTarget((wchar_t*)QFileInfo(link).absoluteFilePath().replace(QLatin1Char('/'),QLatin1Char('\\')).utf16(), target, MAX_PATH)) {
        result = QString::fromWCharArray(target);
        if (result.startsWith(QLatin1Char('"')))
            result.remove(0,1);
        if (result.endsWith(QLatin1Char('"')))
            result.remove(result.size()-1,1);
    }
    return result;
#endif // Q_OS_WINCE
}

bool QFSFileEngine::link(const QString &newName)
{
#if !defined(Q_OS_WINCE)
#if !defined(QT_NO_LIBRARY) && !defined(Q_CC_MWERKS)
    bool ret = false;

    QString linkName = newName;
    //### assume that they add .lnk

    IShellLink *psl;
    bool neededCoInit = false;

    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);

    if (hres == CO_E_NOTINITIALIZED) { // COM was not initialized
        neededCoInit = true;
        CoInitialize(NULL);
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
    }

    if (SUCCEEDED(hres)) {
        hres = psl->SetPath((wchar_t *)fileName(AbsoluteName).replace(QLatin1Char('/'), QLatin1Char('\\')).utf16());
        if (SUCCEEDED(hres)) {
            hres = psl->SetWorkingDirectory((wchar_t *)fileName(AbsolutePathName).replace(QLatin1Char('/'), QLatin1Char('\\')).utf16());
            if (SUCCEEDED(hres)) {
                IPersistFile *ppf;
                hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
                if (SUCCEEDED(hres)) {
                    hres = ppf->Save((wchar_t*)linkName.utf16(), TRUE);
                    if (SUCCEEDED(hres))
                         ret = true;
                    ppf->Release();
                }
            }
        }
        psl->Release();
    }
    if (!ret)
        setError(QFile::RenameError, qt_error_string());

    if (neededCoInit)
        CoUninitialize();

    return ret;
#else
    Q_UNUSED(newName);
    return false;
#endif // QT_NO_LIBRARY
#else
    QString linkName = newName;
    if (!linkName.endsWith(QLatin1String(".lnk")))
        linkName += QLatin1String(".lnk");
    QString orgName = fileName(AbsoluteName).replace(QLatin1Char('/'), QLatin1Char('\\'));
    // Need to append on our own
    orgName.prepend(QLatin1Char('"'));
    orgName.append(QLatin1Char('"'));
    bool ret = SUCCEEDED(SHCreateShortcut((wchar_t*)linkName.utf16(), (wchar_t*)orgName.utf16()));
    if (!ret)
        setError(QFile::RenameError, qt_error_string());
    return ret;
#endif // Q_OS_WINCE
}

/*!
    \internal
*/
QAbstractFileEngine::FileFlags QFSFileEnginePrivate::getPermissions() const
{
    QAbstractFileEngine::FileFlags ret = 0;

#if !defined(QT_NO_LIBRARY)
    if((qt_ntfs_permission_lookup > 0) && ((QSysInfo::WindowsVersion&QSysInfo::WV_NT_based) > QSysInfo::WV_NT)) {
        PSID pOwner = 0;
        PSID pGroup = 0;
        PACL pDacl;
        PSECURITY_DESCRIPTOR pSD;
        ACCESS_MASK access_mask;

        enum { ReadMask = 0x00000001, WriteMask = 0x00000002, ExecMask = 0x00000020 };
        resolveLibs();
        if(ptrGetNamedSecurityInfoW && ptrBuildTrusteeWithSidW && ptrGetEffectiveRightsFromAclW) {

            QString fname = filePath.endsWith(QLatin1String(".lnk")) ? readLink(filePath) : filePath;
            DWORD res = ptrGetNamedSecurityInfoW((wchar_t*)fname.utf16(), SE_FILE_OBJECT,
                                                 OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
                                                 &pOwner, &pGroup, &pDacl, 0, &pSD);

            if(res == ERROR_SUCCESS) {
                TRUSTEE_W trustee;
                { //user
                    if(ptrGetEffectiveRightsFromAclW(pDacl, &currentUserTrusteeW, &access_mask) != ERROR_SUCCESS)
                        access_mask = (ACCESS_MASK)-1;
                    if(access_mask & ReadMask)
                        ret |= QAbstractFileEngine::ReadUserPerm;
                    if(access_mask & WriteMask)
                        ret |= QAbstractFileEngine::WriteUserPerm;
                    if(access_mask & ExecMask)
                        ret |= QAbstractFileEngine::ExeUserPerm;
                }
                { //owner
                    ptrBuildTrusteeWithSidW(&trustee, pOwner);
                    if(ptrGetEffectiveRightsFromAclW(pDacl, &trustee, &access_mask) != ERROR_SUCCESS)
                        access_mask = (ACCESS_MASK)-1;
                    if(access_mask & ReadMask)
                        ret |= QAbstractFileEngine::ReadOwnerPerm;
                    if(access_mask & WriteMask)
                        ret |= QAbstractFileEngine::WriteOwnerPerm;
                    if(access_mask & ExecMask)
                        ret |= QAbstractFileEngine::ExeOwnerPerm;
                }
                { //group
                    ptrBuildTrusteeWithSidW(&trustee, pGroup);
                    if(ptrGetEffectiveRightsFromAclW(pDacl, &trustee, &access_mask) != ERROR_SUCCESS)
                        access_mask = (ACCESS_MASK)-1;
                    if(access_mask & ReadMask)
                        ret |= QAbstractFileEngine::ReadGroupPerm;
                    if(access_mask & WriteMask)
                        ret |= QAbstractFileEngine::WriteGroupPerm;
                    if(access_mask & ExecMask)
                        ret |= QAbstractFileEngine::ExeGroupPerm;
                }
                { //other (world)
                    if(ptrGetEffectiveRightsFromAclW(pDacl, &worldTrusteeW, &access_mask) != ERROR_SUCCESS)
                        access_mask = (ACCESS_MASK)-1; // ###
                    if(access_mask & ReadMask)
                        ret |= QAbstractFileEngine::ReadOtherPerm;
                    if(access_mask & WriteMask)
                        ret |= QAbstractFileEngine::WriteOtherPerm;
                    if(access_mask & ExecMask)
                        ret |= QAbstractFileEngine::ExeOtherPerm;
                }
                LocalFree(pSD);
            }
        }
    } else
#endif
           {
        //### what to do with permissions if we don't use NTFS
        // for now just add all permissions and what about exe missions ??
        // also qt_ntfs_permission_lookup is now not set by defualt ... should it ?
        ret |= QAbstractFileEngine::ReadOtherPerm | QAbstractFileEngine::ReadGroupPerm
            | QAbstractFileEngine::ReadOwnerPerm | QAbstractFileEngine::ReadUserPerm
            | QAbstractFileEngine::WriteUserPerm | QAbstractFileEngine::WriteOwnerPerm
            | QAbstractFileEngine::WriteGroupPerm | QAbstractFileEngine::WriteOtherPerm;

        if (doStat()) {
            if (ret & (QAbstractFileEngine::WriteOwnerPerm | QAbstractFileEngine::WriteUserPerm |
                QAbstractFileEngine::WriteGroupPerm | QAbstractFileEngine::WriteOtherPerm)) {
                if (fileAttrib & FILE_ATTRIBUTE_READONLY)
                    ret &= ~(QAbstractFileEngine::WriteOwnerPerm | QAbstractFileEngine::WriteUserPerm |
                             QAbstractFileEngine::WriteGroupPerm | QAbstractFileEngine::WriteOtherPerm);
            }

            QString fname = filePath.endsWith(QLatin1String(".lnk")) ? readLink(filePath) : filePath;
            QString ext = fname.right(4).toLower();
            if (ext == QLatin1String(".exe") || ext == QLatin1String(".com") || ext == QLatin1String(".bat") ||
                ext == QLatin1String(".pif") || ext == QLatin1String(".cmd") || (fileAttrib & FILE_ATTRIBUTE_DIRECTORY))
                ret |= QAbstractFileEngine::ExeOwnerPerm | QAbstractFileEngine::ExeGroupPerm |
                       QAbstractFileEngine::ExeOtherPerm | QAbstractFileEngine::ExeUserPerm;
        }
    }
    return ret;
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::isSymlink() const
{
#if !defined(Q_OS_WINCE)
    if (need_lstat) {
        need_lstat = false;
        is_link = false;

        if (fileAttrib & FILE_ATTRIBUTE_REPARSE_POINT) {
            QString path = QDir::toNativeSeparators(filePath);
            // path for the FindFirstFile should not end with a trailing slash
            while (path.endsWith(QLatin1Char('\\')))
                path.chop(1);

            WIN32_FIND_DATA findData;
            HANDLE hFind = ::FindFirstFile((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16(),
                                           &findData);
            if (hFind != INVALID_HANDLE_VALUE) {
                ::FindClose(hFind);
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
                    && (findData.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT
                        || findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK)) {
                    is_link = true;
                }
            }
        }
    }
    return is_link;
#else
    return false;
#endif // Q_OS_WINCE
}

/*!
    \reimp
*/
QAbstractFileEngine::FileFlags QFSFileEngine::fileFlags(QAbstractFileEngine::FileFlags type) const
{
    Q_D(const QFSFileEngine);
    QAbstractFileEngine::FileFlags ret = 0;
    // Force a stat, so that we're guaranteed to get up-to-date results
    if (type & Refresh) {
        d->tried_stat = 0;
#if !defined(Q_OS_WINCE)
        d->need_lstat = 1;
#endif
    }

    if (type & PermsMask) {
        ret |= d->getPermissions();
        // ### Workaround pascals ### above. Since we always set all properties to true
        // we need to disable read and exec access if the file does not exists
        if (d->doStat())
            ret |= ExistsFlag;
        else
            ret &= 0x2222;
    }
    if (type & TypesMask) {
        if (d->filePath.endsWith(QLatin1String(".lnk"))) {
            ret |= LinkType;
            QString l = readLink(d->filePath);
            if (!l.isEmpty()) {
                bool existed = false;
                if (isDirPath(l, &existed) && existed)
                    ret |= DirectoryType;
                else if (existed)
                    ret |= FileType;
            }
        } else if (d->doStat()) {
            if ((type & LinkType) && d->isSymlink())
                ret |= LinkType;
            if (d->fileAttrib & FILE_ATTRIBUTE_DIRECTORY) {
                ret |= DirectoryType;
            } else {
                ret |= FileType;
            }
        }
    }
    if (type & FlagsMask) {
        ret |= LocalDiskFlag;
        if (d->doStat()) {
            ret |= ExistsFlag;
            if (d->filePath == QLatin1String("/") || isDriveRoot(d->filePath) || isUncRoot(d->filePath)) {
                ret |= RootFlag;
            } else if (d->fileAttrib & FILE_ATTRIBUTE_HIDDEN) {
                QString baseName = fileName(BaseName);
                if (baseName != QLatin1String(".") && baseName != QLatin1String(".."))
                    ret |= HiddenFlag;
            }
        }
    }
    return ret;
}

QString QFSFileEngine::fileName(FileName file) const
{
    Q_D(const QFSFileEngine);
    if (file == BaseName) {
        int slash = d->filePath.lastIndexOf(QLatin1Char('/'));
        if (slash == -1) {
            int colon = d->filePath.lastIndexOf(QLatin1Char(':'));
            if (colon != -1)
                return d->filePath.mid(colon + 1);
            return d->filePath;
        }
        return d->filePath.mid(slash + 1);
    } else if (file == PathName) {
        if (!d->filePath.size())
            return d->filePath;

        int slash = d->filePath.lastIndexOf(QLatin1Char('/'));
        if (slash == -1) {
            if (d->filePath.length() >= 2 && d->filePath.at(1) == QLatin1Char(':'))
                return d->filePath.left(2);
            return QString(QLatin1Char('.'));
        } else {
            if (!slash)
                return QString(QLatin1Char('/'));
            if (slash == 2 && d->filePath.length() >= 2 && d->filePath.at(1) == QLatin1Char(':'))
                slash++;
            return d->filePath.left(slash);
        }
    } else if (file == AbsoluteName || file == AbsolutePathName) {
        QString ret;

        if (!isRelativePath()) {
#if !defined(Q_OS_WINCE)
            if ((d->filePath.size() > 2 && d->filePath.at(1) == QLatin1Char(':')
                && d->filePath.at(2) != QLatin1Char('/')) || // It's a drive-relative path, so Z:a.txt -> Z:\currentpath\a.txt
                d->filePath.startsWith(QLatin1Char('/'))    // It's a absolute path to the current drive, so \a.txt -> Z:\a.txt
                ) {
                ret = QDir::fromNativeSeparators(nativeAbsoluteFilePath(d->filePath));
            } else {
                ret = d->filePath;
            }
#else
            ret = d->filePath;
#endif
        } else {
            ret = QDir::cleanPath(QDir::currentPath() + QLatin1Char('/') + d->filePath);
        }

        // The path should be absolute at this point.
        // From the docs :
        // Absolute paths begin with the directory separator "/"
        // (optionally preceded by a drive specification under Windows).
        if (ret.at(0) != QLatin1Char('/')) {
            Q_ASSERT(ret.length() >= 2);
            Q_ASSERT(ret.at(0).isLetter());
            Q_ASSERT(ret.at(1) == QLatin1Char(':'));

            // Force uppercase drive letters.
            ret[0] = ret.at(0).toUpper();
        }

        if (file == AbsolutePathName) {
            int slash = ret.lastIndexOf(QLatin1Char('/'));
            if (slash < 0)
                return ret;
            else if (ret.at(0) != QLatin1Char('/') && slash == 2)
                return ret.left(3);      // include the slash
            else
                return ret.left(slash > 0 ? slash : 1);
        }
        return ret;
    } else if (file == CanonicalName || file == CanonicalPathName) {
        if (!(fileFlags(ExistsFlag) & ExistsFlag))
            return QString();

        QString ret = QFSFileEnginePrivate::canonicalized(fileName(AbsoluteName));
        if (!ret.isEmpty() && file == CanonicalPathName) {
            int slash = ret.lastIndexOf(QLatin1Char('/'));
            if (slash == -1)
                ret = QDir::currentPath();
            else if (slash == 0)
                ret = QString(QLatin1Char('/'));
            ret = ret.left(slash);
        }
        return ret;
    } else if (file == LinkName) {
        QString ret;
        if (d->filePath.endsWith(QLatin1String(".lnk")))
            ret = readLink(d->filePath);
        else if (d->doStat() && d->isSymlink())
            ret = readSymLink(d->filePath);
        return QDir::fromNativeSeparators(ret);
    } else if (file == BundleName) {
        return QString();
    }
    return d->filePath;
}

bool QFSFileEngine::isRelativePath() const
{
    Q_D(const QFSFileEngine);
    // drive, e.g. "a:", or UNC root, e.q. "//"
    return !(d->filePath.startsWith(QLatin1Char('/'))
        || (d->filePath.length() >= 2
        && ((d->filePath.at(0).isLetter() && d->filePath.at(1) == QLatin1Char(':'))
        || (d->filePath.at(0) == QLatin1Char('/') && d->filePath.at(1) == QLatin1Char('/')))));
}

uint QFSFileEngine::ownerId(FileOwner /*own*/) const
{
    static const uint nobodyID = (uint) -2;
    return nobodyID;
}

QString QFSFileEngine::owner(FileOwner own) const
{
#if !defined(QT_NO_LIBRARY)
    Q_D(const QFSFileEngine);
    if((qt_ntfs_permission_lookup > 0) && ((QSysInfo::WindowsVersion&QSysInfo::WV_NT_based) > QSysInfo::WV_NT)) {
	PSID pOwner = 0;
	PSECURITY_DESCRIPTOR pSD;
	QString name;
	QFSFileEnginePrivate::resolveLibs();

	if(ptrGetNamedSecurityInfoW && ptrLookupAccountSidW) {
	    if(ptrGetNamedSecurityInfoW((wchar_t*)d->filePath.utf16(), SE_FILE_OBJECT,
					 own == OwnerGroup ? GROUP_SECURITY_INFORMATION : OWNER_SECURITY_INFORMATION,
					 NULL, &pOwner, NULL, NULL, &pSD) == ERROR_SUCCESS) {
		DWORD lowner = 0, ldomain = 0;
		SID_NAME_USE use;
		// First call, to determine size of the strings (with '\0').
		ptrLookupAccountSidW(NULL, pOwner, NULL, &lowner, NULL, &ldomain, (SID_NAME_USE*)&use);
		wchar_t *owner = new wchar_t[lowner];
		wchar_t *domain = new wchar_t[ldomain];
		// Second call, size is without '\0'
		if(ptrLookupAccountSidW(NULL, pOwner, (LPWSTR)owner, &lowner,
					 (LPWSTR)domain, &ldomain, (SID_NAME_USE*)&use)) {
		    name = QString::fromUtf16((ushort*)owner);
		}
		LocalFree(pSD);
		delete [] owner;
		delete [] domain;
	    }
	}
	return name;
    }
#else
    Q_UNUSED(own);
#endif
    return QString();
}

bool QFSFileEngine::setPermissions(uint perms)
{
    Q_D(QFSFileEngine);
    bool ret = false;
    int mode = 0;

    if (perms & QFile::ReadOwner || perms & QFile::ReadUser || perms & QFile::ReadGroup || perms & QFile::ReadOther)
        mode |= _S_IREAD;
    if (perms & QFile::WriteOwner || perms & QFile::WriteUser || perms & QFile::WriteGroup || perms & QFile::WriteOther)
        mode |= _S_IWRITE;

    if (mode == 0) // not supported
        return false;

    ret = ::_wchmod((wchar_t*)QFSFileEnginePrivate::longFileName(d->filePath).utf16(), mode) == 0;
    if (!ret)
        setError(QFile::PermissionsError, qt_error_string(errno));
    return ret;
}

bool QFSFileEngine::setSize(qint64 size)
{
    Q_D(QFSFileEngine);

    if (d->fileHandle != INVALID_HANDLE_VALUE || d->fd != -1) {
        // resize open file
        HANDLE fh = d->fileHandle;
#if !defined(Q_OS_WINCE)
        if (fh == INVALID_HANDLE_VALUE)
            fh = (HANDLE)_get_osfhandle(d->fd);
#endif
        if (fh == INVALID_HANDLE_VALUE)
            return false;
        qint64 currentPos = pos();

        if (seek(size) && SetEndOfFile(fh)) {
            seek(qMin(currentPos, size));
            return true;
        }

        seek(currentPos);
        return false;
    }

    if (!d->nativeFilePath.isEmpty()) {
        // resize file on disk
        QFile file(d->filePath);
        if (file.open(QFile::ReadWrite)) {
            bool ret = file.resize(size);
            if (!ret)
                setError(QFile::ResizeError, file.errorString());
            return ret;
        }
    }
    return false;
}


static inline QDateTime fileTimeToQDateTime(const FILETIME *time)
{
    QDateTime ret;

#if defined(Q_OS_WINCE)
    SYSTEMTIME systime;
    FILETIME ftime;
    systime.wYear = 1970;
    systime.wMonth = 1;
    systime.wDay = 1;
    systime.wHour = 0;
    systime.wMinute = 0;
    systime.wSecond = 0;
    systime.wMilliseconds = 0;
    systime.wDayOfWeek = 4;
    SystemTimeToFileTime(&systime, &ftime);
    unsigned __int64 acttime = (unsigned __int64)time->dwHighDateTime << 32 | time->dwLowDateTime;
    FileTimeToSystemTime(time, &systime);
    unsigned __int64 time1970 = (unsigned __int64)ftime.dwHighDateTime << 32 | ftime.dwLowDateTime;
    unsigned __int64 difftime = acttime - time1970;
    difftime /= 10000000;
    ret.setTime_t((unsigned int)difftime);
#else
    SYSTEMTIME sTime, lTime;
    FileTimeToSystemTime(time, &sTime);
    SystemTimeToTzSpecificLocalTime(0, &sTime, &lTime);
    ret.setDate(QDate(lTime.wYear, lTime.wMonth, lTime.wDay));
    ret.setTime(QTime(lTime.wHour, lTime.wMinute, lTime.wSecond, lTime.wMilliseconds));
#endif

    return ret;
}

QDateTime QFSFileEngine::fileTime(FileTime time) const
{
    Q_D(const QFSFileEngine);
    QDateTime ret;
    if (d->fd != -1) {
#if !defined(Q_OS_WINCE)
        HANDLE fh = (HANDLE)_get_osfhandle(d->fd);
        if (fh != INVALID_HANDLE_VALUE) {
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if (GetFileTime(fh, &creationTime, &lastAccessTime, &lastWriteTime)) {
                if(time == CreationTime)
                    ret = fileTimeToQDateTime(&creationTime);
                else if(time == ModificationTime)
                    ret = fileTimeToQDateTime(&lastWriteTime);
                else if(time == AccessTime)
                    ret = fileTimeToQDateTime(&lastAccessTime);
            }
        }
#endif
    } else {
        WIN32_FILE_ATTRIBUTE_DATA attribData;
        bool ok = ::GetFileAttributesEx((wchar_t*)QFSFileEnginePrivate::longFileName(d->filePath).utf16(), GetFileExInfoStandard, &attribData);
        if (!ok) {
            int errorCode = GetLastError();
            if (errorCode == ERROR_ACCESS_DENIED || errorCode == ERROR_SHARING_VIOLATION) {
                QString path = QDir::toNativeSeparators(d->filePath);
                // path for the FindFirstFile should not end with a trailing slash
                while (path.endsWith(QLatin1Char('\\')))
                    path.chop(1);

                // FindFirstFile can not handle drives
                if (!path.endsWith(QLatin1Char(':'))) {
                    WIN32_FIND_DATA findData;
                    HANDLE hFind = ::FindFirstFile((wchar_t*)QFSFileEnginePrivate::longFileName(path).utf16(),
                                                   &findData);
                    if (hFind != INVALID_HANDLE_VALUE) {
                        ::FindClose(hFind);
                        ok = true;
                        attribData.ftCreationTime = findData.ftCreationTime;
                        attribData.ftLastWriteTime = findData.ftLastWriteTime;
                        attribData.ftLastAccessTime = findData.ftLastAccessTime;
                    }
                }
            }
        }
        if (ok) {
            if(time == CreationTime)
                ret = fileTimeToQDateTime(&attribData.ftCreationTime);
            else if(time == ModificationTime)
                ret = fileTimeToQDateTime(&attribData.ftLastWriteTime);
            else if(time == AccessTime)
                ret = fileTimeToQDateTime(&attribData.ftLastAccessTime);
        }
    }
    return ret;
}

uchar *QFSFileEnginePrivate::map(qint64 offset, qint64 size,
                                 QFile::MemoryMapFlags flags)
{
    Q_Q(QFSFileEngine);
    Q_UNUSED(flags);
    if (openMode == QFile::NotOpen) {
        q->setError(QFile::PermissionsError, qt_error_string(ERROR_ACCESS_DENIED));
        return 0;
    }
    if (offset == 0 && size == 0) {
        q->setError(QFile::UnspecifiedError, qt_error_string(ERROR_INVALID_PARAMETER));
        return 0;
    }

    if (mapHandle == INVALID_HANDLE_VALUE) {
        // get handle to the file
        HANDLE handle = fileHandle;

#ifndef Q_OS_WINCE
        if (handle == INVALID_HANDLE_VALUE && fh)
            handle = (HANDLE)::_get_osfhandle(QT_FILENO(fh));
#endif

#ifdef Q_USE_DEPRECATED_MAP_API
        nativeClose();
        // handle automatically closed by kernel with mapHandle (below).
        handle = ::CreateFileForMapping((const wchar_t*)nativeFilePath.constData(),
                GENERIC_READ | (openMode & QIODevice::WriteOnly ? GENERIC_WRITE : 0),
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
#endif

        if (handle == INVALID_HANDLE_VALUE) {
            q->setError(QFile::PermissionsError, qt_error_string(ERROR_ACCESS_DENIED));
            return 0;
        }

        // first create the file mapping handle
        DWORD protection = (openMode & QIODevice::WriteOnly) ? PAGE_READWRITE : PAGE_READONLY;
        mapHandle = ::CreateFileMapping(handle, 0, protection, 0, 0, 0);
        if (mapHandle == INVALID_HANDLE_VALUE) {
            q->setError(QFile::PermissionsError, qt_error_string());
#ifdef Q_USE_DEPRECATED_MAP_API
            ::CloseHandle(handle);
#endif
            return 0;
        }
    }

    // setup args to map
    DWORD access = 0;
    if (openMode & QIODevice::ReadOnly) access = FILE_MAP_READ;
    if (openMode & QIODevice::WriteOnly) access = FILE_MAP_WRITE;

    DWORD offsetHi = offset >> 32;
    DWORD offsetLo = offset & Q_UINT64_C(0xffffffff);
    SYSTEM_INFO sysinfo;
    ::GetSystemInfo(&sysinfo);
    DWORD mask = sysinfo.dwAllocationGranularity - 1;
    DWORD extra = offset & mask;
    if (extra)
        offsetLo &= ~mask;

    // attempt to create the map
    LPVOID mapAddress = ::MapViewOfFile(mapHandle, access,
                                      offsetHi, offsetLo, size + extra);
    if (mapAddress) {
        uchar *address = extra + static_cast<uchar*>(mapAddress);
        maps[address] = extra;
        return address;
    }

    switch(GetLastError()) {
    case ERROR_ACCESS_DENIED:
        q->setError(QFile::PermissionsError, qt_error_string());
        break;
    case ERROR_INVALID_PARAMETER:
        // size are out of bounds
    default:
        q->setError(QFile::UnspecifiedError, qt_error_string());
    }

    ::CloseHandle(mapHandle);
    return 0;
}

bool QFSFileEnginePrivate::unmap(uchar *ptr)
{
    Q_Q(QFSFileEngine);
    if (!maps.contains(ptr)) {
        q->setError(QFile::PermissionsError, qt_error_string(ERROR_ACCESS_DENIED));
        return false;
    }
    uchar *start = ptr - maps[ptr];
    if (!UnmapViewOfFile(start)) {
        q->setError(QFile::PermissionsError, qt_error_string());
        return false;
    }

    maps.remove(ptr);
    if (maps.isEmpty()) {
        ::CloseHandle(mapHandle);
        mapHandle = INVALID_HANDLE_VALUE;
    }

    return true;
}

QT_END_NAMESPACE
