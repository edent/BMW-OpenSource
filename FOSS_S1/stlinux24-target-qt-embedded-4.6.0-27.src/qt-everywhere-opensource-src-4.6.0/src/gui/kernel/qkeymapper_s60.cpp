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

#include "private/qkeymapper_p.h"
#include <e32keys.h>

QT_BEGIN_NAMESPACE

QKeyMapperPrivate::QKeyMapperPrivate()
{
    fillKeyMap();
}

QKeyMapperPrivate::~QKeyMapperPrivate()
{
}

QList<int> QKeyMapperPrivate::possibleKeys(QKeyEvent * /* e */)
{
    QList<int> result;
    return result;
}

void QKeyMapperPrivate::clearMappings()
{
    // stub
}

QString QKeyMapperPrivate::translateKeyEvent(int keySym, Qt::KeyboardModifiers /* modifiers */)
{
    if (keySym >= Qt::Key_Escape)
        return QString();

    // Symbian doesn't actually use modifiers, but gives us the character code directly.

    return QString(QChar(keySym));
}

void QKeyMapperPrivate::fillKeyMap()
{
    using namespace Qt;
    static const struct {
        TUint s60Key;
        int qtKey;
    } map[] = {
        {EKeyBell, Key_unknown},
        {EKeyBackspace, Key_Backspace},
        {EKeyTab, Key_Tab},
        {EKeyLineFeed, Key_unknown},
        {EKeyVerticalTab, Key_unknown},
        {EKeyFormFeed, Key_unknown},
        {EKeyEnter, Key_Enter},
        {EKeyEscape, Key_Escape},
        {EKeySpace, Key_Space},
        {EKeyDelete, Key_Delete},
        {EKeyPrintScreen, Key_SysReq},
        {EKeyPause, Key_Pause},
        {EKeyHome, Key_Home},
        {EKeyEnd, Key_End},
        {EKeyPageUp, Key_PageUp},
        {EKeyPageDown, Key_PageDown},
        {EKeyInsert, Key_Insert},
        {EKeyLeftArrow, Key_Left},
        {EKeyRightArrow, Key_Right},
        {EKeyUpArrow, Key_Up},
        {EKeyDownArrow, Key_Down},
        {EKeyLeftShift, Key_Shift},
        {EKeyRightShift, Key_Shift},
        {EKeyLeftAlt, Key_Alt},
        {EKeyRightAlt, Key_AltGr},
        {EKeyLeftCtrl, Key_Control},
        {EKeyRightCtrl, Key_Control},
        {EKeyLeftFunc, Key_Super_L},
        {EKeyRightFunc, Key_Super_R},
        {EKeyCapsLock, Key_CapsLock},
        {EKeyNumLock, Key_NumLock},
        {EKeyScrollLock, Key_ScrollLock},
        {EKeyF1, Key_F1},
        {EKeyF2, Key_F2},
        {EKeyF3, Key_F3},
        {EKeyF4, Key_F4},
        {EKeyF5, Key_F5},
        {EKeyF6, Key_F6},
        {EKeyF7, Key_F7},
        {EKeyF8, Key_F8},
        {EKeyF9, Key_F9},
        {EKeyF10, Key_F10},
        {EKeyF11, Key_F11},
        {EKeyF12, Key_F12},
        {EKeyF13, Key_F13},
        {EKeyF14, Key_F14},
        {EKeyF15, Key_F15},
        {EKeyF16, Key_F16},
        {EKeyF17, Key_F17},
        {EKeyF18, Key_F18},
        {EKeyF19, Key_F19},
        {EKeyF20, Key_F20},
        {EKeyF21, Key_F21},
        {EKeyF22, Key_F22},
        {EKeyF23, Key_F23},
        {EKeyF24, Key_F24},
        {EKeyOff, Key_unknown},
        {EKeyIncContrast, Key_unknown},
        {EKeyDecContrast, Key_unknown},
        {EKeyBacklightOn, Key_unknown},
        {EKeyBacklightOff, Key_unknown},
        {EKeyBacklightToggle, Key_unknown},
        {EKeySliderDown, Key_unknown},
        {EKeySliderUp, Key_unknown},
        {EKeyMenu, Key_Menu},
        {EKeyDictaphonePlay, Key_unknown},
        {EKeyDictaphoneStop, Key_unknown},
        {EKeyDictaphoneRecord, Key_unknown},
        {EKeyHelp, Key_unknown},
        {EKeyDial, Key_Call},
        {EKeyScreenDimension0, Key_unknown},
        {EKeyScreenDimension1, Key_unknown},
        {EKeyScreenDimension2, Key_unknown},
        {EKeyScreenDimension3, Key_unknown},
        {EKeyIncVolume, Key_unknown},
        {EKeyDecVolume, Key_unknown},
        {EKeyDevice0, Key_Context1}, // Found by manual testing, left softkey.
        {EKeyDevice1, Key_Context2}, // Found by manual testing.
        {EKeyDevice2, Key_unknown},
        {EKeyDevice3, Key_Select}, // Found by manual testing.
        {EKeyDevice4, Key_unknown},
        {EKeyDevice5, Key_unknown},
        {EKeyDevice6, Key_unknown},
        {EKeyDevice7, Key_unknown},
        {EKeyDevice8, Key_unknown},
        {EKeyDevice9, Key_unknown},
        {EKeyDeviceA, Key_unknown},
        {EKeyDeviceB, Key_unknown},
        {EKeyDeviceC, Key_unknown},
        {EKeyDeviceD, Key_unknown},
        {EKeyDeviceE, Key_unknown},
        {EKeyDeviceF, Key_unknown},
        {EKeyApplication0, Key_Launch0},
        {EKeyApplication1, Key_Launch1},
        {EKeyApplication2, Key_Launch2},
        {EKeyApplication3, Key_Launch3},
        {EKeyApplication4, Key_Launch4},
        {EKeyApplication5, Key_Launch5},
        {EKeyApplication6, Key_Launch6},
        {EKeyApplication7, Key_Launch7},
        {EKeyApplication8, Key_Launch8},
        {EKeyApplication9, Key_Launch9},
        {EKeyApplicationA, Key_LaunchA},
        {EKeyApplicationB, Key_LaunchB},
        {EKeyApplicationC, Key_LaunchC},
        {EKeyApplicationD, Key_LaunchD},
        {EKeyApplicationE, Key_LaunchE},
        {EKeyApplicationF, Key_LaunchF},
        {EKeyYes, Key_Yes},
        {EKeyNo, Key_No},
        {EKeyIncBrightness, Key_unknown},
        {EKeyDecBrightness, Key_unknown},
        {EKeyKeyboardExtend, Key_unknown},
        {EKeyDevice10, Key_unknown},
        {EKeyDevice11, Key_unknown},
        {EKeyDevice12, Key_unknown},
        {EKeyDevice13, Key_unknown},
        {EKeyDevice14, Key_unknown},
        {EKeyDevice15, Key_unknown},
        {EKeyDevice16, Key_unknown},
        {EKeyDevice17, Key_unknown},
        {EKeyDevice18, Key_unknown},
        {EKeyDevice19, Key_unknown},
        {EKeyDevice1A, Key_unknown},
        {EKeyDevice1B, Key_unknown},
        {EKeyDevice1C, Key_unknown},
        {EKeyDevice1D, Key_unknown},
        {EKeyDevice1E, Key_unknown},
        {EKeyDevice1F, Key_unknown},
        {EKeyApplication10, Key_unknown},
        {EKeyApplication11, Key_unknown},
        {EKeyApplication12, Key_unknown},
        {EKeyApplication13, Key_unknown},
        {EKeyApplication14, Key_unknown},
        {EKeyApplication15, Key_unknown},
        {EKeyApplication16, Key_unknown},
        {EKeyApplication17, Key_unknown},
        {EKeyApplication18, Key_unknown},
        {EKeyApplication19, Key_unknown},
        {EKeyApplication1A, Key_unknown},
        {EKeyApplication1B, Key_unknown},
        {EKeyApplication1C, Key_unknown},
        {EKeyApplication1D, Key_unknown},
        {EKeyApplication1E, Key_unknown},
        {EKeyApplication1F, Key_unknown}
    };
    const int mapSize = int(sizeof(map)/sizeof(map[0]));
    s60ToQtKeyMap.reserve(mapSize + 5); // +5? docs: Ideally, slightly more than number of items
    for (int i = 0; i < mapSize; ++i)
        s60ToQtKeyMap.insert(map[i].s60Key, map[i].qtKey);
}

int QKeyMapperPrivate::mapS60KeyToQt(TUint s60key)
{
    QHash<TUint, int>::const_iterator mapping;
    mapping = s60ToQtKeyMap.find(s60key);
    if (mapping != s60ToQtKeyMap.end()) {
        return *mapping;
    } else {
        return Qt::Key_unknown;
    }
}

QT_END_NAMESPACE
