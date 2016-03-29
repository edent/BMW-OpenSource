/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ancestormovemonitor.h"
#include "utils.h"
#include "videooutput.h"
#include "videooutputobserver.h"

#ifndef QT_NO_DEBUG
#include "objectdump.h"
#endif

#include <QPaintEvent>
#include <QPainter>
#include <QMoveEvent>
#include <QResizeEvent>

#include <QtGui/private/qwidget_p.h> // to access QWExtra

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::VideoOutput
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoOutput::VideoOutput
    (AncestorMoveMonitor* ancestorMoveMonitor, QWidget* parent)
        :   QWidget(parent)
        ,   m_ancestorMoveMonitor(ancestorMoveMonitor)
        ,   m_observer(0)
{
    TRACE_CONTEXT(VideoOutput::VideoOutput, EVideoInternal);
    TRACE_ENTRY("parent 0x%08x", parent);

    setPalette(QPalette(Qt::black));
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);

    // Causes QSymbianControl::Draw not to BitBlt this widget's region of the
    // backing store.  Since the backing store is (by default) a 16MU bitmap,
    // blitting it results in this widget's screen region in the final
    // framebuffer having opaque alpha values.  This in turn causes the video
    // to be invisible when running on the target device.
    qt_widget_private(this)->extraData()->disableBlit = true;

    registerForAncestorMoved();

    dump();

    TRACE_EXIT_0();
}

MMF::VideoOutput::~VideoOutput()
{
    TRACE_CONTEXT(VideoOutput::~VideoOutput, EVideoInternal);
    TRACE_ENTRY_0();

    m_ancestorMoveMonitor->unRegisterTarget(this);

    TRACE_EXIT_0();
}

void MMF::VideoOutput::setFrameSize(const QSize& frameSize)
{
    TRACE_CONTEXT(VideoOutput::setFrameSize, EVideoInternal);
    TRACE("oldSize %d %d newSize %d %d",
          m_frameSize.width(), m_frameSize.height(),
          frameSize.width(), frameSize.height());

    if (frameSize != m_frameSize) {
        m_frameSize = frameSize;
        updateGeometry();
    }
}

void MMF::VideoOutput::setObserver(VideoOutputObserver* observer)
{
    TRACE_CONTEXT(VideoOutput::setObserver, EVideoInternal);
    TRACE("observer 0x%08x", observer);

    m_observer = observer;
}

void MMF::VideoOutput::ancestorMoved()
{
    TRACE_CONTEXT(VideoOutput::ancestorMoved, EVideoInternal);
    TRACE_ENTRY_0();

    videoOutputRegionChanged();

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// QWidget
//-----------------------------------------------------------------------------

QSize MMF::VideoOutput::sizeHint() const
{
    // TODO: replace this with a more sensible default
    QSize result(320, 240);

    if (!m_frameSize.isNull()) {
        result = m_frameSize;
    }

    return result;
}

void MMF::VideoOutput::paintEvent(QPaintEvent* event)
{
    TRACE_CONTEXT(VideoOutput::paintEvent, EVideoInternal);
    TRACE("rect %d %d - %d %d",
          event->rect().left(), event->rect().top(),
          event->rect().right(), event->rect().bottom());
    TRACE("regions %d", event->region().rectCount());
    TRACE("type %d", event->type());

    // Do nothing
}

void MMF::VideoOutput::resizeEvent(QResizeEvent* event)
{
    TRACE_CONTEXT(VideoOutput::resizeEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldSize().width(), event->oldSize().height(),
          event->size().width(), event->size().height());

    videoOutputRegionChanged();
}

void MMF::VideoOutput::moveEvent(QMoveEvent* event)
{
    TRACE_CONTEXT(VideoOutput::moveEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldPos().x(), event->oldPos().y(),
          event->pos().x(), event->pos().y());

    videoOutputRegionChanged();
}

bool MMF::VideoOutput::event(QEvent* event)
{
    TRACE_CONTEXT(VideoOutput::event, EVideoInternal);

    if (event->type() == QEvent::WinIdChange) {
        TRACE_0("WinIdChange");
        videoOutputRegionChanged();
        return true;
    } else if (event->type() == QEvent::ParentChange) {
        // Tell ancestor move monitor to update ancestor list for this widget
        registerForAncestorMoved();
        return true;
    } else
        return QWidget::event(event);
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::VideoOutput::videoOutputRegionChanged()
{
    dump();
    if (m_observer)
        m_observer->videoOutputRegionChanged();
}

void MMF::VideoOutput::registerForAncestorMoved()
{
    m_ancestorMoveMonitor->registerTarget(this);
}

void MMF::VideoOutput::dump() const
{
#ifndef QT_NO_DEBUG
    TRACE_CONTEXT(VideoOutput::dump, EVideoInternal);
    QScopedPointer<ObjectDump::QVisitor> visitor(new ObjectDump::QVisitor);
    visitor->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*visitor);
    TRACE("Dumping tree from leaf 0x%08x:", this);
    //ObjectDump::dumpAncestors(*this, *visitor);
    ObjectDump::dumpTreeFromLeaf(*this, *visitor);
#endif
}


QT_END_NAMESPACE

