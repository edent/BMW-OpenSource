/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3filedialog.h"

#ifndef QT_NO_FILEDIALOG

#include "qapplication.h"
#include "private/qapplication_p.h"
#include "qt_windows.h"
#include "qregexp.h"
#include "qbuffer.h"
#include "qdir.h"
#include "qstringlist.h"
#include "qlibrary.h"

#ifndef QT_NO_THREAD
#  include "private/qmutexpool_p.h"
#endif

#include <shlobj.h>

#ifdef Q_OS_WINCE
#include <commdlg.h>
#endif

QT_BEGIN_NAMESPACE

extern const char qt3_file_dialog_filter_reg_exp[]; // defined in qfiledialog.cpp

const int maxNameLen = 1023;
const int maxMultiLen = 65535;

// Returns the wildcard part of a filter.
static QString extractFilter(const QString& rawFilter)
{
    QString result = rawFilter;
    QRegExp r(QString::fromLatin1(qt3_file_dialog_filter_reg_exp));
    int index = r.indexIn(result);
    if (index >= 0)
        result = r.cap(2);
    return result.replace(QLatin1Char(' '), QLatin1Char(';'));
}

// Makes a list of filters from ;;-separated text.
static QStringList makeFiltersList(const QString &filter)
{
    QString f(filter);

    if (f.isEmpty())
        f = Q3FileDialog::tr("All Files (*.*)");

    if (f.isEmpty())
        return QStringList();

    int i = f.find(QLatin1String(";;"), 0);
    QString sep(QLatin1String(";;"));
    if (i == -1) {
        if (f.find(QLatin1String("\n"), 0) != -1) {
            sep = QLatin1String("\n");
            i = f.find(sep, 0);
        }
    }

    return QStringList::split(sep, f );
}

// Makes a NUL-oriented Windows filter from a Qt filter.
static QString winFilter(const QString& filter)
{
    QStringList filterLst = makeFiltersList(filter);
    QStringList::Iterator it = filterLst.begin();
    QString winfilters;
    for (; it != filterLst.end(); ++it) {
        winfilters += *it;
        winfilters += QChar::null;
        winfilters += extractFilter(*it);
        winfilters += QChar::null;
    }
    winfilters += QChar::null;
    return winfilters;
}

static QString selFilter(const QString& filter, DWORD idx)
{
    QStringList filterLst = makeFiltersList(filter);
    return filterLst[(int)idx - 1];
}

static QString tFilters, tTitle, tInitDir;

static
OPENFILENAME* makeOFN(QWidget* parent,
                       const QString& initialSelection,
                       const QString& initialDirectory,
                       const QString& title,
                       const QString& filters,
                       Q3FileDialog::Mode mode)
{
    if (parent)
        parent = parent->window();
    else
        parent = qApp->activeWindow();

    tInitDir = QDir::toNativeSeparators(initialDirectory);
    tFilters = filters;
    tTitle = title;
    QString initSel = QDir::toNativeSeparators(initialSelection);

    int maxLen = mode == Q3FileDialog::ExistingFiles ? maxMultiLen : maxNameLen;
    wchar_t *tInitSel = new wchar_t[maxLen+1];
    if (initSel.length() > 0 && initSel.length() <= maxLen)
        memcpy(tInitSel, initSel.utf16(), (initSel.length() + 1) * sizeof(wchar_t));
    else
        tInitSel[0] = 0;

    OPENFILENAME* ofn = new OPENFILENAME;
    memset(ofn, 0, sizeof(OPENFILENAME));

    ofn->lStructSize     = sizeof(OPENFILENAME);
    ofn->hwndOwner       = parent ? parent->winId() : 0;
    ofn->lpstrFilter     = (wchar_t*)tFilters.utf16();
    ofn->lpstrFile       = tInitSel;
    ofn->nMaxFile        = maxLen;
    ofn->lpstrInitialDir = (wchar_t*)tInitDir.utf16();
    ofn->lpstrTitle      = (wchar_t*)tTitle.utf16();
    ofn->Flags           = (OFN_NOCHANGEDIR | OFN_HIDEREADONLY);

    if (mode == Q3FileDialog::ExistingFile ||
         mode == Q3FileDialog::ExistingFiles)
        ofn->Flags |= (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);
    if (mode == Q3FileDialog::ExistingFiles)
        ofn->Flags |= (OFN_ALLOWMULTISELECT | OFN_EXPLORER);

    return ofn;
}

static void cleanUpOFN(OPENFILENAME** ofn)
{
    delete (*ofn)->lpstrFile;
    delete *ofn;
    *ofn = 0;
}

QString Q3FileDialog::winGetOpenFileName(const QString &initialSelection,
                                         const QString &filter,
                                         QString* initialDirectory,
                                         QWidget *parent, const char* /*name*/,
                                         const QString& caption,
                                         QString* selectedFilter)
{
    QString result;

    QString isel = initialSelection;

    if (initialDirectory && initialDirectory->left(5) == QLatin1String("file:"))
        initialDirectory->remove(0, 5);
    QFileInfo fi(*initialDirectory);

    if (initialDirectory && !fi.isDir()) {
        *initialDirectory = fi.dirPath(true);
        if (isel.isEmpty())
            isel = fi.fileName();
    }

    if (!fi.exists())
        *initialDirectory = QDir::homeDirPath();

    QString title = caption;
    if (title.isNull())
        title = tr("Open");

    DWORD selFilIdx = 0;

    int idx = 0;
    if (selectedFilter && !selectedFilter->isEmpty()) {
        QStringList filterLst = makeFiltersList(filter);
        idx = filterLst.findIndex(*selectedFilter);
    }

    if (parent) {
        QEvent e(QEvent::WindowBlocked);
        QApplication::sendEvent(parent, &e);
        QApplicationPrivate::enterModal(parent);
    }

    OPENFILENAME* ofn = makeOFN(parent, isel,
                                *initialDirectory, title,
                                winFilter(filter), ExistingFile);
    if (idx)
        ofn->nFilterIndex = idx + 1;
    if (GetOpenFileName(ofn)) {
        result = QString::fromWCharArray(ofn->lpstrFile);
        selFilIdx = ofn->nFilterIndex;
    }
    cleanUpOFN(&ofn);

    if (parent) {
        QApplicationPrivate::leaveModal(parent);
        QEvent e(QEvent::WindowUnblocked);
        QApplication::sendEvent(parent, &e);
    }

    if (result.isEmpty()) {
        return result;
    }
    else {
        QFileInfo fi(result);
        *initialDirectory = fi.dirPath();
        if (selectedFilter)
            *selectedFilter = selFilter(filter, selFilIdx);
        return fi.absFilePath();
    }
}


QString Q3FileDialog::winGetSaveFileName(const QString &initialSelection,
                                         const QString &filter,
                                         QString* initialDirectory,
                                         QWidget *parent, const char* /*name*/,
                                         const QString& caption,
                                         QString* selectedFilter)
{
    QString result;

    QString isel = initialSelection;
    if (initialDirectory && initialDirectory->left(5) == QLatin1String("file:"))
        initialDirectory->remove(0, 5);
    QFileInfo fi(*initialDirectory);

    if (initialDirectory && !fi.isDir()) {
        *initialDirectory = fi.dirPath(true);
        if (isel.isEmpty())
            isel = fi.fileName();
    }

    if (!fi.exists())
        *initialDirectory = QDir::homeDirPath();

    QString title = caption;
    if (title.isNull())
        title = tr("Save As");

    DWORD selFilIdx = 0;

    int idx = 0;
    if (selectedFilter && !selectedFilter->isEmpty()) {
        QStringList filterLst = makeFiltersList(filter);
        idx = filterLst.findIndex(*selectedFilter);
    }

    if (parent) {
        QEvent e(QEvent::WindowBlocked);
        QApplication::sendEvent(parent, &e);
        QApplicationPrivate::enterModal(parent);
    }

    OPENFILENAME* ofn = makeOFN(parent, isel,
                                 *initialDirectory, title,
                                 winFilter(filter), AnyFile);
    if (idx)
        ofn->nFilterIndex = idx + 1;
    if (GetSaveFileName(ofn)) {
        result = QString::fromWCharArray(ofn->lpstrFile);
        selFilIdx = ofn->nFilterIndex;
    }
    cleanUpOFN(&ofn);

    if (parent) {
        QApplicationPrivate::leaveModal(parent);
        QEvent e(QEvent::WindowUnblocked);
        QApplication::sendEvent(parent, &e);
    }

    if (result.isEmpty()) {
        return result;
    }
    else {
        QFileInfo fi(result);
        *initialDirectory = fi.dirPath();
        if (selectedFilter)
            *selectedFilter = selFilter(filter, selFilIdx);
        return fi.absFilePath();
    }
}



QStringList Q3FileDialog::winGetOpenFileNames(const QString &filter,
                                              QString* initialDirectory,
                                              QWidget *parent,
                                              const char* /*name*/,
                                              const QString& caption,
                                              QString* selectedFilter)
{
    QStringList result;
    QFileInfo fi;
    QDir dir;
    QString isel;

    if (initialDirectory && initialDirectory->left(5) == QLatin1String("file:"))
        initialDirectory->remove(0, 5);
    fi = QFileInfo(*initialDirectory);

    if (initialDirectory && !fi.isDir()) {
        *initialDirectory = fi.dirPath(true);
        isel = fi.fileName();
    }

    if (!fi.exists())
        *initialDirectory = QDir::homeDirPath();

    QString title = caption;
    if (title.isNull())
        title = tr("Open ");

    DWORD selFilIdx = 0;

    int idx = 0;
    if (selectedFilter && !selectedFilter->isEmpty()) {
        QStringList filterLst = makeFiltersList(filter);
        idx = filterLst.findIndex(*selectedFilter);
    }

    if (parent) {
        QEvent e(QEvent::WindowBlocked);
        QApplication::sendEvent(parent, &e);
        QApplicationPrivate::enterModal(parent);
    }

    OPENFILENAME* ofn = makeOFN(parent, isel,
                                 *initialDirectory, title,
                                 winFilter(filter), ExistingFiles);
    if (idx)
        ofn->nFilterIndex = idx + 1;
    if (GetOpenFileName(ofn)) {
        QString fileOrDir = QString::fromWCharArray(ofn->lpstrFile);
        selFilIdx = ofn->nFilterIndex;
        int offset = fileOrDir.length() + 1;
        if (ofn->lpstrFile[offset] == 0) {
            // Only one file selected; has full path
            fi.setFile(fileOrDir);
            QString res = fi.absFilePath();
            if (!res.isEmpty())
                result.append(res);
        }
        else {
            // Several files selected; first string is path
            dir.setPath(fileOrDir);
            QString f;
            while (!(f = QString::fromWCharArray(ofn->lpstrFile + offset)).isEmpty()) {
                fi.setFile(dir, f);
                QString res = fi.absFilePath();
                if (!res.isEmpty())
                    result.append(res);
                offset += f.length() + 1;
            }
        }
    }
    cleanUpOFN(&ofn);

    if (parent) {
        QApplicationPrivate::leaveModal(parent);
        QEvent e(QEvent::WindowUnblocked);
        QApplication::sendEvent(parent, &e);
    }

    if (!result.isEmpty()) {
        *initialDirectory = fi.dirPath();    // only save the path if there is a result
        if (selectedFilter)
            *selectedFilter = selFilter(filter, selFilIdx);
    }
    return result;
}

// MFC Directory Dialog. Contrib: Steve Williams (minor parts from Scott Powers)

static int __stdcall winGetExistDirCallbackProc(HWND hwnd,
                                                UINT uMsg,
                                                LPARAM lParam,
                                                LPARAM lpData)
{
#ifndef Q_OS_WINCE
    if (uMsg == BFFM_INITIALIZED && lpData != 0) {
        QString *initDir = (QString *)(lpData);
        if (!initDir->isEmpty()) {
            SendMessage(hwnd, BFFM_SETSELECTION, TRUE, Q_ULONG(initDir->utf16()));
        }
    } else if (uMsg == BFFM_SELCHANGED) {
        wchar_t path[MAX_PATH];
        SHGetPathFromIDList(LPITEMIDLIST(lParam), path);
        QString tmpStr = QString::fromWCharArray(path);
        if (!tmpStr.isEmpty())
            SendMessage(hwnd, BFFM_ENABLEOK, 1, 1);
        else
            SendMessage(hwnd, BFFM_ENABLEOK, 0, 0);
        SendMessage(hwnd, BFFM_SETSTATUSTEXT, 1, Q_ULONG(path));
    }
#endif
    return 0;
}

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
#endif


QString Q3FileDialog::winGetExistingDirectory(const QString& initialDirectory,
                                             QWidget *parent,
                                             const char* /*name*/,
                                             const QString& caption)
{
#ifndef Q_OS_WINCE
    QString currentDir = QDir::currentDirPath();
    QString result;
    if (parent)
        parent = parent->window();
    else
        parent = qApp->activeWindow();
    QString title = caption;
    if (title.isNull())
        title = tr("Select a Directory");

    if (parent) {
        QEvent e(QEvent::WindowBlocked);
        QApplication::sendEvent(parent, &e);
        QApplicationPrivate::enterModal(parent);
    }

    QString initDir = QDir::toNativeSeparators(initialDirectory);
    wchar_t path[MAX_PATH];
    wchar_t initPath[MAX_PATH];
    initPath[0] = 0;
    path[0] = 0;
    tTitle = title;
    BROWSEINFO bi;
    bi.hwndOwner = (parent ? parent->winId() : 0);
    bi.pidlRoot = NULL;
    bi.lpszTitle = (wchar_t*)tTitle.utf16();
    bi.pszDisplayName = initPath;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
    bi.lpfn = winGetExistDirCallbackProc;
    bi.lParam = Q_ULONG(&initDir);
    LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);
    if (pItemIDList) {
        SHGetPathFromIDList(pItemIDList, path);
        IMalloc *pMalloc;
        if (SHGetMalloc(&pMalloc) != NOERROR)
            result.clear();
        else {
            pMalloc->Free(pItemIDList);
            pMalloc->Release();
            result = QString::fromWCharArray(path);
        }
    } else
        result.clear();
    tTitle.clear();

    if (parent) {
        QApplicationPrivate::leaveModal(parent);
        QEvent e(QEvent::WindowUnblocked);
        QApplication::sendEvent(parent, &e);
    }

    if (!result.isEmpty())
        result.replace(QLatin1Char('\\'), QLatin1Char('/'));
    return result;
#else
    return QString();
#endif
}

QT_END_NAMESPACE

#endif
