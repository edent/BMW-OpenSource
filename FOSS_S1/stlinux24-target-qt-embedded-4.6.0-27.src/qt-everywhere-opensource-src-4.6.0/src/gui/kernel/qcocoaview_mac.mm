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

#import <private/qcocoaview_mac_p.h>
#ifdef QT_MAC_USE_COCOA

#include <private/qwidget_p.h>
#include <private/qt_mac_p.h>
#include <private/qapplication_p.h>
#include <private/qabstractscrollarea_p.h>
#include <private/qt_cocoa_helpers_mac_p.h>
#include <private/qdnd_p.h>
#include <private/qmacinputcontext_p.h>
#include <private/qmultitouch_mac_p.h>
#include <private/qevent_p.h>
#include <private/qbackingstore_p.h>

#include <qscrollarea.h>
#include <qhash.h>
#include <qtextformat.h>
#include <qpaintengine.h>
#include <QUrl>
#include <QAccessible>
#include <QFileInfo>
#include <QFile>

#include <qdebug.h>

@interface NSEvent (DeviceDelta)
  - (CGFloat)deviceDeltaX;
  - (CGFloat)deviceDeltaY;
  - (CGFloat)deviceDeltaZ;
@end

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(DnDParams, qMacDnDParams);

extern void qt_mac_update_cursor_at_global_pos(const QPoint &globalPos); // qcursor_mac.mm
extern bool qt_sendSpontaneousEvent(QObject *, QEvent *); // qapplication.cpp
extern OSViewRef qt_mac_nativeview_for(const QWidget *w); // qwidget_mac.mm
extern const QStringList& qEnabledDraggedTypes(); // qmime_mac.cpp
extern QPointer<QWidget> qt_mouseover; //qapplication_mac.mm

Qt::MouseButton cocoaButton2QtButton(NSInteger buttonNum)
{
    if (buttonNum == 0)
        return Qt::LeftButton;
    if (buttonNum == 1)
        return Qt::RightButton;
    if (buttonNum == 2)
        return Qt::MidButton;
    if (buttonNum == 3)
        return Qt::XButton1;
    if (buttonNum == 4)
        return Qt::XButton2;
    return Qt::NoButton;
}

struct dndenum_mapper
{
    NSDragOperation mac_code;
    Qt::DropAction qt_code;
    bool Qt2Mac;
};

static dndenum_mapper dnd_enums[] = {
    { NSDragOperationLink,  Qt::LinkAction, true },
    { NSDragOperationMove,  Qt::MoveAction, true },
    { NSDragOperationCopy,  Qt::CopyAction, true },
    { NSDragOperationGeneric,  Qt::CopyAction, false },
    { NSDragOperationEvery, Qt::ActionMask, false },
    { NSDragOperationNone, Qt::IgnoreAction, false }
};

static NSDragOperation qt_mac_mapDropAction(Qt::DropAction action)
{
    for (int i=0; dnd_enums[i].qt_code; i++) {
        if (dnd_enums[i].Qt2Mac && (action & dnd_enums[i].qt_code)) {
            return dnd_enums[i].mac_code;
        }
    }
    return NSDragOperationNone;
}

static NSDragOperation qt_mac_mapDropActions(Qt::DropActions actions)
{
    NSDragOperation nsActions = NSDragOperationNone;
    for (int i=0; dnd_enums[i].qt_code; i++) {
        if (dnd_enums[i].Qt2Mac && (actions & dnd_enums[i].qt_code))
            nsActions |= dnd_enums[i].mac_code;
    }
    return nsActions;
}

static Qt::DropAction qt_mac_mapNSDragOperation(NSDragOperation nsActions)
{
    Qt::DropAction action = Qt::IgnoreAction;
    for (int i=0; dnd_enums[i].mac_code; i++) {
        if (nsActions & dnd_enums[i].mac_code)
            return dnd_enums[i].qt_code;
    }
    return action;
}

static Qt::DropActions qt_mac_mapNSDragOperations(NSDragOperation nsActions)
{
    Qt::DropActions actions = Qt::IgnoreAction;
    for (int i=0; dnd_enums[i].mac_code; i++) {
        if (nsActions & dnd_enums[i].mac_code)
            actions |= dnd_enums[i].qt_code;
    }
    return actions;
}

static QColor colorFrom(NSColor *color)
{
    QColor qtColor;
    NSString *colorSpace = [color colorSpaceName];
    if (colorSpace == NSDeviceCMYKColorSpace) {
        CGFloat cyan, magenta, yellow, black, alpha;
        [color getCyan:&cyan magenta:&magenta yellow:&yellow black:&black alpha:&alpha];
        qtColor.setCmykF(cyan, magenta, yellow, black, alpha);
    } else {
        NSColor *tmpColor;
        tmpColor = [color colorUsingColorSpaceName:NSDeviceRGBColorSpace];
        CGFloat red, green, blue, alpha;
        [tmpColor getRed:&red green:&green blue:&blue alpha:&alpha];
        qtColor.setRgbF(red, green, blue, alpha);
    }
    return qtColor;
}

QT_END_NAMESPACE

QT_FORWARD_DECLARE_CLASS(QMacCocoaAutoReleasePool)
QT_FORWARD_DECLARE_CLASS(QCFString)
QT_FORWARD_DECLARE_CLASS(QDragManager)
QT_FORWARD_DECLARE_CLASS(QMimeData)
QT_FORWARD_DECLARE_CLASS(QPoint)
QT_FORWARD_DECLARE_CLASS(QApplication)
QT_FORWARD_DECLARE_CLASS(QApplicationPrivate)
QT_FORWARD_DECLARE_CLASS(QDragEnterEvent)
QT_FORWARD_DECLARE_CLASS(QDragMoveEvent)
QT_FORWARD_DECLARE_CLASS(QStringList)
QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QRect)
QT_FORWARD_DECLARE_CLASS(QRegion)
QT_FORWARD_DECLARE_CLASS(QAbstractScrollArea)
QT_FORWARD_DECLARE_CLASS(QAbstractScrollAreaPrivate)
QT_FORWARD_DECLARE_CLASS(QPaintEvent)
QT_FORWARD_DECLARE_CLASS(QPainter)
QT_FORWARD_DECLARE_CLASS(QHoverEvent)
QT_FORWARD_DECLARE_CLASS(QCursor)
QT_USE_NAMESPACE
extern "C" {
    extern NSString *NSTextInputReplacementRangeAttributeName;
}


@implementation QT_MANGLE_NAMESPACE(QCocoaView)

- (id)initWithQWidget:(QWidget *)widget widgetPrivate:(QWidgetPrivate *)widgetprivate
{
    self = [super init];
    if (self) {
        [self finishInitWithQWidget:widget widgetPrivate:widgetprivate];
    }
    composingText = new QString();
    composing = false;
    sendKeyEvents = true;
    currentCustomTypes = 0;
    [self setHidden:YES];
    return self;
}

- (void) finishInitWithQWidget:(QWidget *)widget widgetPrivate:(QWidgetPrivate *)widgetprivate
{
    qwidget = widget;
    qwidgetprivate = widgetprivate;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(frameDidChange:)
                                                 name:@"NSViewFrameDidChangeNotification"
                                               object:self];
}

-(void)registerDragTypes
{
    QMacCocoaAutoReleasePool pool;
    // Calling registerForDraggedTypes is slow, so only do it once for each widget
    // or when the custom types change.
    const QStringList& customTypes = qEnabledDraggedTypes();
    if (currentCustomTypes == 0 || *currentCustomTypes != customTypes) {
        if (currentCustomTypes == 0)
            currentCustomTypes = new QStringList();
        *currentCustomTypes = customTypes;
        const NSString* mimeTypeGeneric = @"com.trolltech.qt.MimeTypeName";
	NSMutableArray *supportedTypes = [NSMutableArray arrayWithObjects:NSColorPboardType,
                                   NSFilenamesPboardType, NSStringPboardType,
                                   NSFilenamesPboardType, NSPostScriptPboardType, NSTIFFPboardType,
                                   NSRTFPboardType, NSTabularTextPboardType, NSFontPboardType,
                                   NSRulerPboardType, NSFileContentsPboardType, NSColorPboardType,
                                   NSRTFDPboardType, NSHTMLPboardType, NSPICTPboardType,
                                   NSURLPboardType, NSPDFPboardType, NSVCardPboardType,
                                   NSFilesPromisePboardType, NSInkTextPboardType,
                                   NSMultipleTextSelectionPboardType, mimeTypeGeneric, nil];
        // Add custom types supported by the application.
        for (int i = 0; i < customTypes.size(); i++) {
           [supportedTypes addObject:reinterpret_cast<const NSString *>(QCFString::toCFStringRef(customTypes[i]))];
        }
        [self registerForDraggedTypes:supportedTypes];
    }
}

- (void)resetCursorRects
{
    QWidget *cursorWidget = qwidget;

    if (cursorWidget->testAttribute(Qt::WA_TransparentForMouseEvents))
        cursorWidget = QApplication::widgetAt(qwidget->mapToGlobal(qwidget->rect().center()));

    if (cursorWidget == 0)
        return;

    if (!cursorWidget->testAttribute(Qt::WA_SetCursor)) {
        [super resetCursorRects];
        return;
    }

    QRegion mask = qt_widget_private(cursorWidget)->extra->mask;
    NSCursor *nscursor = static_cast<NSCursor *>(qt_mac_nsCursorForQCursor(cursorWidget->cursor()));
    if (mask.isEmpty()) {
        [self addCursorRect:[qt_mac_nativeview_for(cursorWidget) visibleRect] cursor:nscursor];
    } else {
        const QVector<QRect> &rects = mask.rects();
        for (int i = 0; i < rects.size(); ++i) {
            const QRect &rect = rects.at(i);
            [self addCursorRect:NSMakeRect(rect.x(), rect.y(), rect.width(), rect.height()) cursor:nscursor];
        }
    }
}

- (void)removeDropData
{
    if (dropData) {
        delete dropData;
        dropData = 0;
    }
}

- (void)addDropData:(id <NSDraggingInfo>)sender
{
    [self removeDropData];
    CFStringRef dropPasteboard = (CFStringRef) [[sender draggingPasteboard] name];
    dropData = new QCocoaDropData(dropPasteboard);
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    if (qwidget->testAttribute(Qt::WA_DropSiteRegistered) == false)
        return NSDragOperationNone;
    NSPoint windowPoint = [sender draggingLocation];
    if (qwidget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
        // pass the drag enter event to the view underneath.
        NSView *candidateView = [[[self window] contentView] hitTest:windowPoint];
        if (candidateView && candidateView != self)
            return [candidateView draggingEntered:sender];
    }
    dragEnterSequence = [sender draggingSequenceNumber];
    [self addDropData:sender];
    QMimeData *mimeData = dropData;
    if (QDragManager::self()->source())
        mimeData = QDragManager::self()->dragPrivate()->data;
    NSPoint globalPoint = [[sender draggingDestinationWindow] convertBaseToScreen:windowPoint];
    NSPoint localPoint = [self convertPoint:windowPoint fromView:nil];
    QPoint posDrag(localPoint.x, localPoint.y);
    NSDragOperation nsActions = [sender draggingSourceOperationMask];
    Qt::DropActions qtAllowed = qt_mac_mapNSDragOperations(nsActions);
    QT_PREPEND_NAMESPACE(qt_mac_dnd_answer_rec.lastOperation) = nsActions;
    Qt::KeyboardModifiers modifiers  = Qt::NoModifier;
    if ([sender draggingSource] != nil) {
        // modifier flags might have changed, update it here since we don't send any input events.
        QApplicationPrivate::modifier_buttons = qt_cocoaModifiers2QtModifiers([[NSApp currentEvent] modifierFlags]);
        modifiers = QApplication::keyboardModifiers();
    } else {
        // when the source is from another application the above technique will not work.
        modifiers = qt_cocoaDragOperation2QtModifiers(nsActions);
    }
    // send the drag enter event to the widget.
    QDragEnterEvent qDEEvent(posDrag, qtAllowed, mimeData, QApplication::mouseButtons(), modifiers);
    QApplication::sendEvent(qwidget, &qDEEvent);
    if (!qDEEvent.isAccepted()) {
        // widget is not interested in this drag, so ignore this drop data.
        [self removeDropData];
        return NSDragOperationNone;
    } else {
        // save the mouse position, used by draggingExited handler.
        DnDParams *dndParams = [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent];
        dndParams->activeDragEnterPos = windowPoint;
        // send a drag move event immediately after a drag enter event (as per documentation).
        QDragMoveEvent qDMEvent(posDrag, qtAllowed, mimeData, QApplication::mouseButtons(), modifiers);
        qDMEvent.setDropAction(qDEEvent.dropAction());
        qDMEvent.accept(); // accept by default, since enter event was accepted.
        QApplication::sendEvent(qwidget, &qDMEvent);
        if (!qDMEvent.isAccepted() || qDMEvent.dropAction() == Qt::IgnoreAction) {
            // since we accepted the drag enter event, the widget expects
            // future drage move events.
            // ### check if we need to treat this like the drag enter event.
            nsActions = QT_PREPEND_NAMESPACE(qt_mac_mapDropAction)(qDEEvent.dropAction());
        } else {
            nsActions = QT_PREPEND_NAMESPACE(qt_mac_mapDropAction)(qDMEvent.dropAction());
        }
        QT_PREPEND_NAMESPACE(qt_mac_copy_answer_rect)(qDMEvent);
        return nsActions;
    }
 }

- (NSDragOperation)draggingUpdated:(id < NSDraggingInfo >)sender
{
    NSPoint windowPoint = [sender draggingLocation];
    if (qwidget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
        // pass the drag move event to the view underneath.
        NSView *candidateView = [[[self window] contentView] hitTest:windowPoint];
        if (candidateView && candidateView != self)
            return [candidateView draggingUpdated:sender];
    }
    // in cases like QFocusFrame, the view under the mouse might
    // not have received the drag enter. Generate a synthetic
    // drag enter event for that view.
    if (dragEnterSequence != [sender draggingSequenceNumber])
        [self draggingEntered:sender];
    // drag enter event was rejected, so ignore the move event.
    if (dropData == 0)
        return NSDragOperationNone;
    // return last value, if we are still in the answerRect.
    NSPoint globalPoint = [[sender draggingDestinationWindow] convertBaseToScreen:windowPoint];
    NSPoint localPoint = [self convertPoint:windowPoint fromView:nil];
    NSDragOperation nsActions = [sender draggingSourceOperationMask];
    QPoint posDrag(localPoint.x, localPoint.y);
    if (qt_mac_mouse_inside_answer_rect(posDrag)
        && QT_PREPEND_NAMESPACE(qt_mac_dnd_answer_rec.lastOperation) == nsActions)
        return QT_PREPEND_NAMESPACE(qt_mac_mapDropActions)(QT_PREPEND_NAMESPACE(qt_mac_dnd_answer_rec.lastAction));
    // send drag move event to the widget
    QT_PREPEND_NAMESPACE(qt_mac_dnd_answer_rec.lastOperation) = nsActions;
    Qt::DropActions qtAllowed = QT_PREPEND_NAMESPACE(qt_mac_mapNSDragOperations)(nsActions);
    Qt::KeyboardModifiers modifiers  = Qt::NoModifier;
    if ([sender draggingSource] != nil) {
        // modifier flags might have changed, update it here since we don't send any input events.
        QApplicationPrivate::modifier_buttons = qt_cocoaModifiers2QtModifiers([[NSApp currentEvent] modifierFlags]);
        modifiers = QApplication::keyboardModifiers();
    } else {
        // when the source is from another application the above technique will not work.
        modifiers = qt_cocoaDragOperation2QtModifiers(nsActions);
    }
    QMimeData *mimeData = dropData;
    if (QDragManager::self()->source())
        mimeData = QDragManager::self()->dragPrivate()->data;
    QDragMoveEvent qDMEvent(posDrag, qtAllowed, mimeData, QApplication::mouseButtons(), modifiers);
    qDMEvent.setDropAction(QT_PREPEND_NAMESPACE(qt_mac_dnd_answer_rec).lastAction);
    qDMEvent.accept();
    QApplication::sendEvent(qwidget, &qDMEvent);
    qt_mac_copy_answer_rect(qDMEvent);

    NSDragOperation operation = qt_mac_mapDropAction(qDMEvent.dropAction());
    if (!qDMEvent.isAccepted() || qDMEvent.dropAction() == Qt::IgnoreAction) {
        // ignore this event (we will still receive further notifications)
        operation = NSDragOperationNone;
    }
    return operation;
}

- (void)draggingExited:(id < NSDraggingInfo >)sender
{
    dragEnterSequence = -1;
    if (qwidget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
        // try sending the leave event to the last view which accepted drag enter.
        DnDParams *dndParams = [QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent];
        NSView *candidateView = [[[self window] contentView] hitTest:dndParams->activeDragEnterPos];
        if (candidateView && candidateView != self)
            return [candidateView draggingExited:sender];
    }
    // drag enter event was rejected, so ignore the move event.
    if (dropData) {
        QDragLeaveEvent de;
        QApplication::sendEvent(qwidget, &de);
        [self removeDropData];
    }
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPoint windowPoint = [sender draggingLocation];
    dragEnterSequence = -1;
    if (qwidget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
        // pass the drop event to the view underneath.
        NSView *candidateView = [[[self window] contentView] hitTest:windowPoint];
        if (candidateView && candidateView != self)
            return [candidateView performDragOperation:sender];
    }
    [self addDropData:sender];

    NSPoint globalPoint = [[sender draggingDestinationWindow] convertBaseToScreen:windowPoint];
    NSPoint localPoint = [self convertPoint:windowPoint fromView:nil];
    QPoint posDrop(localPoint.x, localPoint.y);

    NSDragOperation nsActions = [sender draggingSourceOperationMask];
    Qt::DropActions qtAllowed = qt_mac_mapNSDragOperations(nsActions);
    QMimeData *mimeData = dropData;
    if (QDragManager::self()->source())
        mimeData = QDragManager::self()->dragPrivate()->data;
    // send the drop event to the widget.
    QDropEvent de(posDrop, qtAllowed, mimeData,
                  QApplication::mouseButtons(), QApplication::keyboardModifiers());
    if (QDragManager::self()->object)
        QDragManager::self()->dragPrivate()->target = qwidget;
    QApplication::sendEvent(qwidget, &de);
    if (QDragManager::self()->object)
        QDragManager::self()->dragPrivate()->executed_action = de.dropAction();
    if (!de.isAccepted())
        return NO;
    else
        return YES;
}

- (void)dealloc
{
    delete composingText;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    delete currentCustomTypes;
    [self unregisterDraggedTypes];
    [super dealloc];
}

- (BOOL)isOpaque;
{
    return qwidgetprivate->isOpaque;
}

- (BOOL)isFlipped;
{
    return YES;
}

- (BOOL) preservesContentDuringLiveResize;
{
    return qwidget->testAttribute(Qt::WA_StaticContents);
}

- (void) setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    // A change in size has required the view to be invalidated.
    if ([self inLiveResize]) {
        NSRect rects[4];
        NSInteger count;
        [self getRectsExposedDuringLiveResize:rects count:&count];
        while (count-- > 0)
        {
            [self setNeedsDisplayInRect:rects[count]];
        }
    } else {
        [self setNeedsDisplay:YES];
    }
}

- (void)drawRect:(NSRect)aRect
{
    if (QApplicationPrivate::graphicsSystem() != 0) {
        if (QWidgetBackingStore *bs = qwidgetprivate->maybeBackingStore())
            bs->markDirty(qwidget->rect(), qwidget);
        qwidgetprivate->syncBackingStore(qwidget->rect());
        return;
    }
    CGContextRef cg = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    qwidgetprivate->hd = cg;
    CGContextSaveGState(cg);

    if (qwidget->isVisible() && qwidget->updatesEnabled()) { //process the actual paint event.
        if (qwidget->testAttribute(Qt::WA_WState_InPaintEvent))
            qWarning("QWidget::repaint: Recursive repaint detected");

        const QRect qrect = QRect(aRect.origin.x, aRect.origin.y, aRect.size.width, aRect.size.height);
        QRegion qrgn(qrect);

        if (!qwidget->isWindow() && !qobject_cast<QAbstractScrollArea *>(qwidget->parent())) {
            const QRegion &parentMask = qwidget->window()->mask();
            if (!parentMask.isEmpty()) {
                const QPoint mappedPoint = qwidget->mapTo(qwidget->window(), qrect.topLeft());
                qrgn.translate(mappedPoint);
                qrgn &= parentMask;
                qrgn.translate(-mappedPoint.x(), -mappedPoint.y());
            }
        }

        QPoint redirectionOffset(0, 0);
        //setup the context
        qwidget->setAttribute(Qt::WA_WState_InPaintEvent);
        QPaintEngine *engine = qwidget->paintEngine();
        if (engine)
            engine->setSystemClip(qrgn);
        if (qwidgetprivate->extra && qwidgetprivate->extra->hasMask) {
            CGRect widgetRect = CGRectMake(0, 0, qwidget->width(), qwidget->height());
            CGContextTranslateCTM (cg, 0, widgetRect.size.height);
            CGContextScaleCTM(cg, 1, -1);
            if (qwidget->isWindow())
                CGContextClearRect(cg, widgetRect);
            CGContextClipToMask(cg, widgetRect, qwidgetprivate->extra->imageMask);
            CGContextScaleCTM(cg, 1, -1);
            CGContextTranslateCTM (cg, 0, -widgetRect.size.height);
        }

        if (qwidget->isWindow() && !qwidgetprivate->isOpaque
                && !qwidget->testAttribute(Qt::WA_MacBrushedMetal)) {
            CGContextClearRect(cg, NSRectToCGRect(aRect));
        }

        if (engine && !qwidget->testAttribute(Qt::WA_NoSystemBackground)
            && (qwidget->isWindow() || qwidget->autoFillBackground())
                || qwidget->testAttribute(Qt::WA_TintedBackground)
                || qwidget->testAttribute(Qt::WA_StyledBackground)) {
#ifdef DEBUG_WIDGET_PAINT
            if(doDebug)
                qDebug(" Handling erase for [%s::%s]", qwidget->metaObject()->className(),
                       qwidget->objectName().local8Bit().data());
#endif
            QPainter p(qwidget);
            qwidgetprivate->paintBackground(&p, qrgn,
                                            qwidget->isWindow() ? QWidgetPrivate::DrawAsRoot : 0);
            p.end();
        }
        QPaintEvent e(qrgn);
#ifdef QT3_SUPPORT
        e.setErased(true);
#endif
        qt_sendSpontaneousEvent(qwidget, &e);
        if (!redirectionOffset.isNull())
            QPainter::restoreRedirected(qwidget);
        if (engine)
            engine->setSystemClip(QRegion());
        qwidget->setAttribute(Qt::WA_WState_InPaintEvent, false);
        if(!qwidget->testAttribute(Qt::WA_PaintOutsidePaintEvent) && qwidget->paintingActive())
            qWarning("QWidget: It is dangerous to leave painters active on a"
                     " widget outside of the PaintEvent");
    }
    qwidgetprivate->hd = 0;
    CGContextRestoreGState(cg);
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
    Q_UNUSED(theEvent);
    return !qwidget->testAttribute(Qt::WA_MacNoClickThrough);
}

- (NSView *)hitTest:(NSPoint)aPoint
{
    if (qwidget->testAttribute(Qt::WA_TransparentForMouseEvents))
        return nil; // You cannot hit a transparent for mouse event widget.
    return [super hitTest:aPoint];
}

- (void)updateTrackingAreas
{
    QMacCocoaAutoReleasePool pool;
    if (NSArray *trackingArray = [self trackingAreas]) {
        NSUInteger size = [trackingArray count];
        for (NSUInteger i = 0; i < size; ++i) {
            NSTrackingArea *t = [trackingArray objectAtIndex:i];
            [self removeTrackingArea:t];
        }
    }

    // Ideally, we shouldn't have NSTrackingMouseMoved events included below, it should
    // only be turned on if mouseTracking, hover is on or a tool tip is set.
    // Unfortunately, Qt will send "tooltip" events on mouse moves, so we need to
    // turn it on in ALL case. That means EVERY QCocoaView gets to pay the cost of
    // mouse moves delivered to it (Apple recommends keeping it OFF because there
    // is a performance hit). So it goes.
    NSUInteger trackingOptions = NSTrackingMouseEnteredAndExited | NSTrackingActiveInActiveApp
                                 | NSTrackingInVisibleRect | NSTrackingMouseMoved;
    NSTrackingArea *ta = [[NSTrackingArea alloc] initWithRect:NSMakeRect(0, 0,
                                                                         qwidget->width(),
                                                                         qwidget->height())
                                                      options:trackingOptions
                                                        owner:self
                                                     userInfo:nil];
    [self addTrackingArea:ta];
    [ta release];
}

- (void)mouseEntered:(NSEvent *)event
{
    QEvent enterEvent(QEvent::Enter);
    NSPoint windowPoint = [event locationInWindow];
    NSPoint globalPoint = [[event window] convertBaseToScreen:windowPoint];
    NSPoint viewPoint = [self convertPoint:windowPoint fromView:nil];
    if (!qAppInstance()->activeModalWidget() || QApplicationPrivate::tryModalHelper(qwidget, 0)) {
        QApplication::sendEvent(qwidget, &enterEvent);
        qt_mouseover = qwidget;

        // Update cursor and dispatch hover events.
        qt_mac_update_cursor_at_global_pos(flipPoint(globalPoint).toPoint());
        if (qwidget->testAttribute(Qt::WA_Hover) &&
            (!qAppInstance()->activePopupWidget() || qAppInstance()->activePopupWidget() == qwidget->window())) {
            QHoverEvent he(QEvent::HoverEnter, QPoint(viewPoint.x, viewPoint.y), QPoint(-1, -1));
            QApplicationPrivate::instance()->notify_helper(qwidget, &he);
        }
    }
}

- (void)mouseExited:(NSEvent *)event
{
    QEvent leaveEvent(QEvent::Leave);
    NSPoint globalPoint = [[event window] convertBaseToScreen:[event locationInWindow]];
    if (!qAppInstance()->activeModalWidget() || QApplicationPrivate::tryModalHelper(qwidget, 0)) {
        QApplication::sendEvent(qwidget, &leaveEvent);

        // ### Think about if it is necessary to update the cursor, should only be for a few cases.
        qt_mac_update_cursor_at_global_pos(flipPoint(globalPoint).toPoint());
        if (qwidget->testAttribute(Qt::WA_Hover)
            && (!qAppInstance()->activePopupWidget() || qAppInstance()->activePopupWidget() == qwidget->window())) {
            QHoverEvent he(QEvent::HoverLeave, QPoint(-1, -1),
                           qwidget->mapFromGlobal(QApplicationPrivate::instance()->hoverGlobalPos));
            QApplicationPrivate::instance()->notify_helper(qwidget, &he);
        }
    }
}

- (void)flagsChanged:(NSEvent *)theEvent
{
    QWidget *widgetToGetKey = qwidget;

    QWidget *popup = qAppInstance()->activePopupWidget();
    if (popup && popup != qwidget->window())
        widgetToGetKey = popup->focusWidget() ? popup->focusWidget() : popup;
    qt_dispatchModifiersChanged(theEvent, widgetToGetKey);
    [super flagsChanged:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    // We always enable mouse tracking for all QCocoaView-s. In cases where we have
    // child views, we will receive mouseMoved for both parent & the child (if
    // mouse is over the child). We need to ignore the parent mouseMoved in such
    // cases.
    NSPoint windowPoint = [theEvent locationInWindow];
    NSView *candidateView = [[[self window] contentView] hitTest:windowPoint];
    if (candidateView && candidateView == self) {
        qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseMove, Qt::NoButton);
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseButtonPress, Qt::LeftButton);
    // Don't call super here. This prevents us from getting the mouseUp event,
    // which we need to send even if the mouseDown event was not accepted.
    // (this is standard Qt behavior.)
}


- (void)mouseUp:(NSEvent *)theEvent
{
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseButtonRelease, Qt::LeftButton);

    if (!mouseOK)
        [super mouseUp:theEvent];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseButtonPress, Qt::RightButton);

    if (!mouseOK)
        [super rightMouseDown:theEvent];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseButtonRelease, Qt::RightButton);

    if (!mouseOK)
        [super rightMouseUp:theEvent];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    Qt::MouseButton mouseButton = cocoaButton2QtButton([theEvent buttonNumber]);
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseButtonPress, mouseButton);

    if (!mouseOK)
        [super otherMouseDown:theEvent];
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
    Qt::MouseButton mouseButton = cocoaButton2QtButton([theEvent buttonNumber]);
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent,  QEvent::MouseButtonRelease, mouseButton);

    if (!mouseOK)
        [super otherMouseUp:theEvent];

}

- (void)mouseDragged:(NSEvent *)theEvent
{
    qMacDnDParams()->view = self;
    qMacDnDParams()->theEvent = theEvent;
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseMove, Qt::NoButton);

    if (!mouseOK)
        [super mouseDragged:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    qMacDnDParams()->view = self;
    qMacDnDParams()->theEvent = theEvent;
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseMove, Qt::NoButton);

    if (!mouseOK)
        [super rightMouseDragged:theEvent];
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
    qMacDnDParams()->view = self;
    qMacDnDParams()->theEvent = theEvent;
    bool mouseOK = qt_mac_handleMouseEvent(self, theEvent, QEvent::MouseMove, Qt::NoButton);

    if (!mouseOK)
        [super otherMouseDragged:theEvent];
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    // Give the Input Manager a chance to process the wheel event.
    NSInputManager *currentIManager = [NSInputManager currentInputManager];
    if (currentIManager && [currentIManager wantsToHandleMouseEvents]) {
        [currentIManager handleMouseEvent:theEvent];
    }

    NSPoint windowPoint = [theEvent locationInWindow];
    NSPoint globalPoint = [[theEvent window] convertBaseToScreen:windowPoint];
    NSPoint localPoint = [self convertPoint:windowPoint fromView:nil];
    QPoint qlocal = QPoint(localPoint.x, localPoint.y);
    QPoint qglobal = QPoint(globalPoint.x, globalPoint.y);
    Qt::MouseButton buttons = cocoaButton2QtButton([theEvent buttonNumber]);
    bool wheelOK = false;
    Qt::KeyboardModifiers keyMods = qt_cocoaModifiers2QtModifiers([theEvent modifierFlags]);
    QWidget *widgetToGetMouse = qwidget;
    int deltaX = 0;
    int deltaY = 0;
    int deltaZ = 0;

    const EventRef carbonEvent = (EventRef)[theEvent eventRef];
    const UInt32 carbonEventKind = carbonEvent ? ::GetEventKind(carbonEvent) : 0;
    const bool scrollEvent = carbonEventKind == kEventMouseScroll;

    if (scrollEvent) {
        // The mouse device containts pixel scroll wheel support (Mighty Mouse, Trackpad).
        // Since deviceDelta is delivered as pixels rather than degrees, we need to
        // convert from pixels to degrees in a sensible manner.
        // It looks like four degrees per pixel behaves most native.
        // Qt expects the unit for delta to be 1/8 of a degree:
        deltaX = [theEvent deviceDeltaX];
        deltaY = [theEvent deviceDeltaY];
        deltaZ = [theEvent deviceDeltaZ];
    } else {
        // carbonEventKind == kEventMouseWheelMoved
        // Remove acceleration, and use either -120 or 120 as delta:
        deltaX = qBound(-120, int([theEvent deltaX] * 10000), 120);
        deltaY = qBound(-120, int([theEvent deltaY] * 10000), 120);
        deltaZ = qBound(-120, int([theEvent deltaZ] * 10000), 120);
    }

    if (deltaX != 0) {
        QWheelEvent qwe(qlocal, qglobal, deltaX, buttons, keyMods, Qt::Horizontal);
        qt_sendSpontaneousEvent(widgetToGetMouse, &qwe);
        wheelOK = qwe.isAccepted();
        if (!wheelOK && QApplicationPrivate::focus_widget
            && QApplicationPrivate::focus_widget != widgetToGetMouse) {
            QWheelEvent qwe2(QApplicationPrivate::focus_widget->mapFromGlobal(qglobal), qglobal,
                             deltaX, buttons, keyMods, Qt::Horizontal);
            qt_sendSpontaneousEvent(QApplicationPrivate::focus_widget, &qwe2);
            wheelOK = qwe2.isAccepted();
        }
    }

    if (deltaY) {
        QWheelEvent qwe(qlocal, qglobal, deltaY, buttons, keyMods, Qt::Vertical);
        qt_sendSpontaneousEvent(widgetToGetMouse, &qwe);
        wheelOK = qwe.isAccepted();
        if (!wheelOK && QApplicationPrivate::focus_widget
            && QApplicationPrivate::focus_widget != widgetToGetMouse) {
            QWheelEvent qwe2(QApplicationPrivate::focus_widget->mapFromGlobal(qglobal), qglobal,
                             deltaY, buttons, keyMods, Qt::Vertical);
            qt_sendSpontaneousEvent(QApplicationPrivate::focus_widget, &qwe2);
            wheelOK = qwe2.isAccepted();
        }
    }

    if (deltaZ) {
        // Qt doesn't explicitly support wheels with a Z component. In a misguided attempt to
        // try to be ahead of the pack, I'm adding this extra value.
        QWheelEvent qwe(qlocal, qglobal, deltaZ, buttons, keyMods, (Qt::Orientation)3);
        qt_sendSpontaneousEvent(widgetToGetMouse, &qwe);
        wheelOK = qwe.isAccepted();
        if (!wheelOK && QApplicationPrivate::focus_widget
            && QApplicationPrivate::focus_widget != widgetToGetMouse) {
            QWheelEvent qwe2(QApplicationPrivate::focus_widget->mapFromGlobal(qglobal), qglobal,
                             deltaZ, buttons, keyMods, (Qt::Orientation)3);
            qt_sendSpontaneousEvent(QApplicationPrivate::focus_widget, &qwe2);
            wheelOK = qwe2.isAccepted();
        }
    }
    if (!wheelOK) {
        return [super scrollWheel:theEvent];
    }
}

- (void)tabletProximity:(NSEvent *)tabletEvent
{
    qt_dispatchTabletProximityEvent(tabletEvent);
}

- (void)tabletPoint:(NSEvent *)tabletEvent
{
    if (!qt_mac_handleTabletEvent(self, tabletEvent))
        [super tabletPoint:tabletEvent];
}

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
- (void)touchesBeganWithEvent:(NSEvent *)event;
{
    bool all = qwidget->testAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    qt_translateRawTouchEvent(qwidget, QTouchEvent::TouchPad, QCocoaTouch::getCurrentTouchPointList(event, all));
}

- (void)touchesMovedWithEvent:(NSEvent *)event;
{
    bool all = qwidget->testAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    qt_translateRawTouchEvent(qwidget, QTouchEvent::TouchPad, QCocoaTouch::getCurrentTouchPointList(event, all));
}

- (void)touchesEndedWithEvent:(NSEvent *)event;
{
    bool all = qwidget->testAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    qt_translateRawTouchEvent(qwidget, QTouchEvent::TouchPad, QCocoaTouch::getCurrentTouchPointList(event, all));
}

- (void)touchesCancelledWithEvent:(NSEvent *)event;
{
    bool all = qwidget->testAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    qt_translateRawTouchEvent(qwidget, QTouchEvent::TouchPad, QCocoaTouch::getCurrentTouchPointList(event, all));
}

- (void)magnifyWithEvent:(NSEvent *)event;
{
    if (!QApplicationPrivate::tryModalHelper(qwidget, 0))
        return;

    QNativeGestureEvent qNGEvent;
    qNGEvent.gestureType = QNativeGestureEvent::Zoom;
    NSPoint p = [[event window] convertBaseToScreen:[event locationInWindow]];
    qNGEvent.position = flipPoint(p).toPoint();
    qNGEvent.percentage = [event magnification];
    qt_sendSpontaneousEvent(qwidget, &qNGEvent);
}

- (void)rotateWithEvent:(NSEvent *)event;
{
    if (!QApplicationPrivate::tryModalHelper(qwidget, 0))
        return;

    QNativeGestureEvent qNGEvent;
    qNGEvent.gestureType = QNativeGestureEvent::Rotate;
    NSPoint p = [[event window] convertBaseToScreen:[event locationInWindow]];
    qNGEvent.position = flipPoint(p).toPoint();
    qNGEvent.percentage = -[event rotation];
    qt_sendSpontaneousEvent(qwidget, &qNGEvent);
}

- (void)swipeWithEvent:(NSEvent *)event;
{
    if (!QApplicationPrivate::tryModalHelper(qwidget, 0))
        return;

    QNativeGestureEvent qNGEvent;
    qNGEvent.gestureType = QNativeGestureEvent::Swipe;
    NSPoint p = [[event window] convertBaseToScreen:[event locationInWindow]];
    qNGEvent.position = flipPoint(p).toPoint();
    if ([event deltaX] == 1)
        qNGEvent.angle = 180.0f;
    else if ([event deltaX] == -1)
        qNGEvent.angle = 0.0f;
    else if ([event deltaY] == 1)
        qNGEvent.angle = 90.0f;
    else if ([event deltaY] == -1)
        qNGEvent.angle = 270.0f;
    qt_sendSpontaneousEvent(qwidget, &qNGEvent);
}

- (void)beginGestureWithEvent:(NSEvent *)event;
{
    if (!QApplicationPrivate::tryModalHelper(qwidget, 0))
        return;

    QNativeGestureEvent qNGEvent;
    qNGEvent.gestureType = QNativeGestureEvent::GestureBegin;
    NSPoint p = [[event window] convertBaseToScreen:[event locationInWindow]];
    qNGEvent.position = flipPoint(p).toPoint();
    qt_sendSpontaneousEvent(qwidget, &qNGEvent);
}

- (void)endGestureWithEvent:(NSEvent *)event;
{
    if (!QApplicationPrivate::tryModalHelper(qwidget, 0))
        return;

    QNativeGestureEvent qNGEvent;
    qNGEvent.gestureType = QNativeGestureEvent::GestureEnd;
    NSPoint p = [[event window] convertBaseToScreen:[event locationInWindow]];
    qNGEvent.position = flipPoint(p).toPoint();
    qt_sendSpontaneousEvent(qwidget, &qNGEvent);
}
#endif // MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6

- (void)frameDidChange:(NSNotification *)note
{
    Q_UNUSED(note);
    if (qwidget->isWindow())
        return;
    NSRect newFrame = [self frame];
    QRect newGeo(newFrame.origin.x, newFrame.origin.y, newFrame.size.width, newFrame.size.height);
    bool moved = qwidget->testAttribute(Qt::WA_Moved);
    bool resized = qwidget->testAttribute(Qt::WA_Resized);
    qwidget->setGeometry(newGeo);
    qwidget->setAttribute(Qt::WA_Moved, moved);
    qwidget->setAttribute(Qt::WA_Resized, resized);
    qwidgetprivate->syncCocoaMask();
}

- (BOOL)isEnabled
{
    if (!qwidget)
        return [super isEnabled];
    return [super isEnabled] && qwidget->isEnabled();
}

- (void)setEnabled:(BOOL)flag
{
    QMacCocoaAutoReleasePool pool;
    [super setEnabled:flag];
    if (qwidget->isEnabled() != flag)
        qwidget->setEnabled(flag);
}

+ (Class)cellClass
{
    return [NSActionCell class];
}

- (BOOL)acceptsFirstResponder
{
    if (qwidget->isWindow())
        return YES;  // Always do it, so that windows can accept key press events.
    return qwidget->focusPolicy() != Qt::NoFocus;
}

- (BOOL)resignFirstResponder
{
    // Seems like the following test only triggers if this
    // view is inside a QMacNativeWidget:
    if (qwidget == QApplication::focusWidget())
        QApplicationPrivate::setFocusWidget(0, Qt::OtherFocusReason);
    return YES;
}

- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)isLocal
{
    Q_UNUSED(isLocal);
    return supportedActions;
}

- (void)setSupportedActions:(NSDragOperation)actions
{
    supportedActions = actions;
}

- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation
{
    Q_UNUSED(anImage);
    Q_UNUSED(aPoint);
    qMacDnDParams()->performedAction = operation;
    if (QDragManager::self()->object
        && QDragManager::self()->dragPrivate()->executed_action != Qt::ActionMask) {
        qMacDnDParams()->performedAction =
                qt_mac_mapDropAction(QDragManager::self()->dragPrivate()->executed_action);
    }
}

- (QWidget *)qt_qwidget
{
    return qwidget;
}

- (BOOL)qt_leftButtonIsRightButton
{
    return leftButtonIsRightButton;
}

- (void)qt_setLeftButtonIsRightButton:(BOOL)isSwapped
{
    leftButtonIsRightButton = isSwapped;
}

+ (DnDParams*)currentMouseEvent
{
    return qMacDnDParams();
}

- (void)keyDown:(NSEvent *)theEvent
{
    sendKeyEvents = true;

    QWidget *widgetToGetKey = qwidget;

    QWidget *popup = qAppInstance()->activePopupWidget();
    bool sendToPopup = false;
    if (popup && popup != qwidget->window()) {
        widgetToGetKey = popup->focusWidget() ? popup->focusWidget() : popup;
        sendToPopup = true;
    }

    if (widgetToGetKey->testAttribute(Qt::WA_InputMethodEnabled)
            && !(widgetToGetKey->inputMethodHints() & Qt::ImhDigitsOnly
                 || widgetToGetKey->inputMethodHints() & Qt::ImhFormattedNumbersOnly
                 || widgetToGetKey->inputMethodHints() & Qt::ImhHiddenText)) {
        [qt_mac_nativeview_for(widgetToGetKey) interpretKeyEvents:[NSArray arrayWithObject: theEvent]];
    }
    if (sendKeyEvents && !composing) {
        bool keyOK = qt_dispatchKeyEvent(theEvent, widgetToGetKey);
        if (!keyOK && !sendToPopup)
            [super keyDown:theEvent];
    }
}


- (void)keyUp:(NSEvent *)theEvent
{
    if (sendKeyEvents) {
        bool keyOK = qt_dispatchKeyEvent(theEvent, qwidget);
        if (!keyOK)
            [super keyUp:theEvent];
    }
}

- (void)viewWillMoveToWindow:(NSWindow *)window
{
    if (qwidget->windowFlags() & Qt::MSWindowsOwnDC
          && (window != [self window])) { // OpenGL Widget
        // Create a stupid ClearDrawable Event
        QEvent event(QEvent::MacGLClearDrawable);
        qApp->sendEvent(qwidget, &event);
    }
}

- (void)viewDidMoveToWindow
{
    if (qwidget->windowFlags() & Qt::MSWindowsOwnDC && [self window]) {
        // call update paint event
        qwidgetprivate->needWindowChange = true;
        QEvent event(QEvent::MacGLWindowChange);
        qApp->sendEvent(qwidget, &event);
    }
}


// NSTextInput Protocol implementation

- (void) insertText:(id)aString
{
    QString commitText;
    if ([aString length]) {
        if ([aString isKindOfClass:[NSAttributedString class]]) {
            commitText = QCFString::toQString(reinterpret_cast<CFStringRef>([aString string]));
        } else {
            commitText = QCFString::toQString(reinterpret_cast<CFStringRef>(aString));
        };
    }

    if ([aString length] && composing) {
        // Send the commit string to the widget.
        composing = false;
        sendKeyEvents = false;
        QInputMethodEvent e;
        e.setCommitString(commitText);
        qt_sendSpontaneousEvent(qwidget, &e);
    } else {
        // The key sequence "`q" on a French Keyboard will generate two calls to insertText before
        // it returns from interpretKeyEvents. The first call will turn off 'composing' and accept
        // the "`" key. The last keyDown event needs to be processed by the widget to get the
        // character "q". The string parameter is ignored for the second call.
        sendKeyEvents = true;
    }

    composingText->clear();
}

- (void) setMarkedText:(id)aString selectedRange:(NSRange)selRange
{
    // Generate the QInputMethodEvent with preedit string and the attributes
    // for rendering it. The attributes handled here are 'underline',
    // 'underline color' and 'cursor position'.
    sendKeyEvents = false;
    composing = true;
    QString qtText;
    // Cursor position is retrived from the range.
    QList<QInputMethodEvent::Attribute> attrs;
    attrs<<QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, selRange.location, 1, QVariant());
    if ([aString isKindOfClass:[NSAttributedString class]]) {
        qtText = QCFString::toQString(reinterpret_cast<CFStringRef>([aString string]));
        composingLength = qtText.length();
        int index = 0;
        // Create attributes for individual sections of preedit text
        while (index < composingLength) {
            NSRange effectiveRange;
            NSRange range = NSMakeRange(index, composingLength-index);
            NSDictionary *attributes = [aString attributesAtIndex:index
                                            longestEffectiveRange:&effectiveRange
                                                          inRange:range];
            NSNumber *underlineStyle = [attributes objectForKey:NSUnderlineStyleAttributeName];
            if (underlineStyle) {
                QColor clr (Qt::black);
                NSColor *color = [attributes objectForKey:NSUnderlineColorAttributeName];
                if (color) {
                    clr = colorFrom(color);
                }
                QTextCharFormat format;
                format.setFontUnderline(true);
                format.setUnderlineColor(clr);
                attrs<<QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                                    effectiveRange.location,
                                                    effectiveRange.length,
                                                    format);
            }
            index = effectiveRange.location + effectiveRange.length;
        }
    } else {
        // No attributes specified, take only the preedit text.
        qtText = QCFString::toQString(reinterpret_cast<CFStringRef>(aString));
        composingLength = qtText.length();
    }
    // Make sure that we have at least one text format.
    if (attrs.size() <= 1) {
        QTextCharFormat format;
        format.setFontUnderline(true);
        attrs<<QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                            0, composingLength, format);
    }
    *composingText = qtText;
    QInputMethodEvent e(qtText, attrs);
    qt_sendSpontaneousEvent(qwidget, &e);
    if (!composingLength)
        composing = false;
}

- (void) unmarkText
{
    if (composing) {
        QInputMethodEvent e;
        e.setCommitString(*composingText);
        qt_sendSpontaneousEvent(qwidget, &e);
    }
    composingText->clear();
    composing = false;
}

- (BOOL) hasMarkedText
{
    return (composing ? YES: NO);
}

- (void) doCommandBySelector:(SEL)aSelector
{
    Q_UNUSED(aSelector);
}

- (BOOL)isComposing
{
    return composing;
}

- (NSInteger) conversationIdentifier
{
    // Return a unique identifier fot this ime conversation
    return (NSInteger)self;
}

- (NSAttributedString *) attributedSubstringFromRange:(NSRange)theRange
{
    QString selectedText(qwidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
    if (!selectedText.isEmpty()) {
        QCFString string(selectedText.mid(theRange.location, theRange.length));
        const NSString *tmpString = reinterpret_cast<const NSString *>((CFStringRef)string);
        return [[[NSAttributedString alloc]  initWithString:tmpString] autorelease];
    } else {
        return nil;
    }
}

- (NSRange) markedRange
{
    NSRange range;
    if (composing) {
        range.location = 0;
        range.length = composingLength;
    } else {
        range.location = NSNotFound;
        range.length = 0;
    }
    return range;
}

- (NSRange) selectedRange
{
    NSRange selRange;
    QString selectedText(qwidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
    if (!selectedText.isEmpty()) {
        // Consider only the selected text.
        selRange.location = 0;
        selRange.length = selectedText.length();
    } else {
        // No selected text.
        selRange.location = NSNotFound;
        selRange.length = 0;
    }
    return selRange;

}

- (NSRect) firstRectForCharacterRange:(NSRange)theRange
{
    Q_UNUSED(theRange);
    // The returned rect is always based on the internal cursor.
    QRect mr(qwidget->inputMethodQuery(Qt::ImMicroFocus).toRect());
    QPoint mp(qwidget->mapToGlobal(QPoint(mr.bottomLeft())));
    NSRect rect ;
    rect.origin.x = mp.x();
    rect.origin.y = flipYCoordinate(mp.y());
    rect.size.width = mr.width();
    rect.size.height = mr.height();
    return rect;
}

- (NSUInteger)characterIndexForPoint:(NSPoint)thePoint
{
    // We dont support cursor movements using mouse while composing.
    Q_UNUSED(thePoint);
    return NSNotFound;
}

- (NSArray*) validAttributesForMarkedText
{
    if (!qwidget->testAttribute(Qt::WA_InputMethodEnabled))
        return nil;  // Not sure if that's correct, but it's saves a malloc.

    // Support only underline color/style.
    return [NSArray arrayWithObjects:NSUnderlineColorAttributeName,
                                     NSUnderlineStyleAttributeName, nil];
}
@end

QT_BEGIN_NAMESPACE
void QMacInputContext::reset()
{
    QWidget *w = QInputContext::focusWidget();
    if (w) {
        NSView *view = qt_mac_nativeview_for(w);
        if ([view isKindOfClass:[QT_MANGLE_NAMESPACE(QCocoaView) class]]) {
            QMacCocoaAutoReleasePool pool;
            QT_MANGLE_NAMESPACE(QCocoaView) *qc = static_cast<QT_MANGLE_NAMESPACE(QCocoaView) *>(view);
            NSInputManager *currentIManager = [NSInputManager currentInputManager];
            if (currentIManager) {
                [currentIManager markedTextAbandoned:view];
                [qc unmarkText];
            }
        }
    }
}

bool QMacInputContext::isComposing() const
{
    QWidget *w = QInputContext::focusWidget();
    if (w) {
        NSView *view = qt_mac_nativeview_for(w);
        if ([view isKindOfClass:[QT_MANGLE_NAMESPACE(QCocoaView) class]]) {
            return [static_cast<QT_MANGLE_NAMESPACE(QCocoaView) *>(view) isComposing];
        }
    }
    return false;
}

extern bool qt_mac_in_drag;
void * /*NSImage */qt_mac_create_nsimage(const QPixmap &pm);
static const int default_pm_hotx = -2;
static const int default_pm_hoty = -16;
static const char* default_pm[] = {
    "13 9 3 1",
    ".      c None",
    "       c #000000",
    "X      c #FFFFFF",
    "X X X X X X X",
    " X X X X X X ",
    "X ......... X",
    " X.........X ",
    "X ......... X",
    " X.........X ",
    "X ......... X",
    " X X X X X X ",
    "X X X X X X X",
};

Qt::DropAction QDragManager::drag(QDrag *o)
{
    if(qt_mac_in_drag) {     //just make sure..
        qWarning("Qt: Internal error: WH0A, unexpected condition reached");
        return Qt::IgnoreAction;
    }
    if(object == o)
        return Qt::IgnoreAction;
    /* At the moment it seems clear that Mac OS X does not want to drag with a non-left button
     so we just bail early to prevent it */
    if(!(GetCurrentEventButtonState() & kEventMouseButtonPrimary))
        return Qt::IgnoreAction;

    if(object) {
        dragPrivate()->source->removeEventFilter(this);
        cancel();
        beingCancelled = false;
    }

    object = o;
    dragPrivate()->target = 0;

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(this, 0, QAccessible::DragDropStart);
#endif

    // setup the data
    QMacPasteboard dragBoard((CFStringRef) NSDragPboard, QMacPasteboardMime::MIME_DND);
    dragPrivate()->data->setData(QLatin1String("application/x-qt-mime-type-name"), QByteArray());
    dragBoard.setMimeData(dragPrivate()->data);

    // create the image
    QPoint hotspot;
    QPixmap pix = dragPrivate()->pixmap;
    if(pix.isNull()) {
        if(dragPrivate()->data->hasText() || dragPrivate()->data->hasUrls()) {
            // get the string
            QString s = dragPrivate()->data->hasText() ? dragPrivate()->data->text()
            : dragPrivate()->data->urls().first().toString();
            if(s.length() > 26)
                s = s.left(23) + QChar(0x2026);
            if(!s.isEmpty()) {
                // draw it
                QFont f(qApp->font());
                f.setPointSize(12);
                QFontMetrics fm(f);
                QPixmap tmp(fm.width(s), fm.height());
                if(!tmp.isNull()) {
                    QPainter p(&tmp);
                    p.fillRect(0, 0, tmp.width(), tmp.height(), Qt::color0);
                    p.setPen(Qt::color1);
                    p.setFont(f);
                    p.drawText(0, fm.ascent(), s);
                    // save it
                    pix = tmp;
                    hotspot = QPoint(tmp.width() / 2, tmp.height() / 2);
                }
            }
        } else {
            pix = QPixmap(default_pm);
            hotspot = QPoint(default_pm_hotx, default_pm_hoty);
        }
    } else {
        hotspot = dragPrivate()->hotspot;
    }
    // convert the image to NSImage.
    NSImage *image = (NSImage *)qt_mac_create_nsimage(pix);
    [image retain];
    DnDParams dndParams = *[QT_MANGLE_NAMESPACE(QCocoaView) currentMouseEvent];
    // save supported actions
    [dndParams.view setSupportedActions: qt_mac_mapDropActions(dragPrivate()->possible_actions)];
    NSPoint imageLoc = {dndParams.localPoint.x - hotspot.x(),
                        dndParams.localPoint.y + pix.height() - hotspot.y()};
    NSSize mouseOffset = {0.0, 0.0};
    NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
    NSPoint windowPoint = [dndParams.theEvent locationInWindow];
    dragPrivate()->executed_action = Qt::ActionMask;
    // do the drag
    [dndParams.view retain];
    [dndParams.view dragImage:image
                            at:imageLoc
                        offset:mouseOffset
                         event:dndParams.theEvent
                    pasteboard:pboard
                        source:dndParams.view
                     slideBack:YES];
    [dndParams.view release];
    [image release];
    dragPrivate()->executed_action = Qt::IgnoreAction;
    object = 0;
    Qt::DropAction performedAction(qt_mac_mapNSDragOperation(qMacDnDParams()->performedAction));
    // do post drag processing, if required.
    if(performedAction != Qt::IgnoreAction) {
        // check if the receiver points us to a file location.
        // if so, we need to do the file copy/move ourselves.
        QCFType<CFURLRef> pasteLocation = 0;
        PasteboardCopyPasteLocation(dragBoard.pasteBoard(), &pasteLocation);
        if (pasteLocation) {
            QList<QUrl> urls = o->mimeData()->urls();
            for (int i = 0; i < urls.size(); ++i) {
                QUrl fromUrl = urls.at(i);
                QString filename = QFileInfo(fromUrl.path()).fileName();
                QUrl toUrl(QCFString::toQString(CFURLGetString(pasteLocation)) + filename);
                if (performedAction == Qt::MoveAction)
                    QFile::rename(fromUrl.path(), toUrl.path());
                else if (performedAction == Qt::CopyAction)
                    QFile::copy(fromUrl.path(), toUrl.path());
            }
        }
    }
    return performedAction;
}

QT_END_NAMESPACE

#endif // QT_MAC_USE_COCOA
