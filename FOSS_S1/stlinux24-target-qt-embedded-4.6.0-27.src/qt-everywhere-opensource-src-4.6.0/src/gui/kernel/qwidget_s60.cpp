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

#include "qwidget_p.h"
#include "qdesktopwidget.h"
#include "qapplication.h"
#include "qapplication_p.h"
#include "private/qbackingstore_p.h"
#include "qevent.h"
#include "qt_s60_p.h"

#include "qbitmap.h"
#include "private/qwindowsurface_s60_p.h"

#include <qinputcontext.h>

#ifdef Q_WS_S60
#include <aknappui.h>
#endif

// This is necessary in order to be able to perform delayed invokation on slots
// which take arguments of type WId.  One example is
// QWidgetPrivate::_q_delayedDestroy, which is used to delay destruction of
// CCoeControl objects until after the CONE event handler has finished running.
Q_DECLARE_METATYPE(WId)

QT_BEGIN_NAMESPACE

extern bool qt_nograb();

QWidget *QWidgetPrivate::mouseGrabber = 0;
QWidget *QWidgetPrivate::keyboardGrabber = 0;

static bool isEqual(const QList<QAction*>& a, const QList<QAction*>& b)
{
    if ( a.count() != b.count())
        return false;
    int index=0;
    while (index<a.count()) {
        if (a.at(index)->softKeyRole() != b.at(index)->softKeyRole())
            return false;
        if (a.at(index)->text().compare(b.at(index)->text())!=0)
            return false;
        index++;
    }
    return true;
}

void QWidgetPrivate::setWSGeometry(bool dontShow, const QRect &)
{
    // Note: based on x11 implementation

    static const int XCOORD_MAX = 16383;
    static const int WRECT_MAX = 16383;

    Q_Q(QWidget);

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));

    /*
      There are up to four different coordinate systems here:
      Qt coordinate system for this widget.
      Symbian coordinate system for this widget (relative to wrect).
      Qt coordinate system for parent
      Symbian coordinate system for parent (relative to parent's wrect).
     */

    QRect validRange(-XCOORD_MAX,-XCOORD_MAX, 2*XCOORD_MAX, 2*XCOORD_MAX);
    QRect wrectRange(-WRECT_MAX,-WRECT_MAX, 2*WRECT_MAX, 2*WRECT_MAX);
    QRect wrect;
    //xrect is the Symbian geometry of my widget. (starts out in parent's Qt coord sys, and ends up in parent's Symbian coord sys)
    QRect xrect = data.crect;

    const QWidget *const parent = q->parentWidget();
    QRect parentWRect = parent->data->wrect;

    if (parentWRect.isValid()) {
        // parent is clipped, and we have to clip to the same limit as parent
        if (!parentWRect.contains(xrect)) {
            xrect &= parentWRect;
            wrect = xrect;
            //translate from parent's to my Qt coord sys
            wrect.translate(-data.crect.topLeft());
        }
        //translate from parent's Qt coords to parent's X coords
        xrect.translate(-parentWRect.topLeft());

    } else {
        // parent is not clipped, we may or may not have to clip

        if (data.wrect.isValid() && QRect(QPoint(),data.crect.size()).contains(data.wrect)) {
            // This is where the main optimization is: we are already
            // clipped, and if our clip is still valid, we can just
            // move our window, and do not need to move or clip
            // children

            QRect vrect = xrect & parent->rect();
            vrect.translate(-data.crect.topLeft()); //the part of me that's visible through parent, in my Qt coords
            if (data.wrect.contains(vrect)) {
                xrect = data.wrect;
                xrect.translate(data.crect.topLeft());
                if (data.winid)
                    data.winid->SetExtent(TPoint(xrect.x(), xrect.y()), TSize(xrect.width(), xrect.height()));
                return;
            }
        }

        if (!validRange.contains(xrect)) {
            // we are too big, and must clip
            xrect &=wrectRange;
            wrect = xrect;
            wrect.translate(-data.crect.topLeft());
            //parent's X coord system is equal to parent's Qt coord
            //sys, so we don't need to map xrect.
        }
    }

    // unmap if we are outside the valid window system coord system
    bool outsideRange = !xrect.isValid();
    bool mapWindow = false;
    if (q->testAttribute(Qt::WA_OutsideWSRange) != outsideRange) {
        q->setAttribute(Qt::WA_OutsideWSRange, outsideRange);
        if (outsideRange) {
            if (data.winid)
                data.winid->DrawableWindow()->SetVisible(EFalse);
            q->setAttribute(Qt::WA_Mapped, false);
        } else if (!q->isHidden()) {
            mapWindow = true;
        }
    }

    if (outsideRange)
        return;

    bool jump = (data.wrect != wrect);
    data.wrect = wrect;

    // and now recursively for all children...
    for (int i = 0; i < children.size(); ++i) {
        QObject *object = children.at(i);
        if (object->isWidgetType()) {
            QWidget *w = static_cast<QWidget *>(object);
            if (!w->isWindow() && w->testAttribute(Qt::WA_WState_Created))
                w->d_func()->setWSGeometry(jump);
        }
    }

    if (data.winid) {
        // move ourselves to the new position and map (if necessary) after
        // the movement. Rationale: moving unmapped windows is much faster
        // than moving mapped windows
        if (!parent->internalWinId())
            xrect.translate(parent->mapTo(q->nativeParentWidget(), QPoint(0, 0)));

        data.winid->SetExtent(TPoint(xrect.x(), xrect.y()), TSize(xrect.width(), xrect.height()));
    }

    if (mapWindow and !dontShow) {
        q->setAttribute(Qt::WA_Mapped);
        if (q->internalWinId())
            q->internalWinId()->DrawableWindow()->SetVisible(ETrue);
    }

    if  (jump && data.winid) {
        RWindow *const window = static_cast<RWindow *>(data.winid->DrawableWindow());
        window->Invalidate(TRect(0, 0, wrect.width(), wrect.height()));
    }
}

void QWidgetPrivate::setGeometry_sys(int x, int y, int w, int h, bool isMove)
{
    Q_Q(QWidget);

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));

    if ((q->windowType() == Qt::Desktop))
        return;

    QPoint oldPos(q->pos());
    QSize oldSize(q->size());
    QRect oldGeom(data.crect);

    // Lose maximized status if deliberate resize
    if (w != oldSize.width() || h != oldSize.height())
        data.window_state &= ~Qt::WindowMaximized;

    if (extra) {                                // any size restrictions?
        w = qMin(w,extra->maxw);
        h = qMin(h,extra->maxh);
        w = qMax(w,extra->minw);
        h = qMax(h,extra->minh);
    }

    if (q->isWindow())
        topData()->normalGeometry = QRect(0, 0, -1, -1);
    else {
        uint s = data.window_state;
        s &= ~(Qt::WindowMaximized | Qt::WindowFullScreen);
        data.window_state = s;
    }

    bool isResize = w != oldSize.width() || h != oldSize.height();
    if (!isMove && !isResize)
        return;

    if (q->isWindow()) {
        if (w == 0 || h == 0) {
            q->setAttribute(Qt::WA_OutsideWSRange, true);
            if (q->isVisible() && q->testAttribute(Qt::WA_Mapped))
                hide_sys();
            data.crect = QRect(x, y, w, h);
            data.window_state &= ~Qt::WindowFullScreen;
        } else if (q->isVisible() && q->testAttribute(Qt::WA_OutsideWSRange)) {
            q->setAttribute(Qt::WA_OutsideWSRange, false);

            // put the window in its place and show it
            q->internalWinId()->SetRect(TRect(TPoint(x, y), TSize(w, h)));
            data.crect.setRect(x, y, w, h);
            show_sys();
        } else {
            QRect r = QRect(x, y, w, h);
            data.crect = r;
            q->internalWinId()->SetRect(TRect(TPoint(x, y), TSize(w, h)));
            topData()->normalGeometry = data.crect;
        }
    } else {
        data.crect.setRect(x, y, w, h);

        QTLWExtra *tlwExtra = q->window()->d_func()->maybeTopData();
        const bool inTopLevelResize = tlwExtra ? tlwExtra->inTopLevelResize : false;

        if (q->isVisible() && (!inTopLevelResize || q->internalWinId())) {
            // Top-level resize optimization does not work for native child widgets;
            // disable it for this particular widget.
            if (inTopLevelResize)
                tlwExtra->inTopLevelResize = false;
            if (!isResize && maybeBackingStore())
                moveRect(QRect(oldPos, oldSize), x - oldPos.x(), y - oldPos.y());
            else
                invalidateBuffer_resizeHelper(oldPos, oldSize);

            if (inTopLevelResize)
                tlwExtra->inTopLevelResize = true;
        }
        if (q->testAttribute(Qt::WA_WState_Created))
            setWSGeometry();
    }

    if (q->isVisible()) {
        if (isMove && q->pos() != oldPos) {
            QMoveEvent e(q->pos(), oldPos);
            QApplication::sendEvent(q, &e);
        }
        if (isResize) {
            bool slowResize = qgetenv("QT_SLOW_TOPLEVEL_RESIZE").toInt();
            const bool setTopLevelResize = !slowResize && q->isWindow() && extra && extra->topextra
                                           && !extra->topextra->inTopLevelResize;
            if (setTopLevelResize)
                extra->topextra->inTopLevelResize = true;
            QResizeEvent e(q->size(), oldSize);
            QApplication::sendEvent(q, &e);
            if (!q->testAttribute(Qt::WA_StaticContents) && q->internalWinId())
                q->internalWinId()->DrawDeferred();
            if (setTopLevelResize)
                extra->topextra->inTopLevelResize = false;
        }
    } else {
        if (isMove && q->pos() != oldPos)
            q->setAttribute(Qt::WA_PendingMoveEvent, true);
        if (isResize)
            q->setAttribute(Qt::WA_PendingResizeEvent, true);
    }
}

void QWidgetPrivate::create_sys(WId window, bool /* initializeWindow */, bool destroyOldWindow)
{
    Q_Q(QWidget);

    Qt::WindowType type = q->windowType();
    Qt::WindowFlags &flags = data.window_flags;
    QWidget *parentWidget = q->parentWidget();

    bool topLevel = (flags & Qt::Window);
    bool popup = (type == Qt::Popup);
    bool dialog = (type == Qt::Dialog
                   || type == Qt::Sheet
                   || (flags & Qt::MSWindowsFixedSizeDialogHint));
    bool desktop = (type == Qt::Desktop);
    //bool tool = (type == Qt::Tool || type == Qt::Drawer);

    if (popup)
        flags |= Qt::WindowStaysOnTopHint; // a popup stays on top

    TRect clientRect = static_cast<CEikAppUi*>(S60->appUi())->ClientRect();
    int sw = clientRect.Width();
    int sh = clientRect.Height();

    if (desktop) {
        TSize screenSize = S60->screenDevice()->SizeInPixels();
        data.crect.setRect(0, 0, screenSize.iWidth, screenSize.iHeight);
        q->setAttribute(Qt::WA_DontShowOnScreen);
    } else if (topLevel && !q->testAttribute(Qt::WA_Resized)){
        int width = sw;
        int height = sh;
        if (extra) {
            width = qMax(qMin(width, extra->maxw), extra->minw);
            height = qMax(qMin(height, extra->maxh), extra->minh);
        }
        data.crect.setSize(QSize(width, height));
    }

    CCoeControl *const destroyw = destroyOldWindow ? data.winid : 0;

    createExtra();
    if (window) {
        setWinId(window);
        TRect tr = window->Rect();
        data.crect.setRect(tr.iTl.iX, tr.iTl.iY, tr.Width(), tr.Height());

    } else if (topLevel) {
        if (!q->testAttribute(Qt::WA_Moved) && !q->testAttribute(Qt::WA_DontShowOnScreen))
            data.crect.moveTopLeft(QPoint(clientRect.iTl.iX, clientRect.iTl.iY));

        QScopedPointer<QSymbianControl> control( q_check_ptr(new QSymbianControl(q)) );
        QT_TRAP_THROWING(control->ConstructL(true, desktop));
        control->SetMopParent(static_cast<CEikAppUi*>(S60->appUi()));

        // Symbian windows are always created in an inactive state
        // We perform this assignment for the case where the window is being re-created
        // as aa result of a call to setParent_sys, on either this widget or one of its
        // ancestors.
        extra->activated = 0;

        if (!desktop) {
            TInt stackingFlags;
            if ((q->windowType() & Qt::Popup) == Qt::Popup) {
                stackingFlags = ECoeStackFlagRefusesAllKeys | ECoeStackFlagRefusesFocus;
            } else {
                stackingFlags = ECoeStackFlagStandard;
            }
            control->MakeVisible(false);
            QT_TRAP_THROWING(control->ControlEnv()->AppUi()->AddToStackL(control.data(), ECoeStackPriorityDefault, stackingFlags));
            // Avoid keyboard focus to a hidden window.
            control->setFocusSafely(false);

            RDrawableWindow *const drawableWindow = control->DrawableWindow();
            // Request mouse move events.
            drawableWindow->PointerFilter(EPointerFilterEnterExit
                | EPointerFilterMove | EPointerFilterDrag, 0);
            drawableWindow->EnableVisibilityChangeEvents();

            if (!isOpaque) {
                RWindow *const window = static_cast<RWindow *>(drawableWindow);
                const TDisplayMode displayMode = static_cast<TDisplayMode>(window->SetRequiredDisplayMode(EColor16MA));
                if (window->SetTransparencyAlphaChannel() == KErrNone)
                    window->SetBackgroundColor(TRgb(255, 255, 255, 0));
            }
        }

        q->setAttribute(Qt::WA_WState_Created);

        int x, y, w, h;
        data.crect.getRect(&x, &y, &w, &h);
        control->SetRect(TRect(TPoint(x, y), TSize(w, h)));

        // We wait until the control is fully constructed before calling setWinId, because
        // this generates a WinIdChanged event.
        setWinId(control.take());

    } else if (q->testAttribute(Qt::WA_NativeWindow) || paintOnScreen()) { // create native child widget

        QScopedPointer<QSymbianControl> control( q_check_ptr(new QSymbianControl(q)) );
        QT_TRAP_THROWING(control->ConstructL(!parentWidget));

        // Symbian windows are always created in an inactive state
        // We perform this assignment for the case where the window is being re-created
        // as aa result of a call to setParent_sys, on either this widget or one of its
        // ancestors.
        extra->activated = 0;

        TInt stackingFlags;
        if ((q->windowType() & Qt::Popup) == Qt::Popup) {
            stackingFlags = ECoeStackFlagRefusesAllKeys | ECoeStackFlagRefusesFocus;
        } else {
            stackingFlags = ECoeStackFlagStandard;
        }
        control->MakeVisible(false);
        QT_TRAP_THROWING(control->ControlEnv()->AppUi()->AddToStackL(control.data(), ECoeStackPriorityDefault, stackingFlags));
        // Avoid keyboard focus to a hidden window.
        control->setFocusSafely(false);

        q->setAttribute(Qt::WA_WState_Created);
        int x, y, w, h;
        data.crect.getRect(&x, &y, &w, &h);
        control->SetRect(TRect(TPoint(x, y), TSize(w, h)));

        RDrawableWindow *const drawableWindow = control->DrawableWindow();
        // Request mouse move events.
        drawableWindow->PointerFilter(EPointerFilterEnterExit
            | EPointerFilterMove | EPointerFilterDrag, 0);

        if (q->isVisible() && q->testAttribute(Qt::WA_Mapped)) {
            activateSymbianWindow(control.data());
            control->MakeVisible(true);
        }

        // We wait until the control is fully constructed before calling setWinId, because
        // this generates a WinIdChanged event.
        setWinId(control.take());
    }

    if (destroyw) {
        destroyw->ControlEnv()->AppUi()->RemoveFromStack(destroyw);

        // Delay deletion of the control in case this function is called in the
        // context of a CONE event handler such as
        // CCoeControl::ProcessPointerEventL
        QMetaObject::invokeMethod(q, "_q_delayedDestroy",
            Qt::QueuedConnection, Q_ARG(WId, destroyw));
    }

    if (q->testAttribute(Qt::WA_AcceptTouchEvents))
        registerTouchWindow();
}


void QWidgetPrivate::show_sys()
{
    Q_Q(QWidget);

    if (q->testAttribute(Qt::WA_OutsideWSRange))
        return;

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));

    q->setAttribute(Qt::WA_Mapped);

    if (q->testAttribute(Qt::WA_DontShowOnScreen)) {
        invalidateBuffer(q->rect());
        return;
    }

    if (q->internalWinId()) {
        if (!extra->activated)
             activateSymbianWindow();

         QSymbianControl *id = static_cast<QSymbianControl *>(q->internalWinId());

        id->MakeVisible(true);

        if(q->isWindow())
            id->setFocusSafely(true);

        // Force setting of the icon after window is made visible,
        // this is needed even WA_SetWindowIcon is not set, as in that case we need
        // to reset to the application level window icon
        if(q->isWindow())
            setWindowIcon_sys(true);
    }

    invalidateBuffer(q->rect());
}

void QWidgetPrivate::activateSymbianWindow(WId wid)
{
    Q_Q(QWidget);

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
    Q_ASSERT(q->testAttribute(Qt::WA_Mapped));
    Q_ASSERT(!extra->activated);

    if(!wid)
        wid = q->internalWinId();

    Q_ASSERT(wid);

    QT_TRAP_THROWING(wid->ActivateL());
    extra->activated = 1;
}

void QWidgetPrivate::hide_sys()
{
    Q_Q(QWidget);

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
    deactivateWidgetCleanup();
    QSymbianControl *id = static_cast<QSymbianControl *>(q->internalWinId());

    if (id) {
        //Incorrect optimisation - for popup windows, Qt's focus is moved before
        //hide_sys is called, resulting in the popup window keeping its elevated
        //position in the CONE control stack.
        //This can result in keyboard focus being in an invisible widget in some
        //conditions - e.g. QTBUG-4733
        //if(id->IsFocused()) // Avoid unnecessary calls to FocusChanged()
            id->setFocusSafely(false);
        id->MakeVisible(false);
        if (QWidgetBackingStore *bs = maybeBackingStore())
            bs->releaseBuffer();
    } else {
        invalidateBuffer(q->rect());
    }

    q->setAttribute(Qt::WA_Mapped, false);
}

void QWidgetPrivate::setFocus_sys()
{
    Q_Q(QWidget);
    if (q->testAttribute(Qt::WA_WState_Created) && q->window()->windowType() != Qt::Popup)
        if (!q->effectiveWinId()->IsFocused()) // Avoid unnecessry calls to FocusChanged()
            static_cast<QSymbianControl *>(q->effectiveWinId())->setFocusSafely(true);
}

void QWidgetPrivate::raise_sys()
{
    Q_Q(QWidget);

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
    if (q->internalWinId()) {
        q->internalWinId()->DrawableWindow()->SetOrdinalPosition(0);

        // If toplevel widget, raise app to foreground
        if (q->isWindow())
            S60->wsSession().SetWindowGroupOrdinalPosition(S60->windowGroup().Identifier(), 0);
    }
}

void QWidgetPrivate::lower_sys()
{
    Q_Q(QWidget);

    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
    if (q->internalWinId()) {
        // If toplevel widget, lower app to background
        if (q->isWindow())
            S60->wsSession().SetWindowGroupOrdinalPosition(S60->windowGroup().Identifier(), -1);
        else
            q->internalWinId()->DrawableWindow()->SetOrdinalPosition(-1);
    }

    if (!q->isWindow())
        invalidateBuffer(q->rect());
}

void QWidgetPrivate::setModal_sys()
{

}

void QWidgetPrivate::stackUnder_sys(QWidget* w)
{
    Q_Q(QWidget);
    Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));

    if (q->internalWinId() && w->internalWinId()) {
        RDrawableWindow *const thisWindow = q->internalWinId()->DrawableWindow();
        RDrawableWindow *const otherWindow = w->internalWinId()->DrawableWindow();
        thisWindow->SetOrdinalPosition(otherWindow->OrdinalPosition() + 1);
    }

    if (!q->isWindow() || !w->internalWinId())
        invalidateBuffer(q->rect());
}

void QWidgetPrivate::reparentChildren()
{
    Q_Q(QWidget);

    QObjectList chlist = q->children();
    for (int i = 0; i < chlist.size(); ++i) { // reparent children
        QObject *obj = chlist.at(i);
        if (obj->isWidgetType()) {
            QWidget *w = (QWidget *)obj;
            if (!w->testAttribute(Qt::WA_WState_Created))
                continue;
            if (!w->isWindow()) {
                w->d_func()->invalidateBuffer(w->rect());
                WId parent = q->effectiveWinId();
                WId child = w->effectiveWinId();
                if (parent != child) {
                    // Child widget is native.  Because Symbian windows cannot be
                    // re-parented, we must re-create the window.
                    const WId window = 0;
                    const bool initializeWindow = false;
                    const bool destroyOldWindow = true;
                    w->d_func()->create_sys(window, initializeWindow, destroyOldWindow);
                }
                // ### TODO: We probably also need to update the component array here
                w->d_func()->reparentChildren();
            } else {
                bool showIt = w->isVisible();
                QPoint old_pos = w->pos();
                w->setParent(q, w->windowFlags());
                w->move(old_pos);
                if (showIt)
                    w->show();
            }
        }
    }
}

void QWidgetPrivate::setParent_sys(QWidget *parent, Qt::WindowFlags f)
{
    Q_Q(QWidget);

    bool wasCreated = q->testAttribute(Qt::WA_WState_Created);

    if (q->isVisible() && q->parentWidget() && parent != q->parentWidget())
        q->parentWidget()->d_func()->invalidateBuffer(q->geometry());

    if (q->testAttribute(Qt::WA_DropSiteRegistered))
        q->setAttribute(Qt::WA_DropSiteRegistered, false);

    QSymbianControl *old_winid = static_cast<QSymbianControl *>(wasCreated ? data.winid : 0);
    if ((q->windowType() == Qt::Desktop))
        old_winid = 0;
    setWinId(0);

    // hide and reparent our own window away. Otherwise we might get
    // destroyed when emitting the child remove event below. See QWorkspace.
    if (wasCreated && old_winid) {
        old_winid->MakeVisible(false);
        if (old_winid->IsFocused()) // Avoid unnecessary calls to FocusChanged()
            old_winid->setFocusSafely(false);
        old_winid->SetParent(0);
    }

    QObjectPrivate::setParent_helper(parent);
    bool explicitlyHidden = q->testAttribute(Qt::WA_WState_Hidden) && q->testAttribute(Qt::WA_WState_ExplicitShowHide);

    data.window_flags = f;
    data.fstrut_dirty = true;
    q->setAttribute(Qt::WA_WState_Created, false);
    q->setAttribute(Qt::WA_WState_Visible, false);
    q->setAttribute(Qt::WA_WState_Hidden, false);
    adjustFlags(data.window_flags, q);
    // keep compatibility with previous versions, we need to preserve the created state
    // (but we recreate the winId for the widget being reparented, again for compatibility)
    if (wasCreated || (!q->isWindow() && parent->testAttribute(Qt::WA_WState_Created)))
        createWinId();
    if (q->isWindow() || (!parent || parent->isVisible()) || explicitlyHidden)
        q->setAttribute(Qt::WA_WState_Hidden);
    q->setAttribute(Qt::WA_WState_ExplicitShowHide, explicitlyHidden);

    if (wasCreated)
        reparentChildren();

    if (old_winid) {
        CBase::Delete(old_winid);
    }

    if (q->testAttribute(Qt::WA_AcceptDrops)
        || (!q->isWindow() && q->parentWidget() && q->parentWidget()->testAttribute(Qt::WA_DropSiteRegistered)))
        q->setAttribute(Qt::WA_DropSiteRegistered, true);

    invalidateBuffer(q->rect());
}

void QWidgetPrivate::setConstraints_sys()
{

}


void QWidgetPrivate::s60UpdateIsOpaque()
{
    Q_Q(QWidget);

    if (!q->testAttribute(Qt::WA_WState_Created) || !q->testAttribute(Qt::WA_TranslucentBackground))
        return;

    if ((data.window_flags & Qt::FramelessWindowHint) == 0)
        return;

    RWindow *const window = static_cast<RWindow *>(q->effectiveWinId()->DrawableWindow());

    if (!isOpaque) {
        const TDisplayMode displayMode = static_cast<TDisplayMode>(window->SetRequiredDisplayMode(EColor16MA));
        if (window->SetTransparencyAlphaChannel() == KErrNone)
            window->SetBackgroundColor(TRgb(255, 255, 255, 0));
    } else
        window->SetTransparentRegion(TRegionFix<1>());
}

void QWidgetPrivate::setWindowIcon_sys(bool forceReset)
{
#ifdef Q_WS_S60
    Q_Q(QWidget);

    if (!q->testAttribute(Qt::WA_WState_Created) || !q->isWindow() )
        return;

    QTLWExtra* topData = this->topData();
    if (topData->iconPixmap && !forceReset)
        // already been set
        return;

    TRect cPaneRect;
    TBool found = AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EContextPane, cPaneRect );
    CAknContextPane* contextPane = S60->contextPane();
    if (found && contextPane) { // We have context pane with valid metrics
        QIcon icon = q->windowIcon();
        if (!icon.isNull()) {
            // Valid icon -> set it as an context pane picture
            QSize size = icon.actualSize(QSize(cPaneRect.Size().iWidth, cPaneRect.Size().iHeight));
            QPixmap pm = icon.pixmap(size);
            QBitmap mask = pm.mask();
            if (mask.isNull()) {
                mask = QBitmap(pm.size());
                mask.fill(Qt::color1);
            }

            CFbsBitmap* nBitmap = pm.toSymbianCFbsBitmap();
            CFbsBitmap* nMask = mask.toSymbianCFbsBitmap();
            contextPane->SetPicture(nBitmap,nMask);
        } else {
            // Icon set to null -> set context pane picture to default
            QT_TRAP_THROWING(contextPane->SetPictureToDefaultL());
        }
    } else {
        // Context pane does not exist, try setting small icon to title pane
        TRect titlePaneRect;
        TBool found = AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, titlePaneRect );
        CAknTitlePane* titlePane = S60->titlePane();
        if (found && titlePane) { // We have title pane with valid metrics
            // The API to get title_pane graphics size is not public -> assume square space based
            // on titlebar font height. CAknBitmap would be optimum, wihtout setting the size, since
            // then title pane would automatically scale the bitmap. Unfortunately it is not public API
            // Also this function is leaving, although it is not named as such.
            const CFont * font;
            QT_TRAP_THROWING(font = AknLayoutUtils::FontFromId(EAknLogicalFontTitleFont));
            TSize iconSize(font->HeightInPixels(), font->HeightInPixels());

            QIcon icon = q->windowIcon();
            if (!icon.isNull()) {
                // Valid icon -> set it as an title pane small picture
                QSize size = icon.actualSize(QSize(iconSize.iWidth, iconSize.iHeight));
                QPixmap pm = icon.pixmap(size);
                QBitmap mask = pm.mask();
                if (mask.isNull()) {
                    mask = QBitmap(pm.size());
                    mask.fill(Qt::color1);
                }

                CFbsBitmap* nBitmap = pm.toSymbianCFbsBitmap();
                CFbsBitmap* nMask = mask.toSymbianCFbsBitmap();
                titlePane->SetSmallPicture( nBitmap, nMask, ETrue );
            } else {
                // Icon set to null -> set context pane picture to default
                titlePane->SetSmallPicture( NULL, NULL, EFalse );
            }
        }
    }

#else
        Q_UNUSED(forceReset)
#endif
}

void QWidgetPrivate::setWindowTitle_sys(const QString &caption)
{
#ifdef Q_WS_S60
    Q_Q(QWidget);
    if (q->isWindow()) {
        Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
        CAknTitlePane* titlePane = S60->titlePane();
        if (titlePane) {
            if (caption.isEmpty()) {
                QT_TRAP_THROWING(titlePane->SetTextToDefaultL());
            } else {
                QT_TRAP_THROWING(titlePane->SetTextL(qt_QString2TPtrC(caption)));
            }
        }
    }
#else
    Q_UNUSED(caption)
#endif
}

void QWidgetPrivate::setWindowIconText_sys(const QString & /*iconText */)
{

}

void QWidgetPrivate::scroll_sys(int dx, int dy)
{
    Q_Q(QWidget);

    scrollChildren(dx, dy);
    if (!paintOnScreen() || !q->internalWinId() || !q->internalWinId()->OwnsWindow()) {
        scrollRect(q->rect(), dx, dy);
    } else {
        Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
        RDrawableWindow *const window = q->internalWinId()->DrawableWindow();
        window->Scroll(TPoint(dx, dy));
    }
}

void QWidgetPrivate::scroll_sys(int dx, int dy, const QRect &r)
{
    Q_Q(QWidget);

    if (!paintOnScreen() || !q->internalWinId() || !q->internalWinId()->OwnsWindow()) {
        scrollRect(r, dx, dy);
    } else {
        Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
        RDrawableWindow *const window = q->internalWinId()->DrawableWindow();
        window->Scroll(TPoint(dx, dy), qt_QRect2TRect(r));
    }
}

/*!
    For this function to work in the emulator, you must add:
       TRANSPARENCY
    To a line in the wsini.ini file.
*/
void QWidgetPrivate::setWindowOpacity_sys(qreal)
{
    // ### TODO: Implement uniform window transparency
}

void QWidgetPrivate::updateFrameStrut()
{

}

void QWidgetPrivate::updateSystemBackground()
{

}

void QWidgetPrivate::registerDropSite(bool /* on */)
{

}

void QWidgetPrivate::createTLSysExtra()
{
    extra->topextra->backingStore = 0;
}

void QWidgetPrivate::deleteTLSysExtra()
{
    delete extra->topextra->backingStore;
    extra->topextra->backingStore = 0;
}

void QWidgetPrivate::createSysExtra()
{
    extra->activated = 0;
    extra->disableBlit = 0;
}

void QWidgetPrivate::deleteSysExtra()
{
    // this should only be non-zero if destroy() has not run due to constructor fail
    if (data.winid) {
        data.winid->ControlEnv()->AppUi()->RemoveFromStack(data.winid);
        delete data.winid;
        data.winid = 0;
    }
}

QWindowSurface *QWidgetPrivate::createDefaultWindowSurface_sys()
{
    return new QS60WindowSurface(q_func());
}

void QWidgetPrivate::setMask_sys(const QRegion& /* region */)
{

}

void QWidgetPrivate::registerTouchWindow()
{
#ifdef QT_SYMBIAN_SUPPORTS_ADVANCED_POINTER
    Q_Q(QWidget);
    if (q->testAttribute(Qt::WA_WState_Created) && q->windowType() != Qt::Desktop) {
        RWindow *rwindow = static_cast<RWindow *>(q->effectiveWinId()->DrawableWindow());
        rwindow->EnableAdvancedPointers();
    }
#endif
}

int QWidget::metric(PaintDeviceMetric m) const
{
    Q_D(const QWidget);
    int val;
    if (m == PdmWidth) {
        val = data->crect.width();
    } else if (m == PdmHeight) {
        val = data->crect.height();
    } else {
        CWsScreenDevice *scr = S60->screenDevice();
        switch(m) {
        case PdmDpiX:
        case PdmPhysicalDpiX:
            if (d->extra && d->extra->customDpiX) {
                val = d->extra->customDpiX;
            } else {
                const QWidgetPrivate *p = d;
                while (p->parent) {
                    p = static_cast<const QWidget *>(p->parent)->d_func();
                    if (p->extra && p->extra->customDpiX) {
                        val = p->extra->customDpiX;
                        break;
                    }
                }
                if (p == d || !(p->extra && p->extra->customDpiX))
                    val = S60->defaultDpiX;
            }
            break;
        case PdmDpiY:
        case PdmPhysicalDpiY:
            if (d->extra && d->extra->customDpiY) {
                val = d->extra->customDpiY;
            } else {
                const QWidgetPrivate *p = d;
                while (p->parent) {
                    p = static_cast<const QWidget *>(p->parent)->d_func();
                    if (p->extra && p->extra->customDpiY) {
                        val = p->extra->customDpiY;
                        break;
                    }
                }
                if (p == d || !(p->extra && p->extra->customDpiY))
                    val = S60->defaultDpiY;
            }
            break;
        case PdmWidthMM:
        {
            TInt twips = scr->HorizontalPixelsToTwips(data->crect.width());
            val = (int)(twips * (25.4/KTwipsPerInch));
            break;
        }
        case PdmHeightMM:
        {
            TInt twips = scr->VerticalPixelsToTwips(data->crect.height());
            val = (int)(twips * (25.4/KTwipsPerInch));
            break;
        }
        case PdmNumColors:
            val = TDisplayModeUtils::NumDisplayModeColors(scr->DisplayMode());
            break;
        case PdmDepth:
            val = TDisplayModeUtils::NumDisplayModeBitsPerPixel(scr->DisplayMode());
            break;
        default:
            val = 0;
            qWarning("QWidget::metric: Invalid metric command");
        }
    }
    return val;
}

QPaintEngine *QWidget::paintEngine() const
{
    return 0;
}

QPoint QWidget::mapToGlobal(const QPoint &pos) const
{
    Q_D(const QWidget);
    if (!testAttribute(Qt::WA_WState_Created) || !internalWinId()) {

        QPoint p = pos + data->crect.topLeft();
        return (isWindow() || !parentWidget()) ?  p : parentWidget()->mapToGlobal(p);

    } else if ((d->data.window_flags & Qt::Window) && internalWinId()) { //toplevel
        QPoint tp = geometry().topLeft();
        return pos + tp;
    }

    // Native window case
    const TPoint widgetScreenOffset = internalWinId()->PositionRelativeToScreen();
    const QPoint globalPos = QPoint(widgetScreenOffset.iX, widgetScreenOffset.iY) + pos;
    return globalPos;
}

QPoint QWidget::mapFromGlobal(const QPoint &pos) const
{
    Q_D(const QWidget);
    if (!testAttribute(Qt::WA_WState_Created) || !internalWinId()) {
        QPoint p = (isWindow() || !parentWidget()) ?  pos : parentWidget()->mapFromGlobal(pos);
        return p - data->crect.topLeft();
    } else if ((d->data.window_flags & Qt::Window) && internalWinId()) { //toplevel
        QPoint tp = geometry().topLeft();
        return pos - tp;
    }

    // Native window case
    const TPoint widgetScreenOffset = internalWinId()->PositionRelativeToScreen();
    const QPoint widgetPos = pos - QPoint(widgetScreenOffset.iX, widgetScreenOffset.iY);
    return widgetPos;
}

void QWidget::setWindowState(Qt::WindowStates newstate)
{
    Q_D(QWidget);

    Qt::WindowStates oldstate = windowState();
    if (oldstate == newstate)
        return;

    if (isWindow()) {
#ifdef Q_WS_S60
        // Change window decoration visibility if switching to or from fullsccreen
        // In addition decoration visibility is changed when the initial has been
        // WindowNoState.
        // The window decoration visibility has to be changed before doing actual
        // window state change since in that order the availableGeometry will return
        // directly the right size and we will avoid unnecessarty redraws
        if ((oldstate & Qt::WindowFullScreen) != (newstate & Qt::WindowFullScreen) ||
            oldstate == Qt::WindowNoState) {
            CEikStatusPane* statusPane = S60->statusPane();
            CEikButtonGroupContainer* buttonGroup = S60->buttonGroupContainer();
            if (newstate & Qt::WindowFullScreen) {
                if (statusPane)
                    statusPane->MakeVisible(false);
                if (buttonGroup)
                    buttonGroup->MakeVisible(false);
            } else {
                if (statusPane)
                    statusPane->MakeVisible(true);
                if (buttonGroup)
                    buttonGroup->MakeVisible(true);
            }

        }
#endif // Q_WS_S60

        createWinId();
        Q_ASSERT(testAttribute(Qt::WA_WState_Created));
        QTLWExtra *top = d->topData();

        // Ensure the initial size is valid, since we store it as normalGeometry below.
        if (!testAttribute(Qt::WA_Resized) && !isVisible())
            adjustSize();

        if ((oldstate & Qt::WindowMaximized) != (newstate & Qt::WindowMaximized)) {
            if ((newstate & Qt::WindowMaximized)) {
                const QRect normalGeometry = geometry();

                const QRect r = top->normalGeometry;
                setGeometry(qApp->desktop()->availableGeometry(this));
                top->normalGeometry = r;

                if (top->normalGeometry.width() < 0)
                    top->normalGeometry = normalGeometry;
            } else {
                // restore original geometry
                setGeometry(top->normalGeometry);
            }
        }
        if ((oldstate & Qt::WindowFullScreen) != (newstate & Qt::WindowFullScreen)) {
            if (newstate & Qt::WindowFullScreen) {
                const QRect normalGeometry = geometry();
                const QRect r = top->normalGeometry;
                setGeometry(qApp->desktop()->screenGeometry(this));

                top->normalGeometry = r;
                if (top->normalGeometry.width() < 0)
                    top->normalGeometry = normalGeometry;
            } else {
                if (newstate & Qt::WindowMaximized) {
                    const QRect r = top->normalGeometry;
                    setGeometry(qApp->desktop()->availableGeometry(this));
                    top->normalGeometry = r;
                } else {
                    setGeometry(top->normalGeometry);
                }
            }
        }
        if ((oldstate & Qt::WindowMinimized) != (newstate & Qt::WindowMinimized)) {
            if (newstate & Qt::WindowMinimized) {
                if (isVisible()) {
                    QSymbianControl *id = static_cast<QSymbianControl *>(effectiveWinId());
                    if (id->IsFocused()) // Avoid unnecessary calls to FocusChanged()
                        id->setFocusSafely(false);
                    id->MakeVisible(false);
                }
            } else {
                if (isVisible()) {
                    QSymbianControl *id = static_cast<QSymbianControl *>(effectiveWinId());
                    id->MakeVisible(true);
                    if (!id->IsFocused()) // Avoid unnecessary calls to FocusChanged()
                        id->setFocusSafely(true);
                }
                const QRect normalGeometry = geometry();
                const QRect r = top->normalGeometry;
                top->normalGeometry = r;
                if (top->normalGeometry.width() < 0)
                    top->normalGeometry = normalGeometry;
            }
        }
    }

    data->window_state = newstate;

    if (newstate & Qt::WindowActive)
        activateWindow();

    QWindowStateChangeEvent e(oldstate);
    QApplication::sendEvent(this, &e);
}


void QWidget::destroy(bool destroyWindow, bool destroySubWindows)
{
    Q_D(QWidget);
    if (!isWindow() && parentWidget())
        parentWidget()->d_func()->invalidateBuffer(geometry());
    d->deactivateWidgetCleanup();
    QSymbianControl *id = static_cast<QSymbianControl *>(internalWinId());
    if (testAttribute(Qt::WA_WState_Created)) {

#ifndef QT_NO_IM
        if (d->ic) {
            delete d->ic;
        } else {
            QInputContext *ic = inputContext();
            if (ic) {
                ic->widgetDestroyed(this);
            }
        }
#endif

        if (QWidgetPrivate::mouseGrabber == this)
            releaseMouse();
        if (QWidgetPrivate::keyboardGrabber == this)
            releaseKeyboard();
        setAttribute(Qt::WA_WState_Created, false);
        QObjectList childList = children();
        for (int i = 0; i < childList.size(); ++i) { // destroy all widget children
            register QObject *obj = childList.at(i);
            if (obj->isWidgetType())
                static_cast<QWidget*>(obj)->destroy(destroySubWindows,
                                                    destroySubWindows);
        }
        if (destroyWindow && !(windowType() == Qt::Desktop) && id) {
            if (id->IsFocused()) // Avoid unnecessry calls to FocusChanged()
                id->setFocusSafely(false);
            id->ControlEnv()->AppUi()->RemoveFromStack(id);

            // Hack to activate window under destroyed one. With this activation
            // the next visible window will get keyboard focus
            WId wid = CEikonEnv::Static()->AppUi()->TopFocusedControl();
            if (wid) {
                QWidget *widget = QWidget::find(wid);
                QApplication::setActiveWindow(widget);
                if (widget) {
                    // Reset global window title for focusing window
                    widget->d_func()->setWindowTitle_sys(widget->windowTitle());
                }
            }
        }
    }

    QT_TRY {
        d->setWinId(0);
    } QT_CATCH (const std::bad_alloc &) {
        // swallow - destructors must not throw
    }

    if (destroyWindow) {
        delete id;
    }
}

QWidget *QWidget::mouseGrabber()
{
    return QWidgetPrivate::mouseGrabber;
}

QWidget *QWidget::keyboardGrabber()
{
    return QWidgetPrivate::keyboardGrabber;
}

void QWidget::grabKeyboard()
{
    if (!qt_nograb()) {
        if (QWidgetPrivate::keyboardGrabber && QWidgetPrivate::keyboardGrabber != this)
            QWidgetPrivate::keyboardGrabber->releaseKeyboard();

        // ### TODO: Native keyboard grab

        QWidgetPrivate::keyboardGrabber = this;
    }
}

void QWidget::releaseKeyboard()
{
    if (!qt_nograb() && QWidgetPrivate::keyboardGrabber == this) {
        // ### TODO: Native keyboard release
        QWidgetPrivate::keyboardGrabber = 0;
    }
}

void QWidget::grabMouse()
{
    if (isVisible() && !qt_nograb()) {
        if (QWidgetPrivate::mouseGrabber && QWidgetPrivate::mouseGrabber != this)
            QWidgetPrivate::mouseGrabber->releaseMouse();
        Q_ASSERT(testAttribute(Qt::WA_WState_Created));
        WId id = effectiveWinId();
        id->SetPointerCapture(true);
        QWidgetPrivate::mouseGrabber = this;

#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(cursor());
#endif
    }
}

#ifndef QT_NO_CURSOR
void QWidget::grabMouse(const QCursor &cursor)
{
    if (isVisible() && !qt_nograb()) {
        if (QWidgetPrivate::mouseGrabber && QWidgetPrivate::mouseGrabber != this)
            QWidgetPrivate::mouseGrabber->releaseMouse();
        Q_ASSERT(testAttribute(Qt::WA_WState_Created));
        WId id = effectiveWinId();
        id->SetPointerCapture(true);
        QWidgetPrivate::mouseGrabber = this;

        QApplication::setOverrideCursor(cursor);
    }
}
#endif

void QWidget::releaseMouse()
{
    if (!qt_nograb() && QWidgetPrivate::mouseGrabber == this) {
        Q_ASSERT(testAttribute(Qt::WA_WState_Created));
        WId id = effectiveWinId();
        id->SetPointerCapture(false);
        QWidgetPrivate::mouseGrabber = 0;
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
    }
}

void QWidget::activateWindow()
{
    Q_D(QWidget);

    QWidget *tlw = window();
    if (tlw->isVisible()) {
        window()->createWinId();
        QSymbianControl *id = static_cast<QSymbianControl *>(tlw->internalWinId());
        id->setFocusSafely(true);
    }
}

#ifndef QT_NO_CURSOR

void QWidgetPrivate::setCursor_sys(const QCursor &cursor)
{
    Q_UNUSED(cursor);
    Q_Q(QWidget);
    qt_symbian_set_cursor(q, false);
}

void QWidgetPrivate::unsetCursor_sys()
{
    Q_Q(QWidget);
    qt_symbian_set_cursor(q, false);
}
#endif

QT_END_NAMESPACE
