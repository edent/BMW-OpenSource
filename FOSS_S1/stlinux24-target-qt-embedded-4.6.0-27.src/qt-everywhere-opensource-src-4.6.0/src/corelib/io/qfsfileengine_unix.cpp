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

#include "qplatformdefs.h"
#include "qabstractfileengine.h"
#include "private/qfsfileengine_p.h"
#include "private/qcore_unix_p.h"

#ifndef QT_NO_FSFILEENGINE

#include "qfile.h"
#include "qdir.h"
#include "qdatetime.h"
#include "qvarlengtharray.h"

#include <sys/mman.h>
#include <stdlib.h>
#include <limits.h>
#if defined(Q_OS_SYMBIAN)
# include <syslimits.h>
# include <f32file.h>
# include <pathinfo.h>
# include "private/qcore_symbian_p.h"
#endif
#include <errno.h>
#if !defined(QWS) && defined(Q_OS_MAC)
# include <private/qcore_mac_p.h>
#endif

QT_BEGIN_NAMESPACE


#if defined(Q_OS_SYMBIAN)
/*!
    \internal

    Returns true if supplied path is a relative path
*/
static bool isRelativePathSymbian(const QString& fileName)
{
    return !(fileName.startsWith(QLatin1Char('/'))
             || (fileName.length() >= 2
             && ((fileName.at(0).isLetter() && fileName.at(1) == QLatin1Char(':'))
             || (fileName.at(0) == QLatin1Char('/') && fileName.at(1) == QLatin1Char('/')))));
}
#endif

/*!
    \internal

    Returns the stdlib open string corresponding to a QIODevice::OpenMode.
*/
static inline QByteArray openModeToFopenMode(QIODevice::OpenMode flags, const QString &fileName)
{
    QByteArray mode;
    if ((flags & QIODevice::ReadOnly) && !(flags & QIODevice::Truncate)) {
        mode = "rb";
        if (flags & QIODevice::WriteOnly) {
            QT_STATBUF statBuf;
            if (!fileName.isEmpty()
                && QT_STAT(QFile::encodeName(fileName), &statBuf) == 0
                && (statBuf.st_mode & S_IFMT) == S_IFREG) {
                mode += "+";
            } else {
                mode = "wb+";
            }
        }
    } else if (flags & QIODevice::WriteOnly) {
        mode = "wb";
        if (flags & QIODevice::ReadOnly)
            mode += '+';
    }
    if (flags & QIODevice::Append) {
        mode = "ab";
        if (flags & QIODevice::ReadOnly)
            mode += '+';
    }

#if defined(__GLIBC__) && (__GLIBC__ * 0x100 + __GLIBC_MINOR__) >= 0x0207
    // must be glibc >= 2.7
    mode += 'e';
#endif

    return mode;
}

/*!
    \internal

    Returns the stdio open flags corresponding to a QIODevice::OpenMode.
*/
static inline int openModeToOpenFlags(QIODevice::OpenMode mode)
{
    int oflags = QT_OPEN_RDONLY;
#ifdef QT_LARGEFILE_SUPPORT
    oflags |= QT_OPEN_LARGEFILE;
#endif

    if ((mode & QFile::ReadWrite) == QFile::ReadWrite) {
        oflags = QT_OPEN_RDWR | QT_OPEN_CREAT;
    } else if (mode & QFile::WriteOnly) {
        oflags = QT_OPEN_WRONLY | QT_OPEN_CREAT;
    }

    if (mode & QFile::Append) {
        oflags |= QT_OPEN_APPEND;
    } else if (mode & QFile::WriteOnly) {
        if ((mode & QFile::Truncate) || !(mode & QFile::ReadOnly))
            oflags |= QT_OPEN_TRUNC;
    }

    return oflags;
}

/*!
    \internal

    Sets the file descriptor to close on exec. That is, the file
    descriptor is not inherited by child processes.
*/
static inline bool setCloseOnExec(int fd)
{
    return fd != -1 && fcntl(fd, F_SETFD, FD_CLOEXEC) != -1;
}

/*!
    \internal
*/
void QFSFileEnginePrivate::nativeInitFileName()
{
    nativeFilePath = QFile::encodeName(filePath);
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::nativeOpen(QIODevice::OpenMode openMode)
{
    Q_Q(QFSFileEngine);

    if (openMode & QIODevice::Unbuffered) {
        int flags = openModeToOpenFlags(openMode);

        // Try to open the file in unbuffered mode.
        do {
            fd = QT_OPEN(nativeFilePath.constData(), flags, 0666);
        } while (fd == -1 && errno == EINTR);

        // On failure, return and report the error.
        if (fd == -1) {
            q->setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError,
                        qt_error_string(errno));
            return false;
        }

        QT_STATBUF statBuf;
        if (QT_FSTAT(fd, &statBuf) != -1) {
            if ((statBuf.st_mode & S_IFMT) == S_IFDIR) {
                q->setError(QFile::OpenError, QLatin1String("file to open is a directory"));
                QT_CLOSE(fd);
                return false;
            }
        }

        // Seek to the end when in Append mode.
        if (flags & QFile::Append) {
            int ret;
            do {
                ret = QT_LSEEK(fd, 0, SEEK_END);
            } while (ret == -1 && errno == EINTR);

            if (ret == -1) {
                q->setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError,
                            qt_error_string(int(errno)));
                return false;
            }
        }

        fh = 0;
    } else {
        QByteArray fopenMode = openModeToFopenMode(openMode, filePath);

        // Try to open the file in buffered mode.
        do {
            fh = QT_FOPEN(nativeFilePath.constData(), fopenMode.constData());
        } while (!fh && errno == EINTR);

        // On failure, return and report the error.
        if (!fh) {
            q->setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError,
                        qt_error_string(int(errno)));
            return false;
        }

        QT_STATBUF statBuf;
        if (QT_FSTAT(fileno(fh), &statBuf) != -1) {
            if ((statBuf.st_mode & S_IFMT) == S_IFDIR) {
                q->setError(QFile::OpenError, QLatin1String("file to open is a directory"));
                fclose(fh);
                return false;
            }
        }

        setCloseOnExec(fileno(fh)); // ignore failure

        // Seek to the end when in Append mode.
        if (openMode & QIODevice::Append) {
            int ret;
            do {
                ret = QT_FSEEK(fh, 0, SEEK_END);
            } while (ret == -1 && errno == EINTR);

            if (ret == -1) {
                q->setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError,
                            qt_error_string(int(errno)));
                return false;
            }
        }

        fd = -1;
    }

    closeFileHandle = true;
    return true;
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::nativeClose()
{
    return closeFdFh();
}

/*!
    \internal

*/
bool QFSFileEnginePrivate::nativeFlush()
{
    return fh ? flushFh() : fd != -1;
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::nativeRead(char *data, qint64 len)
{
    Q_Q(QFSFileEngine);

    if (fh && nativeIsSequential()) {
        size_t readBytes = 0;
        int oldFlags = fcntl(QT_FILENO(fh), F_GETFL);
        for (int i = 0; i < 2; ++i) {
            // Unix: Make the underlying file descriptor non-blocking
            if ((oldFlags & O_NONBLOCK) == 0)
                fcntl(QT_FILENO(fh), F_SETFL, oldFlags | O_NONBLOCK);

            // Cross platform stdlib read
            size_t read = 0;
            do {
                read = fread(data + readBytes, 1, size_t(len - readBytes), fh);
            } while (read == 0 && !feof(fh) && errno == EINTR);
            if (read > 0) {
                readBytes += read;
                break;
            } else {
                if (readBytes)
                    break;
                readBytes = read;
            }

            // Unix: Restore the blocking state of the underlying socket
            if ((oldFlags & O_NONBLOCK) == 0) {
                fcntl(QT_FILENO(fh), F_SETFL, oldFlags);
                if (readBytes == 0) {
                    int readByte = 0;
                    do {
                        readByte = fgetc(fh);
                    } while (readByte == -1 && errno == EINTR);
                    if (readByte != -1) {
                        *data = uchar(readByte);
                        readBytes += 1;
                    } else {
                        break;
                    }
                }
            }
        }
        // Unix: Restore the blocking state of the underlying socket
        if ((oldFlags & O_NONBLOCK) == 0) {
            fcntl(QT_FILENO(fh), F_SETFL, oldFlags);
        }
        if (readBytes == 0 && !feof(fh)) {
            // if we didn't read anything and we're not at EOF, it must be an error
            q->setError(QFile::ReadError, qt_error_string(int(errno)));
            return -1;
        }
        return readBytes;
    }

    return readFdFh(data, len);
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::nativeReadLine(char *data, qint64 maxlen)
{
    return readLineFdFh(data, maxlen);
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    return writeFdFh(data, len);
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::nativePos() const
{
    return posFdFh();
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::nativeSeek(qint64 pos)
{
    return seekFdFh(pos);
}

/*!
    \internal
*/
int QFSFileEnginePrivate::nativeHandle() const
{
    return fh ? fileno(fh) : fd;
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::nativeIsSequential() const
{
    return isSequentialFdFh();
}

bool QFSFileEngine::remove()
{
    Q_D(QFSFileEngine);
    bool ret = unlink(d->nativeFilePath.constData()) == 0;
    if (!ret)
        setError(QFile::RemoveError, qt_error_string(errno));
    return ret;
}

bool QFSFileEngine::copy(const QString &newName)
{
#if defined(Q_OS_SYMBIAN)
    Q_D(QFSFileEngine);
    RFs rfs = qt_s60GetRFs();
    CFileMan* fm = NULL;
    QString oldNative(QDir::toNativeSeparators(d->filePath));
    TPtrC oldPtr(qt_QString2TPtrC(oldNative));
    QFileInfo fi(newName);
    QString absoluteNewName = fi.absoluteFilePath();
    QString newNative(QDir::toNativeSeparators(absoluteNewName));
    TPtrC newPtr(qt_QString2TPtrC(newNative));
    TRAPD (err,
        fm = CFileMan::NewL(rfs);
        RFile rfile;
        err = rfile.Open(rfs, oldPtr, EFileShareReadersOrWriters);
        if (err == KErrNone) {
            err = fm->Copy(rfile, newPtr);
            rfile.Close();
        }
    ) // End TRAP
    delete fm;
    // ### Add error reporting on failure
    return (err == KErrNone);
#else
    Q_UNUSED(newName);
    // ### Add copy code for Unix here
    setError(QFile::UnspecifiedError, QLatin1String("Not implemented!"));
    return false;
#endif
}

bool QFSFileEngine::rename(const QString &newName)
{
    Q_D(QFSFileEngine);
    bool ret = ::rename(d->nativeFilePath.constData(), QFile::encodeName(newName).constData()) == 0;
    if (!ret)
        setError(QFile::RenameError, qt_error_string(errno));
    return ret;
}

bool QFSFileEngine::link(const QString &newName)
{
    Q_D(QFSFileEngine);
    bool ret = ::symlink(d->nativeFilePath.constData(), QFile::encodeName(newName).constData()) == 0;
    if (!ret)
        setError(QFile::RenameError, qt_error_string(errno));
    return ret;
}

qint64 QFSFileEnginePrivate::nativeSize() const
{
    return sizeFdFh();
}

bool QFSFileEngine::mkdir(const QString &name, bool createParentDirectories) const
{
    QString dirName = name;
    if (createParentDirectories) {
        dirName = QDir::cleanPath(dirName);
#if defined(Q_OS_SYMBIAN)
        dirName = QDir::toNativeSeparators(dirName);
#endif
        for(int oldslash = -1, slash=0; slash != -1; oldslash = slash) {
            slash = dirName.indexOf(QDir::separator(), oldslash+1);
            if (slash == -1) {
                if (oldslash == dirName.length())
                    break;
                slash = dirName.length();
            }
            if (slash) {
                QByteArray chunk = QFile::encodeName(dirName.left(slash));
                QT_STATBUF st;
                if (QT_STAT(chunk, &st) != -1) {
                    if ((st.st_mode & S_IFMT) != S_IFDIR)
                        return false;
                } else if (QT_MKDIR(chunk, 0777) != 0) {
                    return false;
                }
            }
        }
        return true;
    }
#if defined(Q_OS_DARWIN)  // Mac X doesn't support trailing /'s
    if (dirName.endsWith(QLatin1Char('/')))
        dirName.chop(1);
#endif
    return (QT_MKDIR(QFile::encodeName(dirName), 0777) == 0);
}

bool QFSFileEngine::rmdir(const QString &name, bool recurseParentDirectories) const
{
    QString dirName = name;
    if (recurseParentDirectories) {
        dirName = QDir::cleanPath(dirName);
#if defined(Q_OS_SYMBIAN)
        dirName = QDir::toNativeSeparators(dirName);
#endif
        for(int oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash) {
            QByteArray chunk = QFile::encodeName(dirName.left(slash));
            QT_STATBUF st;
            if (QT_STAT(chunk, &st) != -1) {
                if ((st.st_mode & S_IFMT) != S_IFDIR)
                    return false;
                if (::rmdir(chunk) != 0)
                    return oldslash != 0;
            } else {
                return false;
            }
            slash = dirName.lastIndexOf(QDir::separator(), oldslash-1);
        }
        return true;
    }
    return ::rmdir(QFile::encodeName(dirName)) == 0;
}

bool QFSFileEngine::caseSensitive() const
{
#if defined(Q_OS_SYMBIAN)
    return false;
#else
    return true;
#endif
}

bool QFSFileEngine::setCurrentPath(const QString &path)
{
    int r;
    r = QT_CHDIR(QFile::encodeName(path));
    return r >= 0;
}

QString QFSFileEngine::currentPath(const QString &)
{
    QString result;
    QT_STATBUF st;
#if defined(Q_OS_SYMBIAN)
    char nativeCurrentName[PATH_MAX+1];
    if (::getcwd(nativeCurrentName, PATH_MAX))
        result = QDir::fromNativeSeparators(QFile::decodeName(QByteArray(nativeCurrentName)));
    if (result.isEmpty()) {
# if defined(QT_DEBUG)
        qWarning("QFSFileEngine::currentPath: getcwd() failed");
# endif
    } else
#endif
    if (QT_STAT(".", &st) == 0) {
#if defined(__GLIBC__) && !defined(PATH_MAX)
        char *currentName = ::get_current_dir_name();
        if (currentName) {
            result = QFile::decodeName(QByteArray(currentName));
            ::free(currentName);
        }
#elif !defined(Q_OS_SYMBIAN)
        char currentName[PATH_MAX+1];
        if (::getcwd(currentName, PATH_MAX))
            result = QFile::decodeName(QByteArray(currentName));
# if defined(QT_DEBUG)
        if (result.isNull())
            qWarning("QFSFileEngine::currentPath: getcwd() failed");
# endif
#endif
    } else {
#if defined(Q_OS_SYMBIAN)
        // If current dir returned by Open C doesn't exist,
        // try to create it (can happen with application private dirs)
        // Ignore mkdir failures; we want to be consistent with Open C
        // current path regardless.
        QT_MKDIR(QFile::encodeName(QLatin1String(nativeCurrentName)), 0777);
#else
# if defined(QT_DEBUG)
        qWarning("QFSFileEngine::currentPath: stat(\".\") failed");
# endif
#endif
    }
    return result;
}

QString QFSFileEngine::homePath()
{
#if defined(Q_OS_SYMBIAN)
    QString home = rootPath();
#else
    QString home = QFile::decodeName(qgetenv("HOME"));
    if (home.isNull())
        home = rootPath();
#endif
    return home;
}

QString QFSFileEngine::rootPath()
{
#if defined(Q_OS_SYMBIAN)
# ifdef Q_WS_S60
    TFileName symbianPath = PathInfo::PhoneMemoryRootPath();
    return QDir::cleanPath(QDir::fromNativeSeparators(qt_TDesC2QString(symbianPath)));
# else
# warning No fallback implementation of QFSFileEngine::rootPath()
    return QString();
# endif
#else
    return QLatin1String("/");
#endif
}

QString QFSFileEngine::tempPath()
{
#if defined(Q_OS_SYMBIAN)
# ifdef Q_WS_S60
    TFileName symbianPath = PathInfo::PhoneMemoryRootPath();
    QString temp = QDir::fromNativeSeparators(qt_TDesC2QString(symbianPath));
    temp += QLatin1String( "temp/");

    // Just to verify that folder really exist on hardware
    QT_MKDIR(QFile::encodeName(temp), 0777);
# else
# warning No fallback implementation of QFSFileEngine::tempPath()
    QString temp;
# endif
#else
    QString temp = QFile::decodeName(qgetenv("TMPDIR"));
    if (temp.isEmpty())
        temp = QLatin1String("/tmp/");
#endif
    return temp;
}

QFileInfoList QFSFileEngine::drives()
{
    QFileInfoList ret;
#if defined(Q_OS_SYMBIAN)
    TDriveList driveList;
    RFs rfs = qt_s60GetRFs();
    TInt err = rfs.DriveList(driveList);
    if (err == KErrNone) {
        char driveName[] = "A:/";

        for (char i = 0; i < KMaxDrives; i++) {
            if (driveList[i]) {
                driveName[0] = 'A' + i;
                ret.append(QFileInfo(QLatin1String(driveName)));
            }
        }
    } else {
        qWarning("QFSFileEngine::drives: Getting drives failed");
    }
#else
    ret.append(QFileInfo(rootPath()));
#endif
    return ret;
}

bool QFSFileEnginePrivate::doStat() const
{
    if (!tried_stat) {
        tried_stat = true;
        could_stat = false;

        if (fh && nativeFilePath.isEmpty()) {
            // ### actually covers two cases: d->fh and when the file is not open
            could_stat = (QT_FSTAT(QT_FILENO(fh), &st) == 0);
        } else if (fd == -1) {
            // ### actually covers two cases: d->fh and when the file is not open
            could_stat = (QT_STAT(nativeFilePath.constData(), &st) == 0);
        } else {
            could_stat = (QT_FSTAT(fd, &st) == 0);
        }
    }
    return could_stat;
}

bool QFSFileEnginePrivate::isSymlink() const
{
    if (need_lstat) {
        need_lstat = false;

        QT_STATBUF st;          // don't clobber our main one
        is_link = (QT_LSTAT(nativeFilePath.constData(), &st) == 0) ? S_ISLNK(st.st_mode) : false;
    }
    return is_link;
}

#if defined(Q_OS_SYMBIAN)
static bool _q_isSymbianHidden(const QString &path, bool isDir)
{
    RFs rfs = qt_s60GetRFs();
    QFileInfo fi(path);
    QString absPath = fi.absoluteFilePath();
    if (isDir && !absPath.endsWith(QLatin1Char('/')))
        absPath.append(QLatin1Char('/'));
    QString native(QDir::toNativeSeparators(absPath));
    TPtrC ptr(qt_QString2TPtrC(native));
    TUint attributes;
    TInt err = rfs.Att(ptr, attributes);
    return (err == KErrNone && (attributes & KEntryAttHidden));
}
#endif

#if !defined(QWS) && defined(Q_OS_MAC)
static bool _q_isMacHidden(const QString &path)
{
    OSErr err = noErr;

    FSRef fsRef;

    err = FSPathMakeRefWithOptions(reinterpret_cast<const UInt8 *>(QFile::encodeName(QDir::cleanPath(path)).constData()),
                                    kFSPathMakeRefDoNotFollowLeafSymlink, &fsRef, 0);
    if (err != noErr)
        return false;

    FSCatalogInfo catInfo;
    err = FSGetCatalogInfo(&fsRef, kFSCatInfoFinderInfo, &catInfo, NULL, NULL, NULL);
    if (err != noErr)
        return false;

    FileInfo * const fileInfo = reinterpret_cast<FileInfo*>(&catInfo.finderInfo);
    bool result = (fileInfo->finderFlags & kIsInvisible);
    return result;
}
#endif

/*!
    \reimp
*/
QAbstractFileEngine::FileFlags QFSFileEngine::fileFlags(FileFlags type) const
{
    Q_D(const QFSFileEngine);
    // Force a stat, so that we're guaranteed to get up-to-date results
    if (type & Refresh) {
        d->tried_stat = 0;
        d->need_lstat = 1;
    }

    QAbstractFileEngine::FileFlags ret = 0;
    if (type & FlagsMask)
        ret |= LocalDiskFlag;
    bool exists = d->doStat();
    if (!exists && !d->isSymlink())
        return ret;

    if (exists && (type & PermsMask)) {
        if (d->st.st_mode & S_IRUSR)
            ret |= ReadOwnerPerm;
        if (d->st.st_mode & S_IWUSR)
            ret |= WriteOwnerPerm;
        if (d->st.st_mode & S_IXUSR)
            ret |= ExeOwnerPerm;
        if (d->st.st_mode & S_IRUSR)
            ret |= ReadUserPerm;
        if (d->st.st_mode & S_IWUSR)
            ret |= WriteUserPerm;
        if (d->st.st_mode & S_IXUSR)
            ret |= ExeUserPerm;
        if (d->st.st_mode & S_IRGRP)
            ret |= ReadGroupPerm;
        if (d->st.st_mode & S_IWGRP)
            ret |= WriteGroupPerm;
        if (d->st.st_mode & S_IXGRP)
            ret |= ExeGroupPerm;
        if (d->st.st_mode & S_IROTH)
            ret |= ReadOtherPerm;
        if (d->st.st_mode & S_IWOTH)
            ret |= WriteOtherPerm;
        if (d->st.st_mode & S_IXOTH)
            ret |= ExeOtherPerm;
    }
    if (type & TypesMask) {
#if !defined(QWS) && defined(Q_OS_MAC)
        bool foundAlias = false;
        {
            FSRef fref;
            if (FSPathMakeRef((const UInt8 *)QFile::encodeName(QDir::cleanPath(d->filePath)).data(),
                             &fref, NULL) == noErr) {
                Boolean isAlias, isFolder;
                if (FSIsAliasFile(&fref, &isAlias, &isFolder) == noErr && isAlias) {
                    foundAlias = true;
                    ret |= LinkType;
                }
            }
        }
        if (!foundAlias)
#endif
        {
            if ((type & LinkType) && d->isSymlink())
                ret |= LinkType;
            if (exists && (d->st.st_mode & S_IFMT) == S_IFREG)
                ret |= FileType;
            else if (exists && (d->st.st_mode & S_IFMT) == S_IFDIR)
                ret |= DirectoryType;
#if !defined(QWS) && defined(Q_OS_MAC)
            if ((ret & DirectoryType) && (type & BundleType)) {
                QCFType<CFURLRef> url = CFURLCreateWithFileSystemPath(0, QCFString(d->filePath),
                                                                      kCFURLPOSIXPathStyle, true);
                UInt32 type, creator;
                if (CFBundleGetPackageInfoInDirectory(url, &type, &creator))
                    ret |= BundleType;
            }
#endif
        }
    }
    if (type & FlagsMask) {
        if (exists)
            ret |= ExistsFlag;
#if defined(Q_OS_SYMBIAN)
        if (d->filePath == QLatin1String("/")
            || (d->filePath.length() == 3 && d->filePath.at(0).isLetter()
                && d->filePath.at(1) == QLatin1Char(':') && d->filePath.at(2) == QLatin1Char('/'))) {
            ret |= RootFlag;
        } else {
            // In Symbian, all symlinks have hidden attribute for some reason;
            // lets make them visible for better compatibility with other platforms.
            // If somebody actually wants a hidden link, then they are out of luck.
            if (!d->isSymlink() && _q_isSymbianHidden(d->filePath, ret & DirectoryType))
                ret |= HiddenFlag;
        }
#else
        if (d->filePath == QLatin1String("/")) {
            ret |= RootFlag;
        } else {
            QString baseName = fileName(BaseName);
            if ((baseName.size() > 1
                 && baseName.at(0) == QLatin1Char('.') && baseName.at(1) != QLatin1Char('.'))
#  if !defined(QWS) && defined(Q_OS_MAC)
                    || _q_isMacHidden(d->filePath)
#  endif
            ) {
                ret |= HiddenFlag;
            }
        }
#endif
    }
    return ret;
}

#if defined(Q_OS_SYMBIAN)
QString QFSFileEngine::fileName(FileName file) const
{
    Q_D(const QFSFileEngine);
    const QLatin1Char slashChar('/');
    if(file == BaseName) {
        int slash = d->filePath.lastIndexOf(slashChar);
        if(slash == -1) {
            int colon = d->filePath.lastIndexOf(QLatin1Char(':'));
            if(colon != -1)
                return d->filePath.mid(colon + 1);
            return d->filePath;
        }
        return d->filePath.mid(slash + 1);
    } else if(file == PathName) {
        if(!d->filePath.size())
            return d->filePath;

        int slash = d->filePath.lastIndexOf(slashChar);
        if(slash == -1) {
            if(d->filePath.length() >= 2 && d->filePath.at(1) == QLatin1Char(':'))
                return d->filePath.left(2);
            return QLatin1String(".");
        } else {
            if(!slash)
                return QLatin1String("/");
            if(slash == 2 && d->filePath.length() >= 2 && d->filePath.at(1) == QLatin1Char(':'))
                slash++;
            return d->filePath.left(slash);
        }
    } else if(file == AbsoluteName || file == AbsolutePathName) {
        QString ret;
        if (!isRelativePathSymbian(d->filePath)) {
            if (d->filePath.size() > 2 && d->filePath.at(1) == QLatin1Char(':')
                && d->filePath.at(2) != slashChar){
                // It's a drive-relative path, so C:a.txt -> C:/currentpath/a.txt,
                // or if it's different drive than current, Z:a.txt -> Z:/a.txt
                QString currentPath = QDir::currentPath();
                if (0 == currentPath.left(1).compare(d->filePath.left(1), Qt::CaseInsensitive))
                    ret = currentPath + slashChar + d->filePath.mid(2);
                else
                    ret = d->filePath.left(2) + slashChar + d->filePath.mid(2);
            } else if (d->filePath.startsWith(slashChar)) {
                // It's a absolute path to the current drive, so /a.txt -> C:/a.txt
                ret = QDir::currentPath().left(2) + d->filePath;
            } else {
                ret = d->filePath;
            }
        } else {
            ret = QDir::currentPath() + slashChar + d->filePath;
        }

        // The path should be absolute at this point.
        // From the docs :
        // Absolute paths begin with the directory separator "/"
        // (optionally preceded by a drive specification under Windows).
        if (ret.at(0) != slashChar) {
            Q_ASSERT(ret.length() >= 2);
            Q_ASSERT(ret.at(0).isLetter());
            Q_ASSERT(ret.at(1) == QLatin1Char(':'));

            // Force uppercase drive letters.
            ret[0] = ret.at(0).toUpper();
        }

        // Clean up the path
        bool isDir = ret.endsWith(slashChar);
        ret = QDir::cleanPath(ret);
        if (isDir && !ret.endsWith(slashChar))
            ret += slashChar;

        if (file == AbsolutePathName) {
            int slash = ret.lastIndexOf(slashChar);
            if (slash < 0)
                return ret;
            else if (ret.at(0) != slashChar && slash == 2)
                return ret.left(3);      // include the slash
            else
                return ret.left(slash > 0 ? slash : 1);
        }
        return ret;
    } else if(file == CanonicalName || file == CanonicalPathName) {
        if (!(fileFlags(ExistsFlag) & ExistsFlag))
            return QString();

        QString ret = QFSFileEnginePrivate::canonicalized(fileName(AbsoluteName));
        if (file == CanonicalPathName && !ret.isEmpty()) {
            int slash = ret.lastIndexOf(slashChar);
            if (slash == -1)
                ret = QDir::fromNativeSeparators(QDir::currentPath());
            else if (slash == 0)
                ret = QLatin1String("/");
            ret = ret.left(slash);
        }
        return ret;
    } else if(file == LinkName) {
        if (d->isSymlink()) {
            char s[PATH_MAX+1];
            int len = readlink(d->nativeFilePath.constData(), s, PATH_MAX);
            if (len > 0) {
                s[len] = '\0';
                QString ret = QFile::decodeName(QByteArray(s));

                if (isRelativePathSymbian(ret)) {
                    if (!isRelativePathSymbian(d->filePath)) {
                        ret.prepend(d->filePath.left(d->filePath.lastIndexOf(slashChar))
                                    + slashChar);
                    } else {
                        ret.prepend(QDir::currentPath() + slashChar);
                    }
                }
                ret = QDir::cleanPath(ret);
                if (ret.size() > 1 && ret.endsWith(slashChar))
                    ret.chop(1);
                return ret;
            }
        }
        return QString();
    } else if(file == BundleName) {
        return QString();
    }
    return d->filePath;
}

#else

QString QFSFileEngine::fileName(FileName file) const
{
    Q_D(const QFSFileEngine);
    if (file == BundleName) {
#if !defined(QWS) && defined(Q_OS_MAC)
        QCFType<CFURLRef> url = CFURLCreateWithFileSystemPath(0, QCFString(d->filePath),
                                                              kCFURLPOSIXPathStyle, true);
        if (CFDictionaryRef dict = CFBundleCopyInfoDictionaryForURL(url)) {
            if (CFTypeRef name = (CFTypeRef)CFDictionaryGetValue(dict, kCFBundleNameKey)) {
                if (CFGetTypeID(name) == CFStringGetTypeID())
                    return QCFString::toQString((CFStringRef)name);
            }
        }
#endif
        return QString();
    } else if (file == BaseName) {
        int slash = d->filePath.lastIndexOf(QLatin1Char('/'));
        if (slash != -1)
            return d->filePath.mid(slash + 1);
    } else if (file == PathName) {
        int slash = d->filePath.lastIndexOf(QLatin1Char('/'));
        if (slash == -1)
            return QLatin1String(".");
        else if (!slash)
            return QLatin1String("/");
        return d->filePath.left(slash);
    } else if (file == AbsoluteName || file == AbsolutePathName) {
        QString ret;
        if (d->filePath.isEmpty() || !d->filePath.startsWith(QLatin1Char('/')))
            ret = QDir::currentPath();
        if (!d->filePath.isEmpty() && d->filePath != QLatin1String(".")) {
            if (!ret.isEmpty() && !ret.endsWith(QLatin1Char('/')))
                ret += QLatin1Char('/');
            ret += d->filePath;
        }
        if (ret == QLatin1String("/"))
            return ret;
        bool isDir = ret.endsWith(QLatin1Char('/'));
        ret = QDir::cleanPath(ret);
        if (isDir)
            ret += QLatin1Char('/');
        if (file == AbsolutePathName) {
            int slash = ret.lastIndexOf(QLatin1Char('/'));
            if (slash == -1)
                return QDir::currentPath();
            else if (!slash)
                return QLatin1String("/");
            return ret.left(slash);
        }
        return ret;
    } else if (file == CanonicalName || file == CanonicalPathName) {
        if (!(fileFlags(ExistsFlag) & ExistsFlag))
            return QString();

        QString ret = QFSFileEnginePrivate::canonicalized(fileName(AbsoluteName));
        if (file == CanonicalPathName && !ret.isEmpty()) {
            int slash = ret.lastIndexOf(QLatin1Char('/'));
            if (slash == -1)
                ret = QDir::currentPath();
            else if (slash == 0)
                ret = QLatin1String("/");
            ret = ret.left(slash);
        }
        return ret;
    } else if (file == LinkName) {
        if (d->isSymlink()) {
#if defined(__GLIBC__) && !defined(PATH_MAX)
#define PATH_CHUNK_SIZE 256
            char *s = 0;
            int len = -1;
            int size = PATH_CHUNK_SIZE;

            while (1) {
                s = q_check_ptr((char *) ::realloc(s, size));
                len = ::readlink(d->nativeFilePath.constData(), s, size);
                if (len < 0) {
                    ::free(s);
                    break;
                }
                if (len < size) {
                    break;
                }
                size *= 2;
            }
#else
            char s[PATH_MAX+1];
            int len = readlink(d->nativeFilePath.constData(), s, PATH_MAX);
#endif
            if (len > 0) {
                QString ret;
                if (d->doStat() && S_ISDIR(d->st.st_mode) && s[0] != '/') {
                    QDir parent(d->filePath);
                    parent.cdUp();
                    ret = parent.path();
                    if (!ret.isEmpty() && !ret.endsWith(QLatin1Char('/')))
                        ret += QLatin1Char('/');
                }
                s[len] = '\0';
                ret += QFile::decodeName(QByteArray(s));
#if defined(__GLIBC__) && !defined(PATH_MAX)
                ::free(s);
#endif

                if (!ret.startsWith(QLatin1Char('/'))) {
                    if (d->filePath.startsWith(QLatin1Char('/'))) {
                        ret.prepend(d->filePath.left(d->filePath.lastIndexOf(QLatin1Char('/')))
                                    + QLatin1Char('/'));
                    } else {
                        ret.prepend(QDir::currentPath() + QLatin1Char('/'));
                    }
                }
                ret = QDir::cleanPath(ret);
                if (ret.size() > 1 && ret.endsWith(QLatin1Char('/')))
                    ret.chop(1);
                return ret;
            }
        }
#if !defined(QWS) && defined(Q_OS_MAC)
        {
            FSRef fref;
            if (FSPathMakeRef((const UInt8 *)QFile::encodeName(QDir::cleanPath(d->filePath)).data(), &fref, 0) == noErr) {
                Boolean isAlias, isFolder;
                if (FSResolveAliasFile(&fref, true, &isFolder, &isAlias) == noErr && isAlias) {
                    AliasHandle alias;
                    if (FSNewAlias(0, &fref, &alias) == noErr && alias) {
                        CFStringRef cfstr;
                        if (FSCopyAliasInfo(alias, 0, 0, &cfstr, 0, 0) == noErr)
                            return QCFString::toQString(cfstr);
                    }
                }
            }
        }
#endif
        return QString();
    }
    return d->filePath;
}
#endif // Q_OS_SYMBIAN

bool QFSFileEngine::isRelativePath() const
{
    Q_D(const QFSFileEngine);
#if defined(Q_OS_SYMBIAN)
    return isRelativePathSymbian(d->filePath);
#else
    return d->filePath.length() ? d->filePath[0] != QLatin1Char('/') : true;
#endif
}

uint QFSFileEngine::ownerId(FileOwner own) const
{
    Q_D(const QFSFileEngine);
    static const uint nobodyID = (uint) -2;
    if (d->doStat()) {
        if (own == OwnerUser)
            return d->st.st_uid;
        else
            return d->st.st_gid;
    }
    return nobodyID;
}

QString QFSFileEngine::owner(FileOwner own) const
{
#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_OPENBSD)
    int size_max = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (size_max == -1)
        size_max = 1024;
    QVarLengthArray<char, 1024> buf(size_max);
#endif

    if (own == OwnerUser) {
        struct passwd *pw = 0;
#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_OPENBSD)
        struct passwd entry;
        getpwuid_r(ownerId(own), &entry, buf.data(), buf.size(), &pw);
#else
        pw = getpwuid(ownerId(own));
#endif
        if (pw)
            return QFile::decodeName(QByteArray(pw->pw_name));
    } else if (own == OwnerGroup) {
#if !defined(Q_OS_SYMBIAN)
        struct group *gr = 0;
#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_OPENBSD)
        size_max = sysconf(_SC_GETGR_R_SIZE_MAX);
        if (size_max == -1)
            size_max = 1024;
        buf.resize(size_max);
        struct group entry;
        // Some large systems have more members than the POSIX max size
        // Loop over by doubling the buffer size (upper limit 250k)
        for (unsigned size = size_max; size < 256000; size += size)
        {
            buf.resize(size);
            // ERANGE indicates that the buffer was too small
            if (!getgrgid_r(ownerId(own), &entry, buf.data(), buf.size(), &gr)
                || errno != ERANGE)
                break;
        }
#else
        gr = getgrgid(ownerId(own));
#endif
        if (gr)
            return QFile::decodeName(QByteArray(gr->gr_name));
#endif
    }
    return QString();
}

bool QFSFileEngine::setPermissions(uint perms)
{
    Q_D(QFSFileEngine);
    bool ret = false;
    mode_t mode = 0;
    if (perms & ReadOwnerPerm)
        mode |= S_IRUSR;
    if (perms & WriteOwnerPerm)
        mode |= S_IWUSR;
    if (perms & ExeOwnerPerm)
        mode |= S_IXUSR;
    if (perms & ReadUserPerm)
        mode |= S_IRUSR;
    if (perms & WriteUserPerm)
        mode |= S_IWUSR;
    if (perms & ExeUserPerm)
        mode |= S_IXUSR;
    if (perms & ReadGroupPerm)
        mode |= S_IRGRP;
    if (perms & WriteGroupPerm)
        mode |= S_IWGRP;
    if (perms & ExeGroupPerm)
        mode |= S_IXGRP;
    if (perms & ReadOtherPerm)
        mode |= S_IROTH;
    if (perms & WriteOtherPerm)
        mode |= S_IWOTH;
    if (perms & ExeOtherPerm)
        mode |= S_IXOTH;
    if (d->fd != -1)
        ret = fchmod(d->fd, mode) == 0;
    else
        ret = ::chmod(d->nativeFilePath.constData(), mode) == 0;
    if (!ret)
        setError(QFile::PermissionsError, qt_error_string(errno));
    return ret;
}

bool QFSFileEngine::setSize(qint64 size)
{
    Q_D(QFSFileEngine);
    bool ret = false;
    if (d->fd != -1)
        ret = QT_FTRUNCATE(d->fd, size) == 0;
    else if (d->fh)
        ret = QT_FTRUNCATE(QT_FILENO(d->fh), size) == 0;
    else
        ret = QT_TRUNCATE(d->nativeFilePath.constData(), size) == 0;
    if (!ret)
        setError(QFile::ResizeError, qt_error_string(errno));
    return ret;
}

QDateTime QFSFileEngine::fileTime(FileTime time) const
{
    Q_D(const QFSFileEngine);
    QDateTime ret;
    if (d->doStat()) {
        if (time == CreationTime)
            ret.setTime_t(d->st.st_ctime ? d->st.st_ctime : d->st.st_mtime);
        else if (time == ModificationTime)
            ret.setTime_t(d->st.st_mtime);
        else if (time == AccessTime)
            ret.setTime_t(d->st.st_atime);
    }
    return ret;
}

uchar *QFSFileEnginePrivate::map(qint64 offset, qint64 size, QFile::MemoryMapFlags flags)
{
    Q_Q(QFSFileEngine);
    Q_UNUSED(flags);
    if (openMode == QIODevice::NotOpen) {
        q->setError(QFile::PermissionsError, qt_error_string(int(EACCES)));
        return 0;
    }

    if (offset < 0 || offset != qint64(QT_OFF_T(offset))
            || size < 0 || quint64(size) > quint64(size_t(-1))) {
        q->setError(QFile::UnspecifiedError, qt_error_string(int(EINVAL)));
        return 0;
    }

    // If we know the mapping will extend beyond EOF, fail early to avoid
    // undefined behavior. Otherwise, let mmap have its say.
    if (doStat()
            && (QT_OFF_T(size) > st.st_size - QT_OFF_T(offset)))
        qWarning("QFSFileEngine::map: Mapping a file beyond its size is not portable");

    int access = 0;
    if (openMode & QIODevice::ReadOnly) access |= PROT_READ;
    if (openMode & QIODevice::WriteOnly) access |= PROT_WRITE;

    int pageSize = getpagesize();
    int extra = offset % pageSize;

    if (size + extra > (size_t)-1) {
        q->setError(QFile::UnspecifiedError, qt_error_string(int(EINVAL)));
        return 0;
    }

    size_t realSize = (size_t)size + extra;
    QT_OFF_T realOffset = QT_OFF_T(offset);
    realOffset &= ~(QT_OFF_T(pageSize - 1));

#ifdef Q_OS_SYMBIAN
    void *mapAddress;
    TRAPD(err,     mapAddress = QT_MMAP((void*)0, realSize,
                   access, MAP_SHARED, nativeHandle(), realOffset));
    if (err != KErrNone) {
        qWarning("OpenC bug: leave from mmap %d", err);
        mapAddress = MAP_FAILED;
        errno = EINVAL;
    }
#else
    void *mapAddress = QT_MMAP((void*)0, realSize,
                   access, MAP_SHARED, nativeHandle(), realOffset);
#endif
    if (MAP_FAILED != mapAddress) {
        uchar *address = extra + static_cast<uchar*>(mapAddress);
        maps[address] = QPair<int,size_t>(extra, realSize);
        return address;
    }

    switch(errno) {
    case EBADF:
        q->setError(QFile::PermissionsError, qt_error_string(int(EACCES)));
        break;
    case ENFILE:
    case ENOMEM:
        q->setError(QFile::ResourceError, qt_error_string(int(errno)));
        break;
    case EINVAL:
        // size are out of bounds
    default:
        q->setError(QFile::UnspecifiedError, qt_error_string(int(errno)));
        break;
    }
    return 0;
}

bool QFSFileEnginePrivate::unmap(uchar *ptr)
{
    Q_Q(QFSFileEngine);
    if (!maps.contains(ptr)) {
        q->setError(QFile::PermissionsError, qt_error_string(EACCES));
        return false;
    }

    uchar *start = ptr - maps[ptr].first;
    size_t len = maps[ptr].second;
    if (-1 == munmap(start, len)) {
        q->setError(QFile::UnspecifiedError, qt_error_string(errno));
        return false;
    }
    maps.remove(ptr);
    return true;
}

QT_END_NAMESPACE

#endif // QT_NO_FSFILEENGINE
