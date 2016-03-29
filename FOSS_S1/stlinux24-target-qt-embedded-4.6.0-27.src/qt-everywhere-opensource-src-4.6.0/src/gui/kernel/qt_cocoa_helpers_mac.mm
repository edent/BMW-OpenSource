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

/****************************************************************************
**
** Copyright (c) 2007-2008, Apple, Inc.
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
**   * Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**   * Redistributions in binary form must reproduce the above copyright notice,
**     this list of conditions and the following disclaimer in the documentation
**     and/or other materials provided with the distribution.
**
**   * Neither the name of Apple, Inc. nor the names of its contributors
**     may be used to endorse or promote products derived from this software
**     without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include <private/qcore_mac_p.h>
#include <qaction.h>
#include <qwidget.h>
#include <qdesktopwidget.h>
#include <qevent.h>
#include <qpixmapcache.h>
#include <private/qevent_p.h>
#include <private/qt_cocoa_helpers_mac_p.h>
#include <private/qt_mac_p.h>
#include <private/qapplication_p.h>
#include <private/qcocoawindow_mac_p.h>
#include <private/qcocoaview_mac_p.h>
#include <private/qkeymapper_p.h>
#include <private/qwidget_p.h>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QMacWindowFader, macwindowFader);

QMacWindowFader::QMacWindowFader()
    : m_duration(0.250)
{
}

QMacWindowFader *QMacWindowFader::currentFader()
{
    return macwindowFader();
}

void QMacWindowFader::registerWindowToFade(QWidget *window)
{
    m_windowsToFade.append(window);
}

void QMacWindowFader::performFade()
{
    const QWidgetList myWidgetsToFade = m_windowsToFade;
    const int widgetCount = myWidgetsToFade.count();
#if QT_MAC_USE_COCOA
    QMacCocoaAutoReleasePool pool;
    [NSAnimationContext beginGrouping];
    [[NSAnimationContext currentContext] setDuration:NSTimeInterval(m_duration)];
#endif

    for (int i = 0; i < widgetCount; ++i) {
        QWidget *widget = m_windowsToFade.at(i);
        OSWindowRef window = qt_mac_window_for(widget);
#if QT_MAC_USE_COCOA
        [[window animator] setAlphaValue:0.0];
        QTimer::singleShot(qRound(m_duration * 1000), widget, SLOT(hide()));
#else
        TransitionWindowOptions options = {0, m_duration, 0, 0};
        TransitionWindowWithOptions(window, kWindowFadeTransitionEffect, kWindowHideTransitionAction,
                                    0, 1, &options);
#endif
    }
#if QT_MAC_USE_COCOA
    [NSAnimationContext endGrouping];
#endif
    m_duration = 0.250;
    m_windowsToFade.clear();
}

extern bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event); // qapplication.cpp;
extern Qt::MouseButton cocoaButton2QtButton(NSInteger buttonNum); // qcocoaview.mm
extern QWidget * mac_mouse_grabber;

void macWindowFade(void * /*OSWindowRef*/ window, float durationSeconds)
{
    OSWindowRef wnd = static_cast<OSWindowRef>(window);
    if (wnd) {
        QWidget *widget;
#if QT_MAC_USE_COCOA
        widget = [wnd QT_MANGLE_NAMESPACE(qt_qwidget)];
#else
    const UInt32 kWidgetCreatorQt = kEventClassQt;
    enum {
        kWidgetPropertyQWidget = 'QWId' //QWidget *
    };
        if (GetWindowProperty(static_cast<WindowRef>(window), kWidgetCreatorQt, kWidgetPropertyQWidget, sizeof(widget), 0, &widget) != noErr)
            widget = 0;
#endif
        if (widget) {
            QMacWindowFader::currentFader()->setFadeDuration(durationSeconds);
            QMacWindowFader::currentFader()->registerWindowToFade(widget);
            QMacWindowFader::currentFader()->performFade();
        }
    }
}

bool macWindowIsTextured( void * /*OSWindowRef*/ window )
{
    OSWindowRef wnd = static_cast<OSWindowRef>(window);
#if QT_MAC_USE_COCOA
	return ( [wnd styleMask] & NSTexturedBackgroundWindowMask ) ? true : false;
#else
	WindowAttributes currentAttributes;
	GetWindowAttributes(wnd, &currentAttributes);
	return (currentAttributes & kWindowMetalAttribute) ? true : false;
#endif
}

void macWindowToolbarShow(const QWidget *widget, bool show )
{
    OSWindowRef wnd = qt_mac_window_for(widget);
#if QT_MAC_USE_COCOA
    if (NSToolbar *toolbar = [wnd toolbar]) {
        QMacCocoaAutoReleasePool pool;
        if (show != [toolbar isVisible]) {
           [toolbar setVisible:show];
        } else {
            // The toolbar may be in sync, but we are not, update our framestrut.
            qt_widget_private(const_cast<QWidget *>(widget))->updateFrameStrut();
        }
    }
#else
    ShowHideWindowToolbar(wnd, show, false);
#endif
}


void macWindowToolbarSet( void * /*OSWindowRef*/ window, void *toolbarRef  )
{
    OSWindowRef wnd = static_cast<OSWindowRef>(window);
#if QT_MAC_USE_COCOA
    [wnd setToolbar:static_cast<NSToolbar *>(toolbarRef)];
#else
    SetWindowToolbar(wnd, static_cast<HIToolbarRef>(toolbarRef));
#endif
}

bool macWindowToolbarIsVisible( void * /*OSWindowRef*/ window )
{
    OSWindowRef wnd = static_cast<OSWindowRef>(window);
#if QT_MAC_USE_COCOA
    if (NSToolbar *toolbar = [wnd toolbar])
        return [toolbar isVisible];
    return false;
#else
    return IsWindowToolbarVisible(wnd);
#endif
}

void macWindowSetHasShadow( void * /*OSWindowRef*/ window, bool hasShadow  )
{
    OSWindowRef wnd = static_cast<OSWindowRef>(window);
#if QT_MAC_USE_COCOA
    [wnd setHasShadow:BOOL(hasShadow)];
#else
    if (hasShadow)
        ChangeWindowAttributes(wnd, 0, kWindowNoShadowAttribute);
    else
        ChangeWindowAttributes(wnd, kWindowNoShadowAttribute, 0);
#endif
}

void macWindowFlush(void * /*OSWindowRef*/ window)
{
    OSWindowRef wnd = static_cast<OSWindowRef>(window);
#if QT_MAC_USE_COCOA
    [wnd flushWindowIfNeeded];
#else
    HIWindowFlush(wnd);
#endif
}

void * /*NSImage */qt_mac_create_nsimage(const QPixmap &pm)
{
    QMacCocoaAutoReleasePool pool;
    if(QCFType<CGImageRef> image = pm.toMacCGImageRef()) {
        NSImage *newImage = 0;
        NSRect imageRect = NSMakeRect(0.0, 0.0, CGImageGetWidth(image), CGImageGetHeight(image));
        newImage = [[NSImage alloc] initWithSize:imageRect.size];
        [newImage lockFocus];
        {
            CGContextRef imageContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
            CGContextDrawImage(imageContext, *(CGRect*)&imageRect, image);
        }
        [newImage unlockFocus];
        return newImage;
    }
    return 0;
}

void qt_mac_update_mouseTracking(QWidget *widget)
{
#ifdef QT_MAC_USE_COCOA
    [qt_mac_nativeview_for(widget) updateTrackingAreas];
#else
    Q_UNUSED(widget);
#endif
}

OSStatus qt_mac_drawCGImage(CGContextRef inContext, const CGRect *inBounds, CGImageRef inImage)
{
    // Verbatim copy if HIViewDrawCGImage (as shown on Carbon-Dev)
    OSStatus err = noErr;

    require_action(inContext != NULL, InvalidContext, err = paramErr);
    require_action(inBounds != NULL, InvalidBounds, err = paramErr);
    require_action(inImage != NULL, InvalidImage, err = paramErr);

    CGContextSaveGState( inContext );
    CGContextTranslateCTM (inContext, 0, inBounds->origin.y + CGRectGetMaxY(*inBounds));
    CGContextScaleCTM(inContext, 1, -1);

    CGContextDrawImage(inContext, *inBounds, inImage);

    CGContextRestoreGState(inContext);
InvalidImage:
InvalidBounds:
InvalidContext:
	return err;
}

bool qt_mac_checkForNativeSizeGrip(const QWidget *widget)
{
#ifndef QT_MAC_USE_COCOA
    OSViewRef nativeSizeGrip = 0;
    HIViewFindByID(HIViewGetRoot(HIViewGetWindow(HIViewRef(widget->winId()))), kHIViewWindowGrowBoxID, &nativeSizeGrip);
    return (nativeSizeGrip != 0);
#else
    return [[reinterpret_cast<NSView *>(widget->winId()) window] showsResizeIndicator];
#endif
}
struct qt_mac_enum_mapper
{
    int mac_code;
    int qt_code;
#if defined(DEBUG_MOUSE_MAPS)
#   define QT_MAC_MAP_ENUM(x) x, #x
    const char *desc;
#else
#   define QT_MAC_MAP_ENUM(x) x
#endif
};

//mouse buttons
static qt_mac_enum_mapper qt_mac_mouse_symbols[] = {
{ kEventMouseButtonPrimary, QT_MAC_MAP_ENUM(Qt::LeftButton) },
{ kEventMouseButtonSecondary, QT_MAC_MAP_ENUM(Qt::RightButton) },
{ kEventMouseButtonTertiary, QT_MAC_MAP_ENUM(Qt::MidButton) },
{ 4, QT_MAC_MAP_ENUM(Qt::XButton1) },
{ 5, QT_MAC_MAP_ENUM(Qt::XButton2) },
{ 0, QT_MAC_MAP_ENUM(0) }
};
Qt::MouseButtons qt_mac_get_buttons(int buttons)
{
#ifdef DEBUG_MOUSE_MAPS
    qDebug("Qt: internal: **Mapping buttons: %d (0x%04x)", buttons, buttons);
#endif
    Qt::MouseButtons ret = Qt::NoButton;
    for(int i = 0; qt_mac_mouse_symbols[i].qt_code; i++) {
        if (buttons & (0x01<<(qt_mac_mouse_symbols[i].mac_code-1))) {
#ifdef DEBUG_MOUSE_MAPS
            qDebug("Qt: internal: got button: %s", qt_mac_mouse_symbols[i].desc);
#endif
            ret |= Qt::MouseButtons(qt_mac_mouse_symbols[i].qt_code);
        }
    }
    return ret;
}
Qt::MouseButton qt_mac_get_button(EventMouseButton button)
{
#ifdef DEBUG_MOUSE_MAPS
    qDebug("Qt: internal: **Mapping button: %d (0x%04x)", button, button);
#endif
    Qt::MouseButtons ret = 0;
    for(int i = 0; qt_mac_mouse_symbols[i].qt_code; i++) {
        if (button == qt_mac_mouse_symbols[i].mac_code) {
#ifdef DEBUG_MOUSE_MAPS
            qDebug("Qt: internal: got button: %s", qt_mac_mouse_symbols[i].desc);
#endif
            return Qt::MouseButton(qt_mac_mouse_symbols[i].qt_code);
        }
    }
    return Qt::NoButton;
}

void macSendToolbarChangeEvent(QWidget *widget)
{
    QToolBarChangeEvent ev(!(GetCurrentKeyModifiers() & cmdKey));
    qt_sendSpontaneousEvent(widget, &ev);
}

Q_GLOBAL_STATIC(QMacTabletHash, tablet_hash)
QMacTabletHash *qt_mac_tablet_hash()
{
    return tablet_hash();
}

#ifdef QT_MAC_USE_COCOA
void qt_dispatchTabletProximityEvent(void * /*NSEvent * */ tabletEvent)
{
    NSEvent *proximityEvent = static_cast<NSEvent *>(tabletEvent);
    // simply construct a Carbon proximity record and handle it all in one spot.
    TabletProximityRec carbonProximityRec = { [proximityEvent vendorID],
                                              [proximityEvent tabletID],
                                              [proximityEvent pointingDeviceID],
                                              [proximityEvent deviceID],
                                              [proximityEvent systemTabletID],
                                              [proximityEvent vendorPointingDeviceType],
                                              [proximityEvent pointingDeviceSerialNumber],
                                              [proximityEvent uniqueID],
                                              [proximityEvent capabilityMask],
                                              [proximityEvent pointingDeviceType],
                                              [proximityEvent isEnteringProximity] };
    qt_dispatchTabletProximityEvent(carbonProximityRec);
}
#endif // QT_MAC_USE_COCOA

void qt_dispatchTabletProximityEvent(const ::TabletProximityRec &proxRec)
{
    QTabletDeviceData proximityDevice;
    proximityDevice.tabletUniqueID = proxRec.uniqueID;
    proximityDevice.capabilityMask = proxRec.capabilityMask;

    switch (proxRec.pointerType) {
        case NSUnknownPointingDevice:
        default:
            proximityDevice.tabletPointerType = QTabletEvent::UnknownPointer;
            break;
        case NSPenPointingDevice:
            proximityDevice.tabletPointerType = QTabletEvent::Pen;
            break;
        case NSCursorPointingDevice:
            proximityDevice.tabletPointerType = QTabletEvent::Cursor;
            break;
        case NSEraserPointingDevice:
            proximityDevice.tabletPointerType = QTabletEvent::Eraser;
            break;
    }
    uint bits = proxRec.vendorPointerType;
    if (bits == 0 && proximityDevice.tabletUniqueID != 0) {
        // Fallback. It seems that the driver doesn't always include all the information.
        // High-End Wacom devices store their "type" in the uper bits of the Unique ID.
        // I'm not sure how to handle it for consumer devices, but I'll test that in a bit.
        bits = proximityDevice.tabletUniqueID >> 32;
    }
    // Defined in the "EN0056-NxtGenImpGuideX"
    // on Wacom's Developer Website (www.wacomeng.com)
    if (((bits & 0x0006) == 0x0002) && ((bits & 0x0F06) != 0x0902)) {
        proximityDevice.tabletDeviceType = QTabletEvent::Stylus;
    } else {
        switch (bits & 0x0F06) {
            case 0x0802:
                proximityDevice.tabletDeviceType = QTabletEvent::Stylus;
                break;
            case 0x0902:
                proximityDevice.tabletDeviceType = QTabletEvent::Airbrush;
                break;
            case 0x0004:
                proximityDevice.tabletDeviceType = QTabletEvent::FourDMouse;
                break;
            case 0x0006:
                proximityDevice.tabletDeviceType = QTabletEvent::Puck;
                break;
            case 0x0804:
                proximityDevice.tabletDeviceType = QTabletEvent::RotationStylus;
                break;
            default:
                proximityDevice.tabletDeviceType = QTabletEvent::NoDevice;
        }
    }
    // The deviceID is "unique" while in the proximity, it's a key that we can use for
    // linking up TabletDeviceData to an event (especially if there are two devices in action).
    bool entering = proxRec.enterProximity;
    if (entering) {
        qt_mac_tablet_hash()->insert(proxRec.deviceID, proximityDevice);
    } else {
        qt_mac_tablet_hash()->remove(proxRec.deviceID);
    }

    QTabletEvent qtabletProximity(entering ? QEvent::TabletEnterProximity
                                  : QEvent::TabletLeaveProximity,
                                  QPoint(), QPoint(), QPointF(), proximityDevice.tabletDeviceType,
                                  proximityDevice.tabletPointerType, 0., 0, 0, 0., 0., 0, 0,
                                  proximityDevice.tabletUniqueID);

    qt_sendSpontaneousEvent(qApp, &qtabletProximity);
}

#ifdef QT_MAC_USE_COCOA
// Use this method to keep all the information in the TextSegment. As long as it is ordered
// we are in OK shape, and we can influence that ourselves.
struct KeyPair
{
    QChar cocoaKey;
    Qt::Key qtKey;
};

bool operator==(const KeyPair &entry, QChar qchar)
{
    return entry.cocoaKey == qchar;
}

bool operator<(const KeyPair &entry, QChar qchar)
{
    return entry.cocoaKey < qchar;
}

bool operator<(QChar qchar, const KeyPair &entry)
{
    return qchar < entry.cocoaKey;
}

static Qt::Key cocoaKey2QtKey(QChar keyCode)
{
    static const int NumEntries = 57;
    static const KeyPair entries[NumEntries] = {
        { NSEnterCharacter, Qt::Key_Enter },
        { NSTabCharacter, Qt::Key_Tab },
        { NSCarriageReturnCharacter, Qt::Key_Return },
        { NSBackTabCharacter, Qt::Key_Backtab },
        { kEscapeCharCode, Qt::Key_Escape },
        { NSDeleteCharacter, Qt::Key_Backspace },
        { NSUpArrowFunctionKey, Qt::Key_Up },
        { NSDownArrowFunctionKey, Qt::Key_Down },
        { NSLeftArrowFunctionKey, Qt::Key_Left },
        { NSRightArrowFunctionKey, Qt::Key_Right },
        { NSF1FunctionKey, Qt::Key_F1 },
        { NSF2FunctionKey, Qt::Key_F2 },
        { NSF3FunctionKey, Qt::Key_F3 },
        { NSF4FunctionKey, Qt::Key_F4 },
        { NSF5FunctionKey, Qt::Key_F5 },
        { NSF6FunctionKey, Qt::Key_F6 },
        { NSF7FunctionKey, Qt::Key_F7 },
        { NSF8FunctionKey, Qt::Key_F8 },
        { NSF9FunctionKey, Qt::Key_F8 },
        { NSF10FunctionKey, Qt::Key_F10 },
        { NSF11FunctionKey, Qt::Key_F11 },
        { NSF12FunctionKey, Qt::Key_F12 },
        { NSF13FunctionKey, Qt::Key_F13 },
        { NSF14FunctionKey, Qt::Key_F14 },
        { NSF15FunctionKey, Qt::Key_F15 },
        { NSF16FunctionKey, Qt::Key_F16 },
        { NSF17FunctionKey, Qt::Key_F17 },
        { NSF18FunctionKey, Qt::Key_F18 },
        { NSF19FunctionKey, Qt::Key_F19 },
        { NSF20FunctionKey, Qt::Key_F20 },
        { NSF21FunctionKey, Qt::Key_F21 },
        { NSF22FunctionKey, Qt::Key_F22 },
        { NSF23FunctionKey, Qt::Key_F23 },
        { NSF24FunctionKey, Qt::Key_F24 },
        { NSF25FunctionKey, Qt::Key_F25 },
        { NSF26FunctionKey, Qt::Key_F26 },
        { NSF27FunctionKey, Qt::Key_F27 },
        { NSF28FunctionKey, Qt::Key_F28 },
        { NSF29FunctionKey, Qt::Key_F29 },
        { NSF30FunctionKey, Qt::Key_F30 },
        { NSF31FunctionKey, Qt::Key_F31 },
        { NSF32FunctionKey, Qt::Key_F32 },
        { NSF33FunctionKey, Qt::Key_F33 },
        { NSF34FunctionKey, Qt::Key_F34 },
        { NSF35FunctionKey, Qt::Key_F35 },
        { NSInsertFunctionKey, Qt::Key_Insert },
        { NSDeleteFunctionKey, Qt::Key_Delete },
        { NSHomeFunctionKey, Qt::Key_Home },
        { NSEndFunctionKey, Qt::Key_End },
        { NSPageUpFunctionKey, Qt::Key_PageUp },
        { NSPageDownFunctionKey, Qt::Key_PageDown },
        { NSPrintScreenFunctionKey, Qt::Key_Print },
        { NSScrollLockFunctionKey, Qt::Key_ScrollLock },
        { NSPauseFunctionKey, Qt::Key_Pause },
        { NSSysReqFunctionKey, Qt::Key_SysReq },
        { NSMenuFunctionKey, Qt::Key_Menu },
        { NSHelpFunctionKey, Qt::Key_Help },
    };
    static const KeyPair * const end = entries + NumEntries;
    const KeyPair *i = qBinaryFind(entries, end, keyCode);
    if (i == end)
        return Qt::Key(keyCode.unicode());
    return i->qtKey;
}

Qt::KeyboardModifiers qt_cocoaModifiers2QtModifiers(ulong modifierFlags)
{
    Qt::KeyboardModifiers qtMods =Qt::NoModifier;
    if (modifierFlags &  NSShiftKeyMask)
        qtMods |= Qt::ShiftModifier;
    if (modifierFlags & NSControlKeyMask)
        qtMods |= Qt::MetaModifier;
    if (modifierFlags & NSAlternateKeyMask)
        qtMods |= Qt::AltModifier;
    if (modifierFlags & NSCommandKeyMask)
        qtMods |= Qt::ControlModifier;
    if (modifierFlags & NSNumericPadKeyMask)
        qtMods |= Qt::KeypadModifier;
    return qtMods;
}

Qt::KeyboardModifiers qt_cocoaDragOperation2QtModifiers(uint dragOperations)
{
    Qt::KeyboardModifiers qtMods =Qt::NoModifier;
    if (dragOperations &  NSDragOperationLink)
        qtMods |= Qt::MetaModifier;
    if (dragOperations & NSDragOperationGeneric)
        qtMods |= Qt::ControlModifier;
    if (dragOperations & NSDragOperationCopy)
        qtMods |= Qt::AltModifier;
    return qtMods;
}

static inline QEvent::Type cocoaEvent2QtEvent(NSUInteger eventType)
{
    // Handle the trivial cases that can be determined from the type.
    switch (eventType) {
    case NSKeyDown:
        return QEvent::KeyPress;
    case NSKeyUp:
        return QEvent::KeyRelease;
    case NSLeftMouseDown:
    case NSRightMouseDown:
    case NSOtherMouseDown:
        return QEvent::MouseButtonPress;
    case NSLeftMouseUp:
    case NSRightMouseUp:
    case NSOtherMouseUp:
        return QEvent::MouseButtonRelease;
    case NSMouseMoved:
    case NSLeftMouseDragged:
    case NSRightMouseDragged:
    case NSOtherMouseDragged:
        return QEvent::MouseMove;
    case NSScrollWheel:
        return QEvent::Wheel;
    }
    return QEvent::None;
}

static bool mustUseCocoaKeyEvent()
{
    QCFType<TISInputSourceRef> source = TISCopyCurrentKeyboardInputSource();
    return TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData) == 0;
}

bool qt_dispatchKeyEventWithCocoa(void * /*NSEvent * */ keyEvent, QWidget *widgetToGetEvent)
{
    NSEvent *event = static_cast<NSEvent *>(keyEvent);
    NSString *keyChars = [event charactersIgnoringModifiers];
    int keyLength = [keyChars length];
    if (keyLength == 0)
        return false; // Dead Key, nothing to do!
    bool ignoreText = false;
    Qt::Key qtKey = Qt::Key_unknown;
    if (keyLength == 1) {
        QChar ch([keyChars characterAtIndex:0]);
        if (ch.isLower())
            ch = ch.toUpper();
        qtKey = cocoaKey2QtKey(ch);
        // Do not set the text for Function-Key Unicodes characters (0xF700–0xF8FF).
        ignoreText = (ch.unicode() >= 0xF700 && ch.unicode() <= 0xF8FF);
    }
    Qt::KeyboardModifiers keyMods = qt_cocoaModifiers2QtModifiers([event modifierFlags]);
    QString text;

    // To quote from the Carbon port: This is actually wrong--but it is the best that
    // can be done for now because of the Control/Meta mapping issues
    // (we always get text on the Mac)
    if (!ignoreText && !(keyMods & (Qt::ControlModifier | Qt::MetaModifier)))
        text = QCFString::toQString(reinterpret_cast<CFStringRef>(keyChars));

    UInt32 macScanCode = 1;
    QKeyEventEx ke(cocoaEvent2QtEvent([event type]), qtKey, keyMods, text, [event isARepeat], qMax(1, keyLength),
                   macScanCode, [event keyCode], [event modifierFlags]);
    qt_sendSpontaneousEvent(widgetToGetEvent, &ke);
    return ke.isAccepted();
}
#endif

// Helper to share code between QCocoaWindow and QCocoaView
bool qt_dispatchKeyEvent(void * /*NSEvent * */ keyEvent, QWidget *widgetToGetEvent)
{
#ifndef QT_MAC_USE_COCOA
    Q_UNUSED(keyEvent);
    Q_UNUSED(widgetToGetEvent);
    return false;
#else
    NSEvent *event = static_cast<NSEvent *>(keyEvent);
    EventRef key_event = static_cast<EventRef>(const_cast<void *>([event eventRef]));
    Q_ASSERT(key_event);
    if ([event type] == NSKeyDown) {
        qt_keymapper_private()->updateKeyMap(0, key_event, 0);
    }
    if (widgetToGetEvent == 0)
        return false;

    if (qt_mac_sendMacEventToWidget(widgetToGetEvent, key_event))
        return true;

    if (mustUseCocoaKeyEvent())
        return qt_dispatchKeyEventWithCocoa(keyEvent, widgetToGetEvent);
    bool isAccepted;
    qt_keymapper_private()->translateKeyEvent(widgetToGetEvent, 0, key_event, &isAccepted, true);
    return isAccepted;
#endif
}

void qt_dispatchModifiersChanged(void * /*NSEvent * */flagsChangedEvent, QWidget *widgetToGetEvent)
{
#ifndef QT_MAC_USE_COCOA
    Q_UNUSED(flagsChangedEvent);
    Q_UNUSED(widgetToGetEvent);
#else
    UInt32 modifiers = 0;
    // Sync modifiers with Qt
    NSEvent *event = static_cast<NSEvent *>(flagsChangedEvent);
    EventRef key_event = static_cast<EventRef>(const_cast<void *>([event eventRef]));
    Q_ASSERT(key_event);
    GetEventParameter(key_event, kEventParamKeyModifiers, typeUInt32, 0,
                      sizeof(modifiers), 0, &modifiers);
    extern void qt_mac_send_modifiers_changed(quint32 modifiers, QObject *object);
    qt_mac_send_modifiers_changed(modifiers, widgetToGetEvent);
#endif
}


QPointF flipPoint(const NSPoint &p)
{
    return QPointF(p.x, flipYCoordinate(p.y));
}

NSPoint flipPoint(const QPoint &p)
{
    return NSMakePoint(p.x(), flipYCoordinate(p.y()));
}

NSPoint flipPoint(const QPointF &p)
{
    return NSMakePoint(p.x(), flipYCoordinate(p.y()));
}

void qt_mac_dispatchNCMouseMessage(void * /* NSWindow* */eventWindow, void * /* NSEvent* */mouseEvent,
                                   QWidget *widgetToGetEvent, bool &leftButtonIsRightButton)
{
#ifndef QT_MAC_USE_COCOA
    Q_UNUSED(eventWindow);
    Q_UNUSED(mouseEvent);
    Q_UNUSED(widgetToGetEvent);
    Q_UNUSED(leftButtonIsRightButton);
#else
    if (widgetToGetEvent == 0)
        return;
    NSWindow *window = static_cast<NSWindow *>(eventWindow);
    NSEvent *event = static_cast<NSEvent *>(mouseEvent);
    NSEventType evtType = [event type];

    QPoint qlocalPoint;
    QPoint qglobalPoint;
    bool processThisEvent = false;
    bool fakeNCEvents = false;
    bool fakeMouseEvents = false;

    // Check if this is a mouse event.
    if (evtType == NSLeftMouseDown || evtType == NSLeftMouseUp
        || evtType == NSRightMouseDown || evtType == NSRightMouseUp
        || evtType == NSOtherMouseDown || evtType == NSOtherMouseUp
        || evtType == NSMouseMoved     || evtType == NSLeftMouseDragged
        || evtType == NSRightMouseDragged || evtType == NSOtherMouseDragged) {
        // Check if we want to pass this message to another window
        if (mac_mouse_grabber  && mac_mouse_grabber != widgetToGetEvent) {
            NSWindow *grabWindow = static_cast<NSWindow *>(qt_mac_window_for(mac_mouse_grabber));
            if (window != grabWindow) {
                window = grabWindow;
                widgetToGetEvent = mac_mouse_grabber;
                fakeNCEvents = true;
            }
        }
        // Dont generate normal NC mouse events for Left Button dragged
        if(evtType != NSLeftMouseDragged || fakeNCEvents) {
            NSPoint windowPoint = [event locationInWindow];
            NSPoint globalPoint = [[event window] convertBaseToScreen:windowPoint];
            NSRect frameRect = [window frame];
            if (fakeNCEvents || NSMouseInRect(globalPoint, frameRect, NO)) {
                NSRect contentRect = [window contentRectForFrameRect:frameRect];
                if (fakeNCEvents || !NSMouseInRect(globalPoint, contentRect, NO)) {
                    qglobalPoint = QPoint(flipPoint(globalPoint).toPoint());
                    qlocalPoint = widgetToGetEvent->mapFromGlobal(qglobalPoint);
                    processThisEvent = true;
                }
            }
        }
    }
    // This is not an NC area mouse message.
    if (!processThisEvent)
        return;
    // If the window is frame less, generate fake mouse events instead. (floating QToolBar)
    if (fakeNCEvents && (widgetToGetEvent->window()->windowFlags() & Qt::FramelessWindowHint))
        fakeMouseEvents = true;

    Qt::MouseButton button;
    QEvent::Type eventType;
    // Convert to Qt::Event type
    switch (evtType) {
        case NSLeftMouseDown:
            button = Qt::LeftButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonPress
                                           : QEvent::MouseButtonPress;
            break;
        case NSLeftMouseUp:
            button = Qt::LeftButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonRelease
                                           : QEvent::MouseButtonRelease;
            break;
        case NSRightMouseDown:
            button = Qt::RightButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonPress
                                           : QEvent::MouseButtonPress;
            break;
        case NSRightMouseUp:
            button = Qt::RightButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonRelease
                                           : QEvent::MouseButtonRelease;
            break;
        case NSOtherMouseDown:
            button = cocoaButton2QtButton([event buttonNumber]);
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonPress
                                           : QEvent::MouseButtonPress;
            break;
        case NSOtherMouseUp:
            button = cocoaButton2QtButton([event buttonNumber]);
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonRelease
                                           : QEvent::MouseButtonRelease;
            break;
        case NSMouseMoved:
            button = Qt::NoButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseMove
                                           : QEvent::MouseMove;
            break;
        case NSLeftMouseDragged:
            button = Qt::LeftButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseMove
                                           : QEvent::MouseMove;
            break;
        case NSRightMouseDragged:
            button = Qt::RightButton;
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseMove
                                           : QEvent::MouseMove;
            break;
        case NSOtherMouseDragged:
            button = cocoaButton2QtButton([event buttonNumber]);
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseMove
                                           : QEvent::MouseMove;
            break;
        default:
            qWarning("not handled! Non client area mouse message");
            return;
    }

    Qt::KeyboardModifiers keyMods = qt_cocoaModifiers2QtModifiers([event modifierFlags]);
    if (eventType == QEvent::NonClientAreaMouseButtonPress || eventType == QEvent::MouseButtonPress) {
        NSInteger clickCount = [event clickCount];
        if (clickCount % 2 == 0)
            eventType = (!fakeMouseEvents) ? QEvent::NonClientAreaMouseButtonDblClick
                                           : QEvent::MouseButtonDblClick;
        if (button == Qt::LeftButton && (keyMods & Qt::MetaModifier)) {
            button = Qt::RightButton;
            leftButtonIsRightButton = true;
        }
    } else if (eventType == QEvent::NonClientAreaMouseButtonRelease || eventType == QEvent::MouseButtonRelease) {
        if (button == Qt::LeftButton && leftButtonIsRightButton) {
            button = Qt::RightButton;
            leftButtonIsRightButton = false;
        }
    }
    QMouseEvent qme(eventType, qlocalPoint, qglobalPoint, button, button, keyMods);
    qt_sendSpontaneousEvent(widgetToGetEvent, &qme);
#endif
}

bool qt_mac_handleMouseEvent(void * /* NSView * */view, void * /* NSEvent * */event, QEvent::Type eventType, Qt::MouseButton button)
{
#ifndef QT_MAC_USE_COCOA
    Q_UNUSED(view);
    Q_UNUSED(event);
    Q_UNUSED(eventType);
    Q_UNUSED(button);
    return false;
#else
    QT_MANGLE_NAMESPACE(QCocoaView) *theView = static_cast<QT_MANGLE_NAMESPACE(QCocoaView) *>(view);
    NSEvent *theEvent = static_cast<NSEvent *>(event);

    // Give the Input Manager a chance to process the mouse events.
   NSInputManager *currentIManager = [NSInputManager currentInputManager];
   if (currentIManager && [currentIManager wantsToHandleMouseEvents]) {
        [currentIManager handleMouseEvent:theEvent];
   }

    // Handle tablet events (if any) first.
    if (qt_mac_handleTabletEvent(theView, theEvent)) {
        // Tablet event was handled. In Qt we aren't supposed to send the mouse event.
        return true;
    }

    NSPoint windowPoint = [theEvent locationInWindow];
    NSPoint globalPoint = [[theEvent window] convertBaseToScreen:windowPoint];

    // Find the widget that *should* get the event (e.g., maybe it was a pop-up,
    // they always get the mouse event).
    QWidget *qwidget = [theView qt_qwidget];
    QWidget *widgetToGetMouse = qwidget;
    QWidget *popup = qAppInstance()->activePopupWidget();
    NSView *tmpView = theView;
    if (mac_mouse_grabber  && mac_mouse_grabber != widgetToGetMouse) {
        widgetToGetMouse = mac_mouse_grabber;
        tmpView = qt_mac_nativeview_for(widgetToGetMouse);
    }

    if (popup && popup != qwidget->window()) {
        widgetToGetMouse = popup;
        tmpView = qt_mac_nativeview_for(popup);
        windowPoint = [[tmpView window] convertScreenToBase:globalPoint];

        QPoint qWindowPoint(windowPoint.x, windowPoint.y);
        if (widgetToGetMouse->rect().contains(qWindowPoint)) {
            // Keeping the mouse pressed on a combobox button will make
            // the popup pop in front of the mouse. But all mouse events
            // will be sendt to the button. Since we want mouse events
            // to be sendt to widgets inside the popup, we search for the
            // widget in front of the mouse:
            tmpView = [tmpView hitTest:windowPoint];
            if (!tmpView)
                return false;
            widgetToGetMouse =
                [static_cast<QT_MANGLE_NAMESPACE(QCocoaView) *>(tmpView) qt_qwidget];
        }
    }

    NSPoint localPoint = [tmpView convertPoint:windowPoint fromView:nil];
    QPoint qlocalPoint(localPoint.x, localPoint.y);

    EventRef carbonEvent = static_cast<EventRef>(const_cast<void *>([theEvent eventRef]));
    if (qt_mac_sendMacEventToWidget(widgetToGetMouse, carbonEvent))
        return true;

    // Yay! All the special cases are handled, it really is just a normal mouse event.
    Qt::KeyboardModifiers keyMods = qt_cocoaModifiers2QtModifiers([theEvent modifierFlags]);
    NSInteger clickCount = [theEvent clickCount];
    Qt::MouseButtons buttons = 0;
    {
        UInt32 mac_buttons;
        if (GetEventParameter(carbonEvent, kEventParamMouseChord, typeUInt32, 0,
                              sizeof(mac_buttons), 0, &mac_buttons) == noErr)
            buttons = qt_mac_get_buttons(mac_buttons);
    }
    switch (eventType) {
    default:
        qWarning("not handled! %d", eventType);
        break;
    case QEvent::MouseMove:
        break;
    case QEvent::MouseButtonPress:
        [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent]->view = theView;
        [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent]->theEvent = theEvent;
#ifndef QT_NAMESPACE
        Q_ASSERT(clickCount > 0);
#endif
        if (clickCount % 2 == 0)
            eventType = QEvent::MouseButtonDblClick;
        if (button == Qt::LeftButton && (keyMods & Qt::MetaModifier)) {
            button = Qt::RightButton;
            [theView qt_setLeftButtonIsRightButton: true];
        }
        break;
    case QEvent::MouseButtonRelease:
        if (button == Qt::LeftButton && [theView qt_leftButtonIsRightButton]) {
            button = Qt::RightButton;
            [theView qt_setLeftButtonIsRightButton: false];
        }
        break;
    }
    [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent]->localPoint = localPoint;
    QPoint qglobalPoint(flipPoint(globalPoint).toPoint());
    QMouseEvent qme(eventType, qlocalPoint, qglobalPoint, button, buttons, keyMods);
    qt_sendSpontaneousEvent(widgetToGetMouse, &qme);
    if (eventType == QEvent::MouseButtonPress && button == Qt::RightButton) {
        QContextMenuEvent qcme(QContextMenuEvent::Mouse, qlocalPoint, qglobalPoint, keyMods);
        qt_sendSpontaneousEvent(widgetToGetMouse, &qcme);
    }
    return qme.isAccepted();
#endif
}

bool qt_mac_handleTabletEvent(void * /*QCocoaView * */view, void * /*NSEvent * */tabletEvent)
{
#ifndef QT_MAC_USE_COCOA
    Q_UNUSED(view);
    Q_UNUSED(tabletEvent);
    return false;
#else
    QT_MANGLE_NAMESPACE(QCocoaView) *theView = static_cast<QT_MANGLE_NAMESPACE(QCocoaView) *>(view);
    NSView *theNSView = static_cast<NSView *>(view);
    NSEvent *theTabletEvent = static_cast<NSEvent *>(tabletEvent);

    NSEventType eventType = [theTabletEvent type];
    if (eventType != NSTabletPoint && [theTabletEvent subtype] != NSTabletPointEventSubtype)
        return false; // Not a tablet event.

    NSPoint windowPoint = [theTabletEvent locationInWindow];
    NSPoint globalPoint = [[theTabletEvent window] convertBaseToScreen:windowPoint];

    QWidget *qwidget = [theView qt_qwidget];
    QWidget *widgetToGetMouse = qwidget;
    QWidget *popup = qAppInstance()->activePopupWidget();
    if (popup && popup != qwidget->window())
        widgetToGetMouse = popup;

    if (qt_mac_sendMacEventToWidget(widgetToGetMouse,
                                static_cast<EventRef>(const_cast<void *>([theTabletEvent eventRef]))))
        return true;
    if (widgetToGetMouse != qwidget) {
        theNSView = qt_mac_nativeview_for(widgetToGetMouse);
        windowPoint = [[theNSView window] convertScreenToBase:globalPoint];
    }
    NSPoint localPoint = [theNSView convertPoint:windowPoint fromView:nil];
    // Tablet events do not handle WA_TransparentForMouseEvents ATM
    // In theory, people who set the WA_TransparentForMouseEvents attribute won't handle
    // tablet events either in which case they will fall into the mouse event case and get
    // them passed on. This will NOT handle the raw events, but that might not be a big problem.

    const QMacTabletHash *tabletHash = qt_mac_tablet_hash();
    if (!tabletHash->contains([theTabletEvent deviceID])) {
        qWarning("QCocoaView handleTabletEvent: This tablet device is unknown"
                 " (received no proximity event for it). Discarding event.");
        return false;
    }
    const QTabletDeviceData &deviceData = tabletHash->value([theTabletEvent deviceID]);


    QEvent::Type qType;
    switch (eventType) {
    case NSLeftMouseDown:
    case NSRightMouseDown:
        qType = QEvent::TabletPress;
        break;
    case NSLeftMouseUp:
    case NSRightMouseUp:
        qType = QEvent::TabletRelease;
        break;
    case NSMouseMoved:
    case NSTabletPoint:
    case NSLeftMouseDragged:
    case NSRightMouseDragged:
    default:
        qType = QEvent::TabletMove;
        break;
    }

    qreal pressure;
    if (eventType != NSMouseMoved) {
        pressure = [theTabletEvent pressure];
    } else {
        pressure = 0.0;
    }

    NSPoint tilt = [theTabletEvent tilt];
    int xTilt = qRound(tilt.x * 60.0);
    int yTilt = qRound(tilt.y * -60.0);
    qreal tangentialPressure = 0;
    qreal rotation = 0;
    int z = 0;
    if (deviceData.capabilityMask & 0x0200)
        z = [theTabletEvent absoluteZ];

    if (deviceData.capabilityMask & 0x0800)
        tangentialPressure = [theTabletEvent tangentialPressure];

    rotation = [theTabletEvent rotation];
    QPointF hiRes = flipPoint(globalPoint);
    QTabletEvent qtabletEvent(qType, QPoint(localPoint.x, localPoint.y),
                              hiRes.toPoint(), hiRes,
                              deviceData.tabletDeviceType, deviceData.tabletPointerType,
                              pressure, xTilt, yTilt, tangentialPressure, rotation, z,
                              qt_cocoaModifiers2QtModifiers([theTabletEvent modifierFlags]),
                              deviceData.tabletUniqueID);

    qt_sendSpontaneousEvent(widgetToGetMouse, &qtabletEvent);
    return qtabletEvent.isAccepted();
#endif
}

void qt_mac_updateContentBorderMetricts(void * /*OSWindowRef */window, const ::HIContentBorderMetrics &metrics)
{
    OSWindowRef theWindow = static_cast<OSWindowRef>(window);
#if !defined(QT_MAC_USE_COCOA)
#  if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_5) {
        ::HIWindowSetContentBorderThickness(theWindow, &metrics);
    }
#  else
    Q_UNUSED(window);
    Q_UNUSED(metrics);
#  endif
#else
    if ([theWindow styleMask] & NSTexturedBackgroundWindowMask)
        [theWindow setContentBorderThickness:metrics.top forEdge:NSMaxYEdge];
    [theWindow setContentBorderThickness:metrics.bottom forEdge:NSMinYEdge];
#endif
}

void qt_mac_showBaseLineSeparator(void * /*OSWindowRef */window, bool show)
{
#if QT_MAC_USE_COCOA
    QMacCocoaAutoReleasePool pool;
    OSWindowRef theWindow = static_cast<OSWindowRef>(window);
    NSToolbar *macToolbar = [theWindow toolbar];
    if (macToolbar)
        [macToolbar setShowsBaselineSeparator: show];
#endif
}

QStringList qt_mac_NSArrayToQStringList(void *nsarray)
{
    QStringList result;
    NSArray *array = static_cast<NSArray *>(nsarray);
    for (NSUInteger i=0; i<[array count]; ++i)
        result << qt_mac_NSStringToQString([array objectAtIndex:i]);
    return result;
}

void *qt_mac_QStringListToNSMutableArrayVoid(const QStringList &list)
{
    NSMutableArray *result = [NSMutableArray arrayWithCapacity:list.size()];
    for (int i=0; i<list.size(); ++i){
        [result addObject:reinterpret_cast<const NSString *>(QCFString::toCFStringRef(list[i]))];
    }
    return result;
}

void qt_syncCocoaTitleBarButtons(OSWindowRef window, QWidget *widgetForWindow)
{
    if (!widgetForWindow)
        return;

    Qt::WindowFlags flags = widgetForWindow->windowFlags();
    bool customize = flags & Qt::CustomizeWindowHint;

    NSButton *btn = [window standardWindowButton:NSWindowZoomButton];
    // BOOL is not an int, so the bitwise AND doesn't work.
    bool go = uint(customize && !(flags & Qt::WindowMaximizeButtonHint)) == 0;
    [btn setEnabled:go];

    btn = [window standardWindowButton:NSWindowMiniaturizeButton];
    go = uint(customize && !(flags & Qt::WindowMinimizeButtonHint)) == 0;
    [btn setEnabled:go];

    btn = [window standardWindowButton:NSWindowCloseButton];
    go = uint(customize && !(flags & Qt::WindowSystemMenuHint
                             || flags & Qt::WindowCloseButtonHint)) == 0;
    [btn setEnabled:go];

    [window setShowsToolbarButton:uint(flags & Qt::MacWindowToolBarButtonHint) != 0];
}

// Carbon: Make sure you call QDEndContext on the context when done with it.
CGContextRef qt_mac_graphicsContextFor(QWidget *widget)
{
    if (!widget)
        return 0;

#ifndef QT_MAC_USE_COCOA
    CGContextRef context;
    CGrafPtr port = GetWindowPort(qt_mac_window_for(widget));
    QDBeginCGContext(port, &context);
#else
    CGContextRef context = (CGContextRef)[[NSGraphicsContext graphicsContextWithWindow:qt_mac_window_for(widget)] graphicsPort];
#endif
    return context;
}

CGFloat qt_mac_get_scalefactor()
{
#ifndef QT_MAC_USE_COCOA
    return HIGetScaleFactor();
#else
    return [[NSScreen mainScreen] userSpaceScaleFactor];
#endif
}

QString qt_mac_get_pasteboardString(OSPasteboardRef paste)
{
    QMacCocoaAutoReleasePool pool;
    NSPasteboard *pb = nil;
    CFStringRef pbname;
    if (PasteboardCopyName (paste, &pbname)) {
        pb = [NSPasteboard generalPasteboard];
    } else {
        pb = [NSPasteboard pasteboardWithName:reinterpret_cast<const NSString *>(pbname)];
        CFRelease (pbname);
    }
    if (pb) {
        NSString *text = [pb stringForType:NSStringPboardType];
        if (text)
            return qt_mac_NSStringToQString(text);
    }
    return QString();
}

QPixmap qt_mac_convert_iconref(const IconRef icon, int width, int height)
{
    QPixmap ret(width, height);
    ret.fill(QColor(0, 0, 0, 0));

    CGRect rect = CGRectMake(0, 0, width, height);

    CGContextRef ctx = qt_mac_cg_context(&ret);
    CGAffineTransform old_xform = CGContextGetCTM(ctx);
    CGContextConcatCTM(ctx, CGAffineTransformInvert(old_xform));
    CGContextConcatCTM(ctx, CGAffineTransformIdentity);

    ::RGBColor b;
    b.blue = b.green = b.red = 255*255;
    PlotIconRefInContext(ctx, &rect, kAlignNone, kTransformNone, &b, kPlotIconRefNormalFlags, icon);
    CGContextRelease(ctx);
    return ret;
}

void qt_mac_constructQIconFromIconRef(const IconRef icon, const IconRef overlayIcon, QIcon *retIcon, QStyle::StandardPixmap standardIcon)
{
    int size = 16;
    while (size <= 128) {

        const QString cacheKey = QLatin1String("qt_mac_constructQIconFromIconRef") + QString::number(standardIcon) + QString::number(size);
        QPixmap mainIcon;
        if (standardIcon >= QStyle::SP_CustomBase) {
            mainIcon = qt_mac_convert_iconref(icon, size, size);
        } else if (QPixmapCache::find(cacheKey, mainIcon) == false) {
            mainIcon = qt_mac_convert_iconref(icon, size, size);
            QPixmapCache::insert(cacheKey, mainIcon);
        }

        if (overlayIcon) {
            int littleSize = size / 2;
            QPixmap overlayPix = qt_mac_convert_iconref(overlayIcon, littleSize, littleSize);
            QPainter painter(&mainIcon);
            painter.drawPixmap(size - littleSize, size - littleSize, overlayPix);
        }

        retIcon->addPixmap(mainIcon);
        size += size;  // 16 -> 32 -> 64 -> 128
    }
}

void qt_mac_menu_collapseSeparators(void */*NSMenu **/ theMenu, bool collapse)
{
    OSMenuRef menu = static_cast<OSMenuRef>(theMenu);
    if (collapse) {
        bool previousIsSeparator = true; // setting to true kills all the separators placed at the top.
        NSMenuItem *previousItem = nil;
            
        NSArray *itemArray = [menu itemArray];
        for (unsigned int i = 0; i < [itemArray count]; ++i) {
            NSMenuItem *item = reinterpret_cast<NSMenuItem *>([itemArray objectAtIndex:i]);
            if ([item isSeparatorItem]) {
                [item setHidden:previousIsSeparator];
            }

            if (![item isHidden]) {
                previousItem = item;
                previousIsSeparator = ([previousItem isSeparatorItem]);
            }
        }

        // We now need to check the final item since we don't want any separators at the end of the list.
        if (previousItem && previousIsSeparator)
            [previousItem setHidden:YES];
    } else {
        NSArray *itemArray = [menu itemArray];
        for (unsigned int i = 0; i < [itemArray count]; ++i) {
            NSMenuItem *item = reinterpret_cast<NSMenuItem *>([itemArray objectAtIndex:i]);
            if (QAction *action = reinterpret_cast<QAction *>([item tag]))
                [item setHidden:!action->isVisible()];
        }
    }
}

#ifdef QT_MAC_USE_COCOA
void qt_cocoaChangeOverrideCursor(const QCursor &cursor)
{
    QMacCocoaAutoReleasePool pool;
    [static_cast<NSCursor *>(qt_mac_nsCursorForQCursor(cursor)) set];
}
#endif

QT_END_NAMESPACE
