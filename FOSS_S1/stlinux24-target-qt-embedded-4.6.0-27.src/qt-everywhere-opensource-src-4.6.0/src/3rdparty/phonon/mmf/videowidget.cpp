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

#include "mediaobject.h"
#include "utils.h"
#include "videooutput.h"

#include "videowidget.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::VideoWidget
  \internal
*/

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

static const Phonon::VideoWidget::AspectRatio DefaultAspectRatio =
    Phonon::VideoWidget::AspectRatioAuto;
static const qreal DefaultBrightness = 1.0;
static const Phonon::VideoWidget::ScaleMode DefaultScaleMode =
    Phonon::VideoWidget::FitInView;
static const qreal DefaultContrast = 1.0;
static const qreal DefaultHue = 1.0;
static const qreal DefaultSaturation = 1.0;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoWidget::VideoWidget
    (AncestorMoveMonitor* ancestorMoveMonitor, QWidget* parent)
        :   MediaNode(parent)
        ,   m_widget(new VideoOutput(ancestorMoveMonitor, parent))
        ,   m_aspectRatio(DefaultAspectRatio)
        ,   m_brightness(DefaultBrightness)
        ,   m_scaleMode(DefaultScaleMode)
        ,   m_contrast(DefaultContrast)
        ,   m_hue(DefaultHue)
        ,   m_saturation(DefaultSaturation)
{
    TRACE_CONTEXT(VideoWidget::VideoWidget, EVideoApi);
    TRACE_ENTRY_0();

    TRACE_EXIT_0();
}

MMF::VideoWidget::~VideoWidget()
{
    TRACE_CONTEXT(VideoWidget::~VideoWidget, EVideoApi);
    TRACE_ENTRY_0();

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// VideoWidgetInterface
//-----------------------------------------------------------------------------

Phonon::VideoWidget::AspectRatio MMF::VideoWidget::aspectRatio() const
{
    return m_aspectRatio;
}

void MMF::VideoWidget::setAspectRatio
(Phonon::VideoWidget::AspectRatio aspectRatio)
{
    TRACE_CONTEXT(VideoWidget::setAspectRatio, EVideoApi);
    TRACE("aspectRatio %d", aspectRatio);

    m_aspectRatio = aspectRatio;
}

qreal MMF::VideoWidget::brightness() const
{
    return m_brightness;
}

void MMF::VideoWidget::setBrightness(qreal brightness)
{
    TRACE_CONTEXT(VideoWidget::setBrightness, EVideoApi);
    TRACE("brightness %f", brightness);

    m_brightness = brightness;
}

Phonon::VideoWidget::ScaleMode MMF::VideoWidget::scaleMode() const
{
    return m_scaleMode;
}

void MMF::VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
{
    TRACE_CONTEXT(VideoWidget::setScaleMode, EVideoApi);
    TRACE("setScaleMode %d", setScaleMode);

    m_scaleMode = scaleMode;
}

qreal MMF::VideoWidget::contrast() const
{
    return m_contrast;
}

void MMF::VideoWidget::setContrast(qreal contrast)
{
    TRACE_CONTEXT(VideoWidget::setContrast, EVideoApi);
    TRACE("contrast %f", contrast);

    m_contrast = contrast;
}

qreal MMF::VideoWidget::hue() const
{
    return m_hue;
}

void MMF::VideoWidget::setHue(qreal hue)
{
    TRACE_CONTEXT(VideoWidget::setHue, EVideoApi);
    TRACE("hue %f", hue);

    m_hue = hue;
}

qreal MMF::VideoWidget::saturation() const
{
    return m_saturation;
}

void MMF::VideoWidget::setSaturation(qreal saturation)
{
    TRACE_CONTEXT(VideoWidget::setSaturation, EVideoApi);
    TRACE("saturation %f", saturation);

    m_saturation = saturation;
}

QWidget* MMF::VideoWidget::widget()
{
    return m_widget.data();
}

VideoOutput& MMF::VideoWidget::videoOutput()
{
    return *static_cast<VideoOutput*>(widget());
}

bool MMF::VideoWidget::activateOnMediaObject(MediaObject *mo)
{
    mo->setVideoOutput(&videoOutput());
    return true;
}

QT_END_NAMESPACE

