/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qsystemtrayicon_p.h"
#ifndef QT_NO_SYSTEMTRAYICON
#define _WIN32_IE 0x0600 //required for NOTIFYICONDATA_V2_SIZE

//missing defines for MINGW :
#ifndef NIN_BALLOONTIMEOUT
#define NIN_BALLOONTIMEOUT  (WM_USER + 4)
#endif
#ifndef NIN_BALLOONUSERCLICK
#define NIN_BALLOONUSERCLICK (WM_USER + 5)
#endif

#include <qt_windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <QBitmap>
#include <QLibrary>
#include <QApplication>
#include <QToolTip>
#include <QDesktopWidget>
#include <QSettings>

#if defined(Q_WS_WINCE) && !defined(STANDARDSHELL_UI_MODEL)
#   include <streams.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_WS_WINCE)
static const UINT q_uNOTIFYICONID = 13;     // IDs from 0 to 12 are reserved on WinCE.
#else
static const UINT q_uNOTIFYICONID = 0;
#endif

static uint MYWM_TASKBARCREATED = 0;
#define MYWM_NOTIFYICON (WM_APP+101)

struct Q_NOTIFYICONIDENTIFIER {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    GUID guidItem;
};

typedef HRESULT (WINAPI *PtrShell_NotifyIconGetRect)(const Q_NOTIFYICONIDENTIFIER* identifier, RECT* iconLocation);

class QSystemTrayIconSys : QWidget
{
public:
    QSystemTrayIconSys(QSystemTrayIcon *object);
    ~QSystemTrayIconSys();
    bool winEvent( MSG *m, long *result );
    bool trayMessage(DWORD msg);
    bool iconDrawItem(LPDRAWITEMSTRUCT lpdi);
    void setIconContents(NOTIFYICONDATA &data);
    bool showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs);
    bool allowsMessages();
    bool supportsMessages();
    QRect findIconGeometry(const int a_iButtonID);
    void createIcon();
    HICON hIcon;
    QPoint globalPos;
    QSystemTrayIcon *q;
private:
    uint notifyIconSize;
    int maxTipLength;
    bool ignoreNextMouseRelease;
};

bool QSystemTrayIconSys::allowsMessages()
{
#ifndef QT_NO_SETTINGS
    QSettings settings(QLatin1String("HKEY_CURRENT_USER\\Software\\Microsoft"
                                      "\\Windows\\CurrentVersion\\Explorer\\Advanced"), QSettings::NativeFormat);
    return settings.value(QLatin1String("EnableBalloonTips"), true).toBool();
#else
    return false;
#endif
}

bool QSystemTrayIconSys::supportsMessages()
{
#ifndef Q_OS_WINCE
    return allowsMessages();
#endif
    return false;
}

QSystemTrayIconSys::QSystemTrayIconSys(QSystemTrayIcon *object)
    : hIcon(0), q(object), ignoreNextMouseRelease(false)

{
#ifndef Q_OS_WINCE
    notifyIconSize = FIELD_OFFSET(NOTIFYICONDATA, guidItem); // NOTIFYICONDATAW_V2_SIZE;
    maxTipLength = 128;
#else
    notifyIconSize = FIELD_OFFSET(NOTIFYICONDATA, szTip[64]); // NOTIFYICONDATAW_V1_SIZE;
    maxTipLength = 64;
#endif

    // For restoring the tray icon after explorer crashes
    if (!MYWM_TASKBARCREATED) {
        MYWM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");
    }
}

QSystemTrayIconSys::~QSystemTrayIconSys()
{
    if (hIcon)
        DestroyIcon(hIcon);
}

void QSystemTrayIconSys::setIconContents(NOTIFYICONDATA &tnd)
{
    tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnd.uCallbackMessage = MYWM_NOTIFYICON;
    tnd.hIcon = hIcon;
    QString tip = q->toolTip();

    if (!tip.isNull()) {
        tip = tip.left(maxTipLength - 1) + QChar();
        memcpy(tnd.szTip, tip.utf16(), qMin(tip.length() + 1, maxTipLength) * sizeof(wchar_t));
    }
}

static int iconFlag( QSystemTrayIcon::MessageIcon icon )
{
#if NOTIFYICON_VERSION >= 3
    switch (icon) {
        case QSystemTrayIcon::Information:
            return NIIF_INFO;
        case QSystemTrayIcon::Warning:
            return NIIF_WARNING;
        case QSystemTrayIcon::Critical:
            return NIIF_ERROR;
        case QSystemTrayIcon::NoIcon:
            return NIIF_NONE;
        default:
            Q_ASSERT_X(false, "QSystemTrayIconSys::showMessage", "Invalid QSystemTrayIcon::MessageIcon value");
            return NIIF_NONE;
    }
#else
    Q_UNUSED(icon);
    return 0;
#endif
}

bool QSystemTrayIconSys::showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs)
{
#if NOTIFYICON_VERSION >= 3
    NOTIFYICONDATA tnd;
    memset(&tnd, 0, notifyIconSize);
    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    setIconContents(tnd);
    memcpy(tnd.szInfo, message.utf16(), qMin(message.length() + 1, 256) * sizeof(wchar_t));
    memcpy(tnd.szInfoTitle, title.utf16(), qMin(title.length() + 1, 64) * sizeof(wchar_t));

    tnd.uID = q_uNOTIFYICONID;
    tnd.dwInfoFlags = iconFlag(type);
    tnd.cbSize = notifyIconSize;
    tnd.hWnd = winId();
    tnd.uTimeout = uSecs;
    tnd.uFlags = NIF_INFO;

    return Shell_NotifyIcon(NIM_MODIFY, &tnd);
#else
    Q_UNUSED(title);
    Q_UNUSED(message);
    Q_UNUSED(type);
    Q_UNUSED(uSecs);
    return false;
#endif
}

bool QSystemTrayIconSys::trayMessage(DWORD msg)
{
    NOTIFYICONDATA tnd;
    memset(&tnd, 0, notifyIconSize);
    tnd.uID = q_uNOTIFYICONID;
    tnd.cbSize = notifyIconSize;
    tnd.hWnd = winId();

    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    if (msg != NIM_DELETE) {
        setIconContents(tnd);
    }

    return Shell_NotifyIcon(msg, &tnd);
}

bool QSystemTrayIconSys::iconDrawItem(LPDRAWITEMSTRUCT lpdi)
{
    if (!hIcon)
        return false;

    DrawIconEx(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, hIcon, 0, 0, 0, 0, DI_NORMAL);
    return true;
}

void QSystemTrayIconSys::createIcon()
{
    hIcon = 0;
    QIcon icon = q->icon();
    if (icon.isNull())
        return;

    const int iconSizeX = GetSystemMetrics(SM_CXSMICON);
    const int iconSizeY = GetSystemMetrics(SM_CYSMICON);
    QSize size = icon.actualSize(QSize(iconSizeX, iconSizeY));
    QPixmap pm = icon.pixmap(size);
    if (pm.isNull())
        return;

    hIcon = pm.toWinHICON();
}

bool QSystemTrayIconSys::winEvent( MSG *m, long *result )
{
    switch(m->message) {
    case WM_CREATE:
#ifdef GWLP_USERDATA
        SetWindowLongPtr(winId(), GWLP_USERDATA, (LONG_PTR)((CREATESTRUCTW*)m->lParam)->lpCreateParams);
#else
        SetWindowLong(winId(), GWL_USERDATA, (LONG)((CREATESTRUCTW*)m->lParam)->lpCreateParams);
#endif
        break;

    case WM_DRAWITEM:
        return iconDrawItem((LPDRAWITEMSTRUCT)m->lParam);

    case MYWM_NOTIFYICON:
        {
            RECT r;
            GetWindowRect(winId(), &r);
            QEvent *e = 0;
            Qt::KeyboardModifiers keys = QApplication::keyboardModifiers();
            QPoint gpos = QCursor::pos();

            switch (m->lParam) {
            case WM_LBUTTONUP:
                if (ignoreNextMouseRelease)
                    ignoreNextMouseRelease = false;
                else 
                    emit q->activated(QSystemTrayIcon::Trigger);
                break;

            case WM_LBUTTONDBLCLK:
                ignoreNextMouseRelease = true; // Since DBLCLICK Generates a second mouse 
                                               // release we must ignore it
                emit q->activated(QSystemTrayIcon::DoubleClick);
                break;

            case WM_RBUTTONUP:
                if (q->contextMenu()) {
                    q->contextMenu()->popup(gpos);
#if defined(Q_WS_WINCE)
                    // We must ensure that the popup menu doesn't show up behind the task bar.
                    QRect desktopRect = qApp->desktop()->availableGeometry();
                    int maxY = desktopRect.y() + desktopRect.height() - q->contextMenu()->height();
                    if (gpos.y() > maxY) {
                        gpos.ry() = maxY;
                        q->contextMenu()->move(gpos);
                    }
#endif
                    q->contextMenu()->activateWindow();
                    //Must be activated for proper keyboardfocus and menu closing on windows:
                }
                emit q->activated(QSystemTrayIcon::Context);
                break;

#if !defined(Q_WS_WINCE)
            case NIN_BALLOONUSERCLICK:
                emit q->messageClicked();
                break;
#endif

            case WM_MBUTTONUP:
                emit q->activated(QSystemTrayIcon::MiddleClick);
                break;
            default:
                        break;
            }
            if (e) {
                bool res = QApplication::sendEvent(q, e);
                delete e;
                return res;
            }
            break;
        }
    default:
        if (m->message == MYWM_TASKBARCREATED)
            trayMessage(NIM_ADD);
        else
            return QWidget::winEvent(m, result);
        break;
    }
    return 0;
}

void QSystemTrayIconPrivate::install_sys()
{
    Q_Q(QSystemTrayIcon);
    if (!sys) {
        sys = new QSystemTrayIconSys(q);
        sys->createIcon();
        sys->trayMessage(NIM_ADD);
    }
}

/*
* This function tries to determine the icon geometry from the tray
*
* If it fails an invalid rect is returned.
*/
QRect QSystemTrayIconSys::findIconGeometry(const int iconId)
{
    static PtrShell_NotifyIconGetRect Shell_NotifyIconGetRect =
        (PtrShell_NotifyIconGetRect)QLibrary::resolve(QLatin1String("shell32"), "Shell_NotifyIconGetRect");

    if (Shell_NotifyIconGetRect) {
        Q_NOTIFYICONIDENTIFIER nid;
        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(nid);
        nid.hWnd = winId();
        nid.uID = iconId;

        RECT rect;
        HRESULT hr = Shell_NotifyIconGetRect(&nid, &rect);
        if (SUCCEEDED(hr)) {
            return QRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        }
    }

    QRect ret;

    TBBUTTON buttonData;
    DWORD processID = 0;
    HWND trayHandle = FindWindow(L"Shell_TrayWnd", NULL);

    //find the toolbar used in the notification area
    if (trayHandle) {
#if defined(Q_OS_WINCE)
        trayHandle = FindWindow(L"TrayNotifyWnd", NULL);
#else
        trayHandle = FindWindowEx(trayHandle, NULL, L"TrayNotifyWnd", NULL);
#endif
        if (trayHandle) {
#if defined(Q_OS_WINCE)
            HWND hwnd = FindWindow(L"SysPager", NULL);
#else
            HWND hwnd = FindWindowEx(trayHandle, NULL, L"SysPager", NULL);
#endif
            if (hwnd) {
#if defined(Q_OS_WINCE)
                hwnd = FindWindow(L"ToolbarWindow32", NULL);
#else
                hwnd = FindWindowEx(hwnd, NULL, L"ToolbarWindow32", NULL);
#endif
                if (hwnd)
                    trayHandle = hwnd;
            }
        }
    }

    if (!trayHandle)
        return ret;

    GetWindowThreadProcessId(trayHandle, &processID);
    if (processID <= 0)
        return ret;

    HANDLE trayProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, processID);
    if (!trayProcess)
        return ret;

    int buttonCount = SendMessage(trayHandle, TB_BUTTONCOUNT, 0, 0);
#if defined(Q_OS_WINCE)
    LPVOID data = VirtualAlloc(NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);
#else
    LPVOID data = VirtualAllocEx(trayProcess, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);
#endif

    if ( buttonCount < 1 || !data ) {
        CloseHandle(trayProcess);
        return ret;
    }

    //search for our icon among all toolbar buttons
    for (int toolbarButton = 0; toolbarButton  < buttonCount; ++toolbarButton ) {
        SIZE_T numBytes = 0;
        DWORD appData[2] = { 0, 0 };
        SendMessage(trayHandle, TB_GETBUTTON, toolbarButton , (LPARAM)data);

        if (!ReadProcessMemory(trayProcess, data, &buttonData, sizeof(TBBUTTON), &numBytes))
            continue;

        if (!ReadProcessMemory(trayProcess, (LPVOID) buttonData.dwData, appData, sizeof(appData), &numBytes))
            continue;

        int currentIconId = appData[1];
        HWND currentIconHandle = (HWND) appData[0];
        bool isHidden = buttonData.fsState & TBSTATE_HIDDEN;

        if (currentIconHandle == winId() &&
            currentIconId == iconId && !isHidden) {
            SendMessage(trayHandle, TB_GETITEMRECT, toolbarButton , (LPARAM)data);
            RECT iconRect = {0, 0};
            if(ReadProcessMemory(trayProcess, data, &iconRect, sizeof(RECT), &numBytes)) {
                MapWindowPoints(trayHandle, NULL, (LPPOINT)&iconRect, 2);
                QRect geometry(iconRect.left + 1, iconRect.top + 1,
                                iconRect.right - iconRect.left - 2,
                                iconRect.bottom - iconRect.top - 2);
                if (geometry.isValid())
                    ret = geometry;
                break;
            }
        }
    }
#if defined(Q_OS_WINCE)
    VirtualFree(data, 0, MEM_RELEASE);
#else
    VirtualFreeEx(trayProcess, data, 0, MEM_RELEASE);
#endif
    CloseHandle(trayProcess);
    return ret;
}

void QSystemTrayIconPrivate::showMessage_sys(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, int timeOut)
{
    if (!sys || !sys->allowsMessages())
        return;

    uint uSecs = 0;
    if ( timeOut < 0)
        uSecs = 10000; //10 sec default
    else uSecs = (int)timeOut;

    //message is limited to 255 chars + NULL
    QString messageString;
    if (message.isEmpty() && !title.isEmpty())
        messageString = QLatin1Char(' '); //ensures that the message shows when only title is set
    else
        messageString = message.left(255) + QChar();

    //title is limited to 63 chars + NULL
    QString titleString = title.left(63) + QChar();

    if (sys->supportsMessages()) {
        sys->showMessage(titleString, messageString, type, (unsigned int)uSecs);
    } else {
        //use fallback
        QRect iconPos = sys->findIconGeometry(q_uNOTIFYICONID);
        if (iconPos.isValid()) {
            QBalloonTip::showBalloon(type, title, message, sys->q, iconPos.center(), uSecs, true);
        }
    }
}

QRect QSystemTrayIconPrivate::geometry_sys() const
{
    if (!sys)
        return QRect();
    return sys->findIconGeometry(q_uNOTIFYICONID);
}

void QSystemTrayIconPrivate::remove_sys()
{
    if (!sys)
        return;

    sys->trayMessage(NIM_DELETE);
    delete sys;
    sys = 0;
}

void QSystemTrayIconPrivate::updateIcon_sys()
{
    if (!sys)
        return;

    HICON hIconToDestroy = sys->hIcon;

    sys->createIcon();
    sys->trayMessage(NIM_MODIFY);

    if (hIconToDestroy)
        DestroyIcon(hIconToDestroy);
}

void QSystemTrayIconPrivate::updateMenu_sys()
{

}

void QSystemTrayIconPrivate::updateToolTip_sys()
{
#ifdef Q_WS_WINCE
    // Calling sys->trayMessage(NIM_MODIFY) on an existing icon is broken on Windows CE.
    // So we need to call updateIcon_sys() which creates a new icon handle.
    updateIcon_sys();
#else
    if (!sys)
        return;

    sys->trayMessage(NIM_MODIFY);
#endif
}

bool QSystemTrayIconPrivate::isSystemTrayAvailable_sys()
{
    return true;
}

QT_END_NAMESPACE

#endif
