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

#include "qs60style_p.h"

#include "qapplication.h"
#include "qpainter.h"
#include "qstyleoption.h"
#include "qevent.h"
#include "qpixmapcache.h"

#include "qcalendarwidget.h"
#include "qdial.h"
#include "qdialog.h"
#include "qgroupbox.h"
#include "qheaderview.h"
#include "qlist.h"
#include "qlistwidget.h"
#include "qlistview.h"
#include "qmenu.h"
#include "qmenubar.h"
#include "qpushbutton.h"
#include "qscrollarea.h"
#include "qscrollbar.h"
#include "qtabbar.h"
#include "qtablewidget.h"
#include "qtableview.h"
#include "qtextedit.h"
#include "qtoolbar.h"
#include "qtoolbutton.h"
#include "qfocusframe.h"

#include "private/qtoolbarextension_p.h"
#include "private/qcombobox_p.h"
#include "private/qwidget_p.h"
#include "private/qapplication_p.h"

#if !defined(QT_NO_STYLE_S60) || defined(QT_PLUGIN)

QT_BEGIN_NAMESPACE

// from text/qfont.cpp
extern Q_GUI_EXPORT int qt_defaultDpiY();

const QS60StylePrivate::SkinElementFlags QS60StylePrivate::KDefaultSkinElementFlags =
    SkinElementFlags(SF_PointNorth | SF_StateEnabled);

static const QByteArray propertyKeyLayouts = "layouts";
static const QByteArray propertyKeyCurrentlayout = "currentlayout";

static const qreal goldenRatio = 1.618;

const layoutHeader QS60StylePrivate::m_layoutHeaders[] = {
// *** generated layout data ***
{240,320,1,15,true,"QVGA Landscape Mirrored"},
{240,320,1,15,false,"QVGA Landscape"},
{320,240,1,15,true,"QVGA Portrait Mirrored"},
{320,240,1,15,false,"QVGA Portrait"},
{360,640,1,15,true,"NHD Landscape Mirrored"},
{360,640,1,15,false,"NHD Landscape"},
{640,360,1,15,true,"NHD Portrait Mirrored"},
{640,360,1,15,false,"NHD Portrait"},
{352,800,1,12,true,"E90 Landscape Mirrored"},
{352,800,1,12,false,"E90 Landscape"}
// *** End of generated data ***
};
const int QS60StylePrivate::m_numberOfLayouts =
    (int)sizeof(QS60StylePrivate::m_layoutHeaders)/sizeof(QS60StylePrivate::m_layoutHeaders[0]);

const short QS60StylePrivate::data[][MAX_PIXELMETRICS] = {
// *** generated pixel metrics ***
{5,0,-909,0,0,2,0,0,-1,7,12,19,13,13,6,200,-909,-909,-909,20,13,2,0,0,21,7,18,-909,3,3,1,-909,-909,0,1,0,0,12,20,15,15,18,18,1,115,18,0,-909,-909,-909,-909,0,0,16,2,-909,0,0,-909,16,-909,-909,-909,-909,32,18,55,24,55,3,3,4,9,13,-909,5,51,11,5,0,6,3,3,8,3,3,-909,2,-909,-909,-909,-909,5,5,3,1},
{5,0,-909,0,0,2,0,0,-1,7,12,19,13,13,6,200,-909,-909,-909,20,13,2,0,0,21,7,18,-909,3,3,1,-909,-909,0,1,0,0,12,20,15,15,18,18,1,115,18,0,-909,-909,-909,-909,0,0,16,2,-909,0,0,-909,16,-909,-909,-909,-909,32,18,55,24,55,3,3,4,9,13,-909,5,51,11,5,0,3,3,6,8,3,3,-909,2,-909,-909,-909,-909,5,5,3,1},
{5,0,-909,0,0,1,0,0,-1,8,14,22,15,15,7,164,-909,-909,-909,19,15,2,0,0,21,8,27,-909,4,4,1,-909,-909,0,7,6,0,13,23,17,17,21,21,7,115,21,0,-909,-909,-909,-909,0,0,15,1,-909,0,0,-909,15,-909,-909,-909,-909,32,21,65,27,65,4,4,5,10,15,-909,5,58,13,5,0,7,4,4,9,4,4,-909,2,-909,-909,-909,-909,6,6,3,1},
{5,0,-909,0,0,1,0,0,-1,8,14,22,15,15,7,164,-909,-909,-909,19,15,2,0,0,21,8,27,-909,4,4,1,-909,-909,0,7,6,0,13,23,17,17,21,21,7,115,21,0,-909,-909,-909,-909,0,0,15,1,-909,0,0,-909,15,-909,-909,-909,-909,32,21,65,27,65,4,4,5,10,15,-909,5,58,13,5,0,4,4,7,9,4,4,-909,2,-909,-909,-909,-909,6,6,3,1},
{7,0,-909,0,0,2,0,0,-1,25,69,28,19,19,9,258,-909,-909,-909,23,19,26,0,0,32,25,72,-909,5,5,2,-909,-909,0,7,21,0,17,29,22,22,27,27,7,173,29,0,-909,-909,-909,-909,0,0,25,2,-909,0,0,-909,25,-909,-909,-909,-909,87,27,77,35,77,5,5,6,8,19,-909,7,74,19,7,0,8,5,5,12,5,5,-909,3,-909,-909,-909,-909,7,7,3,1},
{7,0,-909,0,0,2,0,0,-1,25,69,28,19,19,9,258,-909,-909,-909,23,19,26,0,0,32,25,72,-909,5,5,2,-909,-909,0,7,21,0,17,29,22,22,27,27,7,173,29,0,-909,-909,-909,-909,0,0,25,2,-909,0,0,-909,25,-909,-909,-909,-909,87,27,77,35,77,5,5,6,8,19,-909,7,74,19,7,0,5,5,8,12,5,5,-909,3,-909,-909,-909,-909,7,7,3,1},
{7,0,-909,0,0,2,0,0,-1,25,68,28,19,19,9,258,-909,-909,-909,31,19,6,0,0,32,25,60,-909,5,5,2,-909,-909,0,7,32,0,17,29,22,22,27,27,7,173,29,0,-909,-909,-909,-909,0,0,26,2,-909,0,0,-909,26,-909,-909,-909,-909,87,27,96,35,96,5,5,6,8,19,-909,7,74,22,7,0,8,5,5,12,5,5,-909,3,-909,-909,-909,-909,7,7,3,1},
{7,0,-909,0,0,2,0,0,-1,25,68,28,19,19,9,258,-909,-909,-909,31,19,6,0,0,32,25,60,-909,5,5,2,-909,-909,0,7,32,0,17,29,22,22,27,27,7,173,29,0,-909,-909,-909,-909,0,0,26,2,-909,0,0,-909,26,-909,-909,-909,-909,87,27,96,35,96,5,5,6,8,19,-909,7,74,22,7,0,5,5,8,12,5,5,-909,3,-909,-909,-909,-909,7,7,3,1},
{7,0,-909,0,0,2,0,0,-1,10,20,27,18,18,9,301,-909,-909,-909,29,18,5,0,0,35,7,32,-909,5,5,2,-909,-909,0,2,8,0,16,28,21,21,26,26,2,170,26,0,-909,-909,-909,-909,0,0,21,5,-909,0,0,-909,-909,-909,-909,-909,-909,54,26,265,34,265,5,5,6,3,18,-909,7,72,19,7,0,8,6,5,11,6,5,-909,2,-909,-909,-909,-909,5,5,3,1},
{7,0,-909,0,0,2,0,0,-1,10,20,27,18,18,9,301,-909,-909,-909,29,18,5,0,0,35,7,32,-909,5,5,2,-909,-909,0,2,8,0,16,28,21,21,26,26,2,170,26,0,-909,-909,-909,-909,0,0,21,6,-909,0,0,-909,-909,-909,-909,-909,-909,54,26,265,34,265,5,5,6,3,18,-909,7,72,19,7,0,5,6,8,11,6,5,-909,2,-909,-909,-909,-909,5,5,3,1}
// *** End of generated data ***
};

const short *QS60StylePrivate::m_pmPointer = QS60StylePrivate::data[0];

// theme background texture
QPixmap *QS60StylePrivate::m_background = 0;

// theme palette
QPalette *QS60StylePrivate::m_themePalette = 0;

const struct QS60StylePrivate::frameElementCenter QS60StylePrivate::m_frameElementsData[] = {
    {SE_ButtonNormal,           QS60StyleEnums::SP_QsnFrButtonTbCenter},
    {SE_ButtonPressed,          QS60StyleEnums::SP_QsnFrButtonTbCenterPressed},
    {SE_FrameLineEdit,          QS60StyleEnums::SP_QsnFrInputCenter},
    {SE_ListHighlight,          QS60StyleEnums::SP_QsnFrListCenter},
    {SE_OptionsMenu,            QS60StyleEnums::SP_QsnFrPopupCenter},
    {SE_SettingsList,           QS60StyleEnums::SP_QsnFrSetOptCenter},
    {SE_TableItem,              QS60StyleEnums::SP_QsnFrCaleCenter},
    {SE_TableHeaderItem,        QS60StyleEnums::SP_QsnFrCaleHeadingCenter},
    {SE_ToolTip,                QS60StyleEnums::SP_QsnFrPopupPreviewCenter},
    {SE_ToolBar,                QS60StyleEnums::SP_QsnFrPopupSubCenter},
    {SE_ToolBarButton,          QS60StyleEnums::SP_QsnFrSctrlButtonCenter},
    {SE_ToolBarButtonPressed,   QS60StyleEnums::SP_QsnFrSctrlButtonCenterPressed},
    {SE_PanelBackground,        QS60StyleEnums::SP_QsnFrSetOptCenter},
    {SE_ButtonInactive,         QS60StyleEnums::SP_QsnFrButtonCenterInactive},
    {SE_Editor,                 QS60StyleEnums::SP_QsnFrNotepadCenter},
};

static const int frameElementsCount =
    int(sizeof(QS60StylePrivate::m_frameElementsData)/sizeof(QS60StylePrivate::m_frameElementsData[0]));

const int KNotFound = -909;
const double KTabFontMul = 0.72;

QS60StylePrivate::~QS60StylePrivate()
{
    clearCaches(); //deletes also background image
    deleteThemePalette();
}

void QS60StylePrivate::drawSkinElement(SkinElements element, QPainter *painter,
    const QRect &rect, SkinElementFlags flags)
{
    switch (element) {
    case SE_ButtonNormal:
        drawFrame(SF_ButtonNormal, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ButtonPressed:
        drawFrame(SF_ButtonPressed, painter, rect, flags | SF_PointNorth);
        break;
    case SE_FrameLineEdit:
        drawFrame(SF_FrameLineEdit, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ProgressBarGrooveHorizontal:
        drawRow(QS60StyleEnums::SP_QgnGrafBarFrameSideL, QS60StyleEnums::SP_QgnGrafBarFrameCenter,
            QS60StyleEnums::SP_QgnGrafBarFrameSideR, Qt::Horizontal, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ProgressBarGrooveVertical:
        drawRow(QS60StyleEnums::SP_QgnGrafBarFrameSideL, QS60StyleEnums::SP_QgnGrafBarFrameCenter,
            QS60StyleEnums::SP_QgnGrafBarFrameSideR, Qt::Vertical, painter, rect, flags | SF_PointEast);
        break;
    case SE_ProgressBarIndicatorHorizontal:
        drawPart(QS60StyleEnums::SP_QgnGrafBarProgress, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ProgressBarIndicatorVertical:
        drawPart(QS60StyleEnums::SP_QgnGrafBarProgress, painter, rect, flags | SF_PointWest);
        break;
    case SE_ScrollBarGrooveHorizontal:
        drawRow(QS60StyleEnums::SP_QsnCpScrollBgBottom, QS60StyleEnums::SP_QsnCpScrollBgMiddle,
            QS60StyleEnums::SP_QsnCpScrollBgTop, Qt::Horizontal, painter, rect, flags | SF_PointEast);
        break;
    case SE_ScrollBarGrooveVertical:
        drawRow(QS60StyleEnums::SP_QsnCpScrollBgTop, QS60StyleEnums::SP_QsnCpScrollBgMiddle,
            QS60StyleEnums::SP_QsnCpScrollBgBottom, Qt::Vertical, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ScrollBarHandleHorizontal:
        drawRow(QS60StyleEnums::SP_QsnCpScrollHandleBottom, QS60StyleEnums::SP_QsnCpScrollHandleMiddle,
            QS60StyleEnums::SP_QsnCpScrollHandleTop, Qt::Horizontal, painter, rect, flags | SF_PointEast);
        break;
    case SE_ScrollBarHandleVertical:
        drawRow(QS60StyleEnums::SP_QsnCpScrollHandleTop, QS60StyleEnums::SP_QsnCpScrollHandleMiddle,
            QS60StyleEnums::SP_QsnCpScrollHandleBottom, Qt::Vertical, painter, rect, flags | SF_PointNorth);
        break;
    case SE_SliderHandleHorizontal:
        drawPart(QS60StyleEnums::SP_QgnIndiSliderEdit, painter, rect, flags | SF_PointNorth);
        break;
    case SE_SliderHandleVertical:
        drawPart(QS60StyleEnums::SP_QgnIndiSliderEdit, painter, rect, flags | SF_PointEast);
        break;
    case SE_TabBarTabEastActive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabActiveL, QS60StyleEnums::SP_QgnGrafTabActiveM,
            QS60StyleEnums::SP_QgnGrafTabActiveR, Qt::Vertical, painter, rect, flags | SF_PointEast);
        break;
    case SE_TabBarTabEastInactive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabPassiveL, QS60StyleEnums::SP_QgnGrafTabPassiveM,
            QS60StyleEnums::SP_QgnGrafTabPassiveR, Qt::Vertical, painter, rect, flags | SF_PointEast);
        break;
    case SE_TabBarTabNorthActive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabActiveL, QS60StyleEnums::SP_QgnGrafTabActiveM,
            QS60StyleEnums::SP_QgnGrafTabActiveR, Qt::Horizontal, painter, rect, flags | SF_PointNorth);
        break;
    case SE_TabBarTabNorthInactive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabPassiveL, QS60StyleEnums::SP_QgnGrafTabPassiveM,
            QS60StyleEnums::SP_QgnGrafTabPassiveR, Qt::Horizontal, painter, rect, flags | SF_PointNorth);
        break;
    case SE_TabBarTabSouthActive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabActiveR, QS60StyleEnums::SP_QgnGrafTabActiveM,
            QS60StyleEnums::SP_QgnGrafTabActiveL, Qt::Horizontal, painter, rect, flags | SF_PointSouth);
        break;
    case SE_TabBarTabSouthInactive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabPassiveR, QS60StyleEnums::SP_QgnGrafTabPassiveM,
            QS60StyleEnums::SP_QgnGrafTabPassiveL, Qt::Horizontal, painter, rect, flags | SF_PointSouth);
        break;
    case SE_TabBarTabWestActive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabActiveR, QS60StyleEnums::SP_QgnGrafTabActiveM,
            QS60StyleEnums::SP_QgnGrafTabActiveL, Qt::Vertical, painter, rect, flags | SF_PointWest);
        break;
    case SE_TabBarTabWestInactive:
        drawRow(QS60StyleEnums::SP_QgnGrafTabPassiveR, QS60StyleEnums::SP_QgnGrafTabPassiveM,
            QS60StyleEnums::SP_QgnGrafTabPassiveL, Qt::Vertical, painter, rect, flags | SF_PointWest);
        break;
    case SE_ListHighlight:
        drawFrame(SF_ListHighlight, painter, rect, flags | SF_PointNorth);
        break;
    case SE_OptionsMenu:
        drawFrame(SF_OptionsMenu, painter, rect, flags | SF_PointNorth);
        break;
    case SE_SettingsList:
        drawFrame(SF_SettingsList, painter, rect, flags | SF_PointNorth);
        break;
    case SE_TableItem:
        drawFrame(SF_TableItem, painter, rect, flags | SF_PointNorth);
        break;
    case SE_TableHeaderItem:
        drawFrame(SF_TableHeaderItem, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ToolTip:
        drawFrame(SF_ToolTip, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ToolBar:
        drawFrame(SF_ToolBar, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ToolBarButton:
        drawFrame(SF_ToolBarButton, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ToolBarButtonPressed:
        drawFrame(SF_ToolBarButtonPressed, painter, rect, flags | SF_PointNorth);
        break;
    case SE_PanelBackground:
        drawFrame(SF_PanelBackground, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ScrollBarHandlePressedHorizontal:
        drawRow(QS60StyleEnums::SP_QsnCpScrollHandleBottomPressed, QS60StyleEnums::SP_QsnCpScrollHandleMiddlePressed,
            QS60StyleEnums::SP_QsnCpScrollHandleTopPressed, Qt::Horizontal, painter, rect, flags | SF_PointEast);
        break;
    case SE_ScrollBarHandlePressedVertical:
        drawRow(QS60StyleEnums::SP_QsnCpScrollHandleTopPressed, QS60StyleEnums::SP_QsnCpScrollHandleMiddlePressed,
            QS60StyleEnums::SP_QsnCpScrollHandleBottomPressed, Qt::Vertical, painter, rect, flags | SF_PointNorth);
        break;
    case SE_ButtonInactive:
        drawFrame(SF_ButtonInactive, painter, rect, flags | SF_PointNorth);
        break;
    case SE_Editor:
        drawFrame(SF_FrameLineEdit, painter, rect, flags | SF_PointNorth);
        break;
    default:
        break;
    }
}

void QS60StylePrivate::drawSkinPart(QS60StyleEnums::SkinParts part,
    QPainter *painter, const QRect &rect, SkinElementFlags flags)
{
    drawPart(part, painter, rect, flags);
}

short QS60StylePrivate::pixelMetric(int metric)
{
    Q_ASSERT(metric < MAX_PIXELMETRICS);
    const short returnValue = m_pmPointer[metric];
    return returnValue;
}

QColor QS60StylePrivate::stateColor(const QColor &color, const QStyleOption *option)
{
    QColor retColor (color);
    if (option && !(option->state & QStyle::State_Enabled)) {
        QColor hsvColor = retColor.toHsv();
        int colorSat = hsvColor.saturation();
        int colorVal = hsvColor.value();
        colorSat = (colorSat!=0) ? (colorSat>>1) : 128;
        colorVal = (colorVal!=0) ? (colorVal>>1) : 128;
        hsvColor.setHsv(hsvColor.hue(), colorSat, colorVal);
        retColor = hsvColor.toRgb();
    }
    return retColor;
}

QColor QS60StylePrivate::lighterColor(const QColor &baseColor)
{
    QColor result(baseColor);
    bool modifyColor = false;
    if (result.saturation() == 0) {
        result.setHsv(result.hue(), 128, result.value());
        modifyColor = true;
    }
    if (result.value() == 0) {
        result.setHsv(result.hue(), result.saturation(), 128);
        modifyColor = true;
    }
    if (modifyColor)
        result = result.lighter(175);
    else
        result = result.lighter(225);
    return result;
}

bool QS60StylePrivate::drawsOwnThemeBackground(const QWidget *widget)
{
    return qobject_cast<const QDialog *> (widget);
}

QFont QS60StylePrivate::s60Font(
    QS60StyleEnums::FontCategories fontCategory, int pointSize) const
{
    QFont result;
    int actualPointSize = pointSize;
    if (actualPointSize <= 0) {
        const QFont appFont = QApplication::font();
        actualPointSize = appFont.pointSize();
        if (actualPointSize <= 0)
            actualPointSize = appFont.pixelSize() * 72 / qt_defaultDpiY();
    }
    Q_ASSERT(actualPointSize > 0);
    const QPair<QS60StyleEnums::FontCategories, int> key(fontCategory, actualPointSize);
    if (!m_mappedFontsCache.contains(key)) {
        result = s60Font_specific(fontCategory, actualPointSize);
        m_mappedFontsCache.insert(key, result);
    } else {
        result = m_mappedFontsCache.value(key);
        if (result.pointSize() != actualPointSize)
            result.setPointSize(actualPointSize);
    }
    return result;
}

void QS60StylePrivate::clearCaches(CacheClearReason reason)
{
    switch(reason){
    case CC_LayoutChange:
        // when layout changes, the colors remain in cache, but graphics and fonts can change
        m_mappedFontsCache.clear();
        deleteBackground();
        QPixmapCache::clear();
        break;
    case CC_ThemeChange:
        m_colorCache.clear();
        QPixmapCache::clear();
        deleteBackground();
        break;
    case CC_UndefinedChange:
    default:
        m_colorCache.clear();
        m_mappedFontsCache.clear();
        QPixmapCache::clear();
        deleteBackground();
        break;
    }
}

// Since S60Style has 'button' and 'tooltip' as a graphic, we don't have any native color which to use
// for QPalette::Button and QPalette::ToolTipBase. Therefore S60Style needs to guesstimate
// palette colors by calculating average rgb values for button pixels.
// Returns Qt::black if there is an issue with the graphics (image is NULL, or no bits() found).
QColor QS60StylePrivate::colorFromFrameGraphics(SkinFrameElements frame) const
{
    const bool cachedColorExists = m_colorCache.contains(frame);
    if (!cachedColorExists) {
        const int frameCornerWidth = pixelMetric(PM_Custom_FrameCornerWidth);
        const int frameCornerHeight = pixelMetric(PM_Custom_FrameCornerHeight);
        Q_ASSERT(2*frameCornerWidth<32);
        Q_ASSERT(2*frameCornerHeight<32);

        const QImage frameImage = QS60StylePrivate::frame(frame, QSize(32,32)).toImage();
        Q_ASSERT(frameImage.bytesPerLine() > 0);
        if (frameImage.isNull())
            return Qt::black;

        const QRgb *pixelRgb = (const QRgb*)frameImage.bits();
        const int pixels = frameImage.byteCount()/sizeof(QRgb);

        int estimatedRed = 0;
        int estimatedGreen = 0;
        int estimatedBlue = 0;

        int skips = 0;
        int estimations = 0;

        const int topBorderLastPixel = frameCornerHeight*frameImage.width()-1;
        const int bottomBorderFirstPixel = frameImage.width()*frameImage.height()-frameCornerHeight*frameImage.width()-1;
        const int rightBorderFirstPixel = frameImage.width()-frameCornerWidth;
        const int leftBorderLastPixel = frameCornerWidth;

        while ((skips + estimations) < pixels) {
            if ((skips+estimations) > topBorderLastPixel &&
                (skips+estimations) < bottomBorderFirstPixel) {
                for (int rowIndex = 0; rowIndex < frameImage.width(); rowIndex++) {
                    if (rowIndex > leftBorderLastPixel &&
                        rowIndex < rightBorderFirstPixel) {
                        estimatedRed += qRed(*pixelRgb);
                        estimatedGreen += qGreen(*pixelRgb);
                        estimatedBlue += qBlue(*pixelRgb);
                    }
                    pixelRgb++;
                    estimations++;
                }
            } else {
                pixelRgb++;
                skips++;
            }
        }
        QColor frameColor(estimatedRed/estimations, estimatedGreen/estimations, estimatedBlue/estimations);
        m_colorCache.insert(frame, frameColor);
        return !estimations ? Qt::black : frameColor;
    } else {
        return m_colorCache.value(frame);
    }

}

void QS60StylePrivate::setThemePalette(QApplication *app) const
{
    Q_UNUSED(app)
    QPalette widgetPalette = QPalette(Qt::white);
    setThemePalette(&widgetPalette);
    QApplication::setPalette(widgetPalette); //calling QApplication::setPalette clears palette hash
    setThemePaletteHash(&widgetPalette);
    storeThemePalette(&widgetPalette);
}

void QS60StylePrivate::setThemePalette(QStyleOption *option) const
{
    setThemePalette(&option->palette);
}

QPalette* QS60StylePrivate::themePalette()
{
    return m_themePalette;
}

void QS60StylePrivate::setBackgroundTexture(QApplication *app) const
{
    Q_UNUSED(app)
    QPalette applicationPalette = QApplication::palette();
    applicationPalette.setBrush(QPalette::Window, backgroundTexture());
    setThemePalette(&applicationPalette);
    QApplication::setPalette(applicationPalette);
    setThemePaletteHash(&applicationPalette);
}

void QS60StylePrivate::deleteBackground()
{
    if (m_background) {
        delete m_background;
        m_background = 0;
    }
}

void QS60StylePrivate::setCurrentLayout(int index)
{
    m_pmPointer = data[index];
}

void QS60StylePrivate::drawPart(QS60StyleEnums::SkinParts skinPart,
    QPainter *painter, const QRect &rect, SkinElementFlags flags)
{
    static const bool doCache =
#if defined(Q_WS_S60)
        // Freezes on 3.1. Anyways, caching is only really needed on touch UI
        !(QSysInfo::s60Version() == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2);
#else
        true;
#endif

    const QPixmap skinPartPixMap((doCache ? cachedPart : part)(skinPart, rect.size(), painter, flags));
    if (!skinPartPixMap.isNull())
        painter->drawPixmap(rect.topLeft(), skinPartPixMap);
}

void QS60StylePrivate::drawFrame(SkinFrameElements frameElement, QPainter *painter, const QRect &rect, SkinElementFlags flags)
{
    static const bool doCache =
#if defined(Q_WS_S60)
        // Freezes on 3.1. Anyways, caching is only really needed on touch UI
        !(QSysInfo::s60Version() == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2);
#else
        true;
#endif
    const QPixmap frameElementPixMap((doCache ? cachedFrame : frame)(frameElement, rect.size(), flags));
    if (!frameElementPixMap.isNull())
        painter->drawPixmap(rect.topLeft(), frameElementPixMap);
}

void QS60StylePrivate::drawRow(QS60StyleEnums::SkinParts start,
    QS60StyleEnums::SkinParts middle, QS60StyleEnums::SkinParts end,
    Qt::Orientation orientation, QPainter *painter, const QRect &rect,
    SkinElementFlags flags)
{
    QSize startEndSize(partSize(start, flags));
    startEndSize.scale(rect.size(), Qt::KeepAspectRatio);

    QRect startRect = QRect(rect.topLeft(), startEndSize);
    QRect middleRect = rect;
    QRect endRect;

    if (orientation == Qt::Horizontal) {
        startRect.setWidth(qMin((rect.width() >> 1) - 1, startRect.width()));
        endRect = startRect.translated(rect.width() - startRect.width(), 0);
        middleRect.adjust(startRect.width(), 0, -startRect.width(), 0);
        if (startRect.bottomRight().x() > endRect.topLeft().x()) {
            const int overlap = (startRect.bottomRight().x() -  endRect.topLeft().x())>>1;
            startRect.setWidth(startRect.width()-overlap);
            endRect.adjust(overlap,0,0,0);
        }
    } else {
        startRect.setHeight(qMin((rect.height() >> 1) - 1, startRect.height()));
        endRect = startRect.translated(0, rect.height() - startRect.height());
        middleRect.adjust(0, startRect.height(), 0, -startRect.height());
        if (startRect.topRight().y() > endRect.bottomLeft().y()) {
            const int overlap = (startRect.topRight().y() - endRect.bottomLeft().y())>>1;
            startRect.setHeight(startRect.height()-overlap);
            endRect.adjust(0,overlap,0,0);
        }
    }

#if 0
    painter->save();
    painter->setOpacity(.3);
    painter->fillRect(startRect, Qt::red);
    painter->fillRect(middleRect, Qt::green);
    painter->fillRect(endRect, Qt::blue);
    painter->restore();
#else
    drawPart(start, painter, startRect, flags);
    if (middleRect.isValid())
        drawPart(middle, painter, middleRect, flags);
    drawPart(end, painter, endRect, flags);
#endif
}

QPixmap QS60StylePrivate::cachedPart(QS60StyleEnums::SkinParts part,
    const QSize &size, QPainter *painter, SkinElementFlags flags)
{
    QPixmap result;
    const QString cacheKey =
        QString::fromLatin1("S60Style: SkinParts=%1 QSize=%2|%3 SkinPartFlags=%4")
            .arg((int)part).arg(size.width()).arg(size.height()).arg((int)flags);
    if (!QPixmapCache::find(cacheKey, result)) {
        result = QS60StylePrivate::part(part, size, painter, flags);
        QPixmapCache::insert(cacheKey, result);
    }
    return result;
}

QPixmap QS60StylePrivate::cachedFrame(SkinFrameElements frame, const QSize &size, SkinElementFlags flags)
{
    QPixmap result;
    const QString cacheKey =
        QString::fromLatin1("S60Style: SkinFrameElements=%1 QSize=%2|%3 SkinElementFlags=%4")
            .arg((int)frame).arg(size.width()).arg(size.height()).arg((int)flags);
    if (!QPixmapCache::find(cacheKey, result)) {
        result = QS60StylePrivate::frame(frame, size, flags);
        QPixmapCache::insert(cacheKey, result);
    }
    return result;
}

void QS60StylePrivate::refreshUI()
{
    QList<QWidget *> widgets = QApplication::allWidgets();

    for (int i = 0; i < widgets.size(); ++i) {
        QWidget *widget = widgets.at(i);
        if (widget == 0)
            continue;

        if (widget->style()) {
            widget->style()->polish(widget);
            QEvent event(QEvent::StyleChange);
            qApp->sendEvent(widget, &event);
        }
        widget->update();
        widget->updateGeometry();
    }
}

void QS60StylePrivate::setFont(QWidget *widget) const
{
    QS60StyleEnums::FontCategories fontCategory = QS60StyleEnums::FC_Undefined;
    if (!widget)
        return;
    if (qobject_cast<QPushButton *>(widget)){
        fontCategory = QS60StyleEnums::FC_Primary;
    } else if (qobject_cast<QToolButton *>(widget)){
        fontCategory = QS60StyleEnums::FC_Primary;
    } else if (qobject_cast<QHeaderView *>(widget)){
        fontCategory = QS60StyleEnums::FC_Secondary;
    } else if (qobject_cast<QGroupBox *>(widget)){
        fontCategory = QS60StyleEnums::FC_Title;
    }
    if (fontCategory != QS60StyleEnums::FC_Undefined) {
        const QFont suggestedFont =
            s60Font(fontCategory, widget->font().pointSizeF());
        widget->setFont(suggestedFont);
    }
}

void QS60StylePrivate::setThemePalette(QWidget *widget) const
{
    if(!widget)
        return;
    QPalette widgetPalette = QApplication::palette(widget);

    //header view and its viewport need to be set 100% transparent button color, since drawing code will
    //draw transparent theme graphics to table column and row headers.
    if (qobject_cast<QHeaderView *>(widget)){
        widgetPalette.setColor(QPalette::Active, QPalette::ButtonText,
            s60Color(QS60StyleEnums::CL_QsnTextColors, 23, 0));
        QHeaderView* header = qobject_cast<QHeaderView *>(widget);
        widgetPalette.setColor(QPalette::Button, Qt::transparent );
        if ( header->viewport() )
            header->viewport()->setPalette(widgetPalette);
        QApplication::setPalette(widgetPalette, "QHeaderView");
    }
}

void QS60StylePrivate::setThemePalette(QPalette *palette) const
{
    if (!palette)
        return;

    // basic colors
    palette->setColor(QPalette::WindowText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 6, 0));
    palette->setColor(QPalette::ButtonText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 6, 0));
    palette->setColor(QPalette::Text,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 6, 0));
    palette->setColor(QPalette::ToolTipText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 55, 0));
    palette->setColor(QPalette::BrightText, palette->color(QPalette::WindowText).lighter());
    palette->setColor(QPalette::HighlightedText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 10, 0));
    palette->setColor(QPalette::Link,
        s60Color(QS60StyleEnums::CL_QsnHighlightColors, 3, 0));
    palette->setColor(QPalette::LinkVisited, palette->color(QPalette::Link).darker());
    palette->setColor(QPalette::Highlight,
        s60Color(QS60StyleEnums::CL_QsnHighlightColors, 2, 0));
    // set background image as a texture brush
    palette->setBrush(QPalette::Window, backgroundTexture());
    // set these as transparent so that styled full screen theme background is visible
    palette->setColor(QPalette::AlternateBase, Qt::transparent);
    palette->setBrush(QPalette::Base, Qt::transparent);
    // set button and tooltipbase based on pixel colors
    const QColor buttonColor = colorFromFrameGraphics(SF_ButtonNormal);
    palette->setColor(QPalette::Button, buttonColor);
    const QColor toolTipColor = colorFromFrameGraphics(SF_ToolTip);
    palette->setColor(QPalette::ToolTipBase, toolTipColor);
    palette->setColor(QPalette::Light, palette->color(QPalette::Button).lighter());
    palette->setColor(QPalette::Dark, palette->color(QPalette::Button).darker());
    palette->setColor(QPalette::Midlight, palette->color(QPalette::Button).lighter(125));
    palette->setColor(QPalette::Mid, palette->color(QPalette::Button).darker(150));
    palette->setColor(QPalette::Shadow, Qt::black);
}

void QS60StylePrivate::deleteThemePalette()
{
    if (m_themePalette) {
        delete m_themePalette;
        m_themePalette = 0;
    }
}

void QS60StylePrivate::storeThemePalette(QPalette *palette)
{
    deleteThemePalette();
    //store specified palette for latter use.
    m_themePalette = new QPalette(*palette);
}

// set widget specific palettes
void QS60StylePrivate::setThemePaletteHash(QPalette *palette) const
{
    if (!palette)
        return;

    //store the original palette
    QPalette widgetPalette = *palette;
    const QColor mainAreaTextColor =
        s60Color(QS60StyleEnums::CL_QsnTextColors, 6, 0);

    widgetPalette.setColor(QPalette::WindowText,
        s60Color(QS60StyleEnums::CL_QsnLineColors, 8, 0));
    QApplication::setPalette(widgetPalette, "QSlider");
    // return to original palette after each widget
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::Active, QPalette::ButtonText, mainAreaTextColor);
    widgetPalette.setColor(QPalette::Inactive, QPalette::ButtonText, mainAreaTextColor);
    const QStyleOption opt;
    widgetPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 6, &opt));
    QApplication::setPalette(widgetPalette, "QPushButton");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::Active, QPalette::ButtonText, mainAreaTextColor);
    widgetPalette.setColor(QPalette::Inactive, QPalette::ButtonText, mainAreaTextColor);
    QApplication::setPalette(widgetPalette, "QToolButton");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::Active, QPalette::ButtonText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 23, 0));
    QApplication::setPalette(widgetPalette, "QHeaderView");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::ButtonText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 8, 0));
    QApplication::setPalette(widgetPalette, "QMenuBar");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::WindowText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 4, 0));
    QApplication::setPalette(widgetPalette, "QTabBar");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::Text,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 22, 0));
    QApplication::setPalette(widgetPalette, "QTableView");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::HighlightedText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 24, 0));
    QApplication::setPalette(widgetPalette, "QLineEdit");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::Text,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 34, 0));
    widgetPalette.setColor(QPalette::HighlightedText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 24, 0));
    QApplication::setPalette(widgetPalette, "QTextEdit");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::HighlightedText,
        s60Color(QS60StyleEnums::CL_QsnTextColors, 24, 0));
    QApplication::setPalette(widgetPalette, "QComboBox");
    widgetPalette = *palette;

    widgetPalette.setColor(QPalette::WindowText, mainAreaTextColor);
    widgetPalette.setColor(QPalette::Button, QApplication::palette().color(QPalette::Button));
    widgetPalette.setColor(QPalette::Dark, mainAreaTextColor.darker());
    widgetPalette.setColor(QPalette::Light, mainAreaTextColor.lighter());
    QApplication::setPalette(widgetPalette, "QDial");
    widgetPalette = *palette;

    widgetPalette.setBrush(QPalette::Window, QBrush());
    QApplication::setPalette(widgetPalette, "QScrollArea");
    widgetPalette = *palette;

    //Webpages should not use S60 theme colors as they are designed to work
    //with themeBackground and do not generally mesh well with web page backgrounds.
    QPalette webPalette = *palette;
    webPalette.setColor(QPalette::WindowText, Qt::black);
    webPalette.setColor(QPalette::Text, Qt::black);
    QApplication::setPalette(webPalette, "QWebView");
    QApplication::setPalette(webPalette, "QGraphicsWebView");
}

QSize QS60StylePrivate::partSize(QS60StyleEnums::SkinParts part, SkinElementFlags flags)
{
    QSize result(20, 20);
    switch (part)
        {
        case QS60StyleEnums::SP_QgnGrafBarProgress:
            result.setWidth(pixelMetric(QStyle::PM_ProgressBarChunkWidth));
            break;
        case QS60StyleEnums::SP_QgnGrafTabActiveM:
        case QS60StyleEnums::SP_QgnGrafTabPassiveM:
        case QS60StyleEnums::SP_QgnGrafTabActiveR:
        case QS60StyleEnums::SP_QgnGrafTabPassiveR:
        case QS60StyleEnums::SP_QgnGrafTabPassiveL:
        case QS60StyleEnums::SP_QgnGrafTabActiveL:
            //Returned QSize for tabs must not be square, but narrow rectangle with width:height
            //ratio of 1:2 for horizontal tab bars (and 2:1 for vertical ones).
            result.setWidth(10);
            break;
        case QS60StyleEnums::SP_QgnIndiSliderEdit:
            result.scale(pixelMetric(QStyle::PM_SliderLength),
                pixelMetric(QStyle::PM_SliderControlThickness), Qt::IgnoreAspectRatio);
            break;

        case QS60StyleEnums::SP_QgnGrafBarFrameSideL:
        case QS60StyleEnums::SP_QgnGrafBarFrameSideR:
            result.setWidth(pixelMetric(PM_Custom_FrameCornerWidth));
            break;

        case QS60StyleEnums::SP_QsnCpScrollHandleBottomPressed:
        case QS60StyleEnums::SP_QsnCpScrollHandleTopPressed:
        case QS60StyleEnums::SP_QsnCpScrollHandleMiddlePressed:
        case QS60StyleEnums::SP_QsnCpScrollBgBottom:
        case QS60StyleEnums::SP_QsnCpScrollBgMiddle:
        case QS60StyleEnums::SP_QsnCpScrollBgTop:
        case QS60StyleEnums::SP_QsnCpScrollHandleBottom:
        case QS60StyleEnums::SP_QsnCpScrollHandleMiddle:
        case QS60StyleEnums::SP_QsnCpScrollHandleTop:
            result.setHeight(pixelMetric(QStyle::PM_ScrollBarExtent));
            result.setWidth(pixelMetric(QStyle::PM_ScrollBarSliderMin));
            break;
        default:
            // Generic frame part size gathering.
            for (int i = 0; i < frameElementsCount; ++i)
            {
                switch (m_frameElementsData[i].center - part) {
                    case 8: /* CornerTl */
                    case 7: /* CornerTr */
                    case 6: /* CornerBl */
                    case 5: /* CornerBr */
                        result.setWidth(pixelMetric(PM_Custom_FrameCornerWidth));
                        // Falltrough intended...
                    case 4: /* SideT */
                    case 3: /* SideB */
                        result.setHeight(pixelMetric(PM_Custom_FrameCornerHeight));
                        break;
                    case 2: /* SideL */
                    case 1: /* SideR */
                        result.setWidth(pixelMetric(PM_Custom_FrameCornerWidth));
                        break;
                    case 0: /* center */
                    default:
                        break;
                }
            }
            break;
    }
    if (flags & (SF_PointEast | SF_PointWest)) {
        const int temp = result.width();
        result.setWidth(result.height());
        result.setHeight(temp);
    }
    return result;
}

bool QS60StylePrivate::canDrawThemeBackground(const QBrush &backgroundBrush) 
{
    //If brush is not changed from style's default values, draw theme graphics.
    return (backgroundBrush.color() == Qt::transparent ||
            backgroundBrush.style() == Qt::NoBrush) ? true : false;
}

/*!
  \class QS60Style
  \brief The QS60Style class provides a look and feel suitable for applications on S60.
  \since 4.6
  \ingroup appearance

  \sa QMacStyle, QWindowsStyle, QWindowsXPStyle, QWindowsVistaStyle, QPlastiqueStyle, QCleanlooksStyle, QMotifStyle
*/


/*!
    Destroys the style.
*/
QS60Style::~QS60Style()
{
}

/*!
  \reimp
*/
void QS60Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    const QS60StylePrivate::SkinElementFlags flags = (option->state & State_Enabled) ?  QS60StylePrivate::SF_StateEnabled : QS60StylePrivate::SF_StateDisabled;
    SubControls sub = option->subControls;

    switch (control) {
#ifndef QT_NO_SCROLLBAR
    case CC_ScrollBar:
        if (const QStyleOptionSlider *optionSlider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            const bool horizontal = optionSlider->orientation == Qt::Horizontal;

            const QRect scrollBarSlider = subControlRect(control, optionSlider, SC_ScrollBarSlider, widget);
            const QRect grooveRect = subControlRect(control, optionSlider, SC_ScrollBarGroove, widget);

            const QS60StylePrivate::SkinElements grooveElement =
                horizontal ? QS60StylePrivate::SE_ScrollBarGrooveHorizontal : QS60StylePrivate::SE_ScrollBarGrooveVertical;
            QS60StylePrivate::drawSkinElement(grooveElement, painter, grooveRect, flags);

            const QStyle::SubControls subControls = optionSlider->subControls;

            // select correct slider (horizontal/vertical/pressed)
            const bool sliderPressed = ((optionSlider->state & QStyle::State_Sunken) && (subControls & SC_ScrollBarSlider));
            const QS60StylePrivate::SkinElements handleElement =
                horizontal ?
                    ( sliderPressed ?
                        QS60StylePrivate::SE_ScrollBarHandlePressedHorizontal :
                        QS60StylePrivate::SE_ScrollBarHandleHorizontal ) :
                    ( sliderPressed ?
                        QS60StylePrivate::SE_ScrollBarHandlePressedVertical :
                        QS60StylePrivate::SE_ScrollBarHandleVertical);
            QS60StylePrivate::drawSkinElement(handleElement, painter, scrollBarSlider, flags);
        }
        break;
#endif // QT_NO_SCROLLBAR
#ifndef QT_NO_SLIDER
    case CC_Slider:
        if (const QStyleOptionSlider *optionSlider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {

            // The groove is just a centered line. Maybe a qgn_graf_line_* at some point
            const QRect sliderGroove = subControlRect(control, optionSlider, SC_SliderGroove, widget);
            const QPoint sliderGrooveCenter = sliderGroove.center();
            const bool horizontal = optionSlider->orientation == Qt::Horizontal;
            painter->save();
            if (widget)
                painter->setPen(widget->palette().windowText().color());
            if (horizontal)
                painter->drawLine(0, sliderGrooveCenter.y(), sliderGroove.right(), sliderGrooveCenter.y());
            else
                painter->drawLine(sliderGrooveCenter.x(), 0, sliderGrooveCenter.x(), sliderGroove.bottom());
            painter->restore();

            const QRect sliderHandle = subControlRect(control, optionSlider, SC_SliderHandle, widget);
            const QS60StylePrivate::SkinElements handleElement =
                horizontal ? QS60StylePrivate::SE_SliderHandleHorizontal : QS60StylePrivate::SE_SliderHandleVertical;
            QS60StylePrivate::drawSkinElement(handleElement, painter, sliderHandle, flags);
        }
        break;
#endif // QT_NO_SLIDER
#ifndef QT_NO_COMBOBOX
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            const QRect cmbxEditField = subControlRect(CC_ComboBox, option, SC_ComboBoxEditField, widget);
            const QRect cmbxFrame = subControlRect(CC_ComboBox, option, SC_ComboBoxFrame, widget);
            const bool direction = cmb->direction == Qt::LeftToRight;

            // Button frame
            QStyleOptionFrame  buttonOption;
            buttonOption.QStyleOption::operator=(*cmb);
            const int maxHeight = cmbxFrame.height();
            const int maxWidth = cmbxFrame.width() - cmbxEditField.width();
            const int topLeftPoint = direction ? cmbxEditField.right()+1 : cmbxEditField.left()+1-maxWidth;
            const QRect buttonRect(topLeftPoint, cmbxEditField.top(), maxWidth, maxHeight);
            buttonOption.rect = buttonRect;
            buttonOption.state = cmb->state & (State_Enabled | State_MouseOver);
            drawPrimitive(PE_PanelButtonCommand, &buttonOption, painter, widget);

            // draw label background - label itself is drawn separately
            const QS60StylePrivate::SkinElements skinElement = QS60StylePrivate::SE_FrameLineEdit;
            QS60StylePrivate::drawSkinElement(skinElement, painter, cmbxEditField, flags);

            // Draw the combobox arrow
            if (sub & SC_ComboBoxArrow) {
                // Make rect slightly smaller
                buttonOption.rect.adjust(1, 1, -1, -1);
                painter->save();
                painter->setPen(option->palette.buttonText().color());
                drawPrimitive(PE_IndicatorSpinDown, &buttonOption, painter, widget);
                painter->restore();
            }
        }
        break;
#endif // QT_NO_COMBOBOX
#ifndef QT_NO_TOOLBUTTON
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolBtn = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            const State bflags = toolBtn->state;
            const QRect button(subControlRect(control, toolBtn, SC_ToolButton, widget));
            QRect menuRect = QRect();
            if (toolBtn->subControls & SC_ToolButtonMenu)
                menuRect = subControlRect(control, toolBtn, SC_ToolButtonMenu, widget);

            QStyleOptionToolButton toolButton = *toolBtn;

            if (sub&SC_ToolButton) {
                QStyleOption tool(0);
                tool.palette = toolBtn->palette;

                // Check if toolbutton is in toolbar.
                QToolBar *toolBar = 0;
                if (widget)
                    toolBar = qobject_cast<QToolBar *>(widget->parentWidget());

                if (bflags & (State_Sunken | State_On | State_Raised)) {
                    tool.rect = button.unite(menuRect);
                    tool.state = bflags;

                    // todo: I'd like to move extension button next to where last button is
                    // however, the painter seems to want to clip the button rect even if I turn of the clipping.
                    if (toolBar && (qobject_cast<const QToolBarExtension *>(widget))){
                        /*QList<QAction *> actionList = toolBar->actions();
                        const int actionCount = actionList.count();
                        const int toolbarWidth = toolBar->width();
                        const int extButtonWidth = pixelMetric(PM_ToolBarExtensionExtent, option, widget);
                        const int toolBarButtonWidth = pixelMetric(PM_ToolBarIconSize, option, widget);
                        const int frame = pixelMetric(PM_ToolBarFrameWidth, option, widget);
                        const int margin = pixelMetric(PM_ToolBarItemMargin, option, widget);
                        const int border = frame + margin;
                        const int spacing = pixelMetric(PM_ToolBarItemSpacing, option, widget);
                        const int toolBarButtonArea = toolbarWidth - extButtonWidth - spacing - 2*border;
                        const int numberOfVisibleButtons = toolBarButtonArea / toolBarButtonWidth;
                        // new extension button place is after border and all the other visible buttons (with spacings)
                        const int newXForExtensionButton = numberOfVisibleButtons * toolBarButtonWidth + (numberOfVisibleButtons-1)*spacing + border;
                        painter->save();
                        painter->setClipping(false);
                        tool.rect.translate(-newXForExtensionButton,0);
                        painter->restore();*/
                    }

                    if (toolBar){
                        /*if (toolBar->orientation() == Qt::Vertical){
                            // todo: I'd like to make all vertical buttons the same size, but again the painter
                            // prefers to use clipping for button rects, even though clipping has been set off.
                            painter->save();
                            painter->setClipping(false);

                            const int origWidth = tool.rect.width();
                            const int newWidth = toolBar->width()-2*pixelMetric(PM_ToolBarFrameWidth, option, widget);
                            painter->translate(origWidth-newWidth,0);
                            tool.rect.translate(origWidth-tool.rect.width(),0);
                            tool.rect.setWidth(newWidth);

                            if (option->state & QStyle::State_Sunken)
                                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ToolBarButtonPressed, painter, tool.rect, flags);
                            else
                                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ToolBarButton, painter, tool.rect, flags);

                        }*/
                        if (option->state & QStyle::State_Sunken)
                            QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ToolBarButtonPressed, painter, tool.rect, flags);
                        else
                            QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ToolBarButton, painter, tool.rect, flags);
                        /*
                        if (toolBar->orientation() == Qt::Vertical)
                            painter->restore();
                            */
                    } else {
                        drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);
                    }

                    if (toolButton.subControls & SC_ToolButtonMenu) {
                        tool.rect = menuRect;
                        tool.state = bflags;
                        drawPrimitive(PE_IndicatorArrowDown, &tool, painter, widget);
                    }
                }
            }

            if (toolBtn->features & QStyleOptionToolButton::Arrow) {
                QStyle::PrimitiveElement pe;
                switch (toolBtn->arrowType) {
                    case Qt::LeftArrow:
                        pe = QStyle::PE_IndicatorArrowLeft;
                        break;
                    case Qt::RightArrow:
                        pe = QStyle::PE_IndicatorArrowRight;
                        break;
                    case Qt::UpArrow:
                        pe = QStyle::PE_IndicatorArrowUp;
                        break;
                    case Qt::DownArrow:
                        pe = QStyle::PE_IndicatorArrowDown;
                        break;
                    default:
                        break; }
                toolButton.rect = button;
                drawPrimitive(pe, &toolButton, painter, widget);
            }

            if (toolBtn->text.length()>0 ||
                !toolBtn->icon.isNull()) {
                const int frameWidth = pixelMetric(PM_DefaultFrameWidth, option, widget);
                toolButton.rect = button.adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);
                drawControl(CE_ToolButtonLabel, &toolButton, painter, widget);
                }
            }
        break;
#endif //QT_NO_TOOLBUTTON
#ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QStyleOptionSpinBox copy = *spinBox;
            PrimitiveElement pe;

            if (spinBox->subControls & SC_SpinBoxUp) {
                copy.subControls = SC_SpinBoxUp;
                QPalette spinBoxPal = spinBox->palette;
                if (!(spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled)) {
                    spinBoxPal.setCurrentColorGroup(QPalette::Disabled);
                    copy.state &= ~State_Enabled;
                    copy.palette = spinBoxPal;
                }

                if (spinBox->activeSubControls == SC_SpinBoxUp && (spinBox->state & State_Sunken)) {
                    copy.state |= State_On;
                    copy.state |= State_Sunken;
                } else {
                    copy.state |= State_Raised;
                    copy.state &= ~State_Sunken;
                }
                pe = (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus) ?
                      PE_IndicatorSpinPlus :
                      PE_IndicatorSpinUp;

                copy.rect = subControlRect(CC_SpinBox, spinBox, SC_SpinBoxUp, widget);
                drawPrimitive(PE_PanelButtonBevel, &copy, painter, widget);
                copy.rect.adjust(1, 1, -1, -1);
                drawPrimitive(pe, &copy, painter, widget);
            }

            if (spinBox->subControls & SC_SpinBoxDown) {
                copy.subControls = SC_SpinBoxDown;
                copy.state = spinBox->state;
                QPalette spinBoxPal = spinBox->palette;
                if (!(spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled)) {
                    spinBoxPal.setCurrentColorGroup(QPalette::Disabled);
                    copy.state &= ~State_Enabled;
                    copy.palette = spinBoxPal;
                }

                if (spinBox->activeSubControls == SC_SpinBoxDown && (spinBox->state & State_Sunken)) {
                    copy.state |= State_On;
                    copy.state |= State_Sunken;
                } else {
                    copy.state |= State_Raised;
                    copy.state &= ~State_Sunken;
                }
                pe = (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus) ?
                      PE_IndicatorSpinMinus :
                      PE_IndicatorSpinDown;

                copy.rect = subControlRect(CC_SpinBox, spinBox, SC_SpinBoxDown, widget);
                drawPrimitive(PE_PanelButtonBevel, &copy, painter, widget);
                copy.rect.adjust(1, 1, -1, -1);
                drawPrimitive(pe, &copy, painter, widget);
            }
        }
        break;
#endif //QT_NO_SPINBOX
#ifndef QT_NO_GROUPBOX
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            // Draw frame
            const QRect textRect = subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);
            const QRect checkBoxRect = subControlRect(CC_GroupBox, option, SC_GroupBoxCheckBox, widget);
            if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
                QStyleOptionFrameV2 frame;
                frame.QStyleOption::operator=(*groupBox);
                frame.features = groupBox->features;
                frame.lineWidth = groupBox->lineWidth;
                frame.midLineWidth = groupBox->midLineWidth;
                frame.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxFrame, widget);
                drawPrimitive(PE_FrameGroupBox, &frame, painter, widget);
            }

            // Draw title
            if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
                const QColor textColor = groupBox->textColor;
                painter->save();

                if (textColor.isValid())
                    painter->setPen(textColor);
                int alignment = int(groupBox->textAlignment);
                if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget))
                    alignment |= Qt::TextHideMnemonic;

                drawItemText(painter, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | Qt::AlignVCenter | alignment,
                             groupBox->palette, groupBox->state & State_Enabled, groupBox->text,
                             textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
                painter->restore();
            }

            // Draw checkbox
            if (groupBox->subControls & SC_GroupBoxCheckBox) {
                QStyleOptionButton box;
                box.QStyleOption::operator=(*groupBox);
                box.rect = checkBoxRect;
                drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
            }
        }
        break;
#endif //QT_NO_GROUPBOX
    default:
        QCommonStyle::drawComplexControl(control, option, painter, widget);
    }
}

/*!
  \reimp
*/
void QS60Style::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D(const QS60Style);
    const QS60StylePrivate::SkinElementFlags flags = (option->state & State_Enabled) ?  QS60StylePrivate::SF_StateEnabled : QS60StylePrivate::SF_StateDisabled;
    switch (element) {
    case CE_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {

            drawControl(CE_PushButtonBevel, btn, painter, widget);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(SE_PushButtonContents, btn, widget);

            drawControl(CE_PushButtonLabel, &subopt, painter, widget);
        }
        break;
    case CE_PushButtonBevel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const bool isDisabled = !(option->state & QStyle::State_Enabled);
            const bool isFlat = button->features & QStyleOptionButton::Flat;
            QS60StyleEnums::SkinParts skinPart;
            QS60StylePrivate::SkinElements skinElement;
            if (!isDisabled) {
                const bool isPressed = (option->state & QStyle::State_Sunken) ||
                                       (option->state & QStyle::State_On);
                if (isFlat) {
                    skinPart =
                        isPressed ? QS60StyleEnums::SP_QsnFrButtonTbCenterPressed : QS60StyleEnums::SP_QsnFrButtonTbCenter;
                } else {
                    skinElement =
                        isPressed ? QS60StylePrivate::SE_ButtonPressed : QS60StylePrivate::SE_ButtonNormal;
                }
            } else {
                if (isFlat)
                    skinPart =QS60StyleEnums::SP_QsnFrButtonCenterInactive;
                else
                    skinElement = QS60StylePrivate::SE_ButtonInactive;
            }
            if (isFlat)
                QS60StylePrivate::drawSkinPart(skinPart, painter, option->rect, flags);
            else
                QS60StylePrivate::drawSkinElement(skinElement, painter, option->rect, flags);
            }
        break;
#ifndef QT_NO_TOOLBUTTON
    case CE_ToolButtonLabel:
        if (const QStyleOptionToolButton *toolBtn = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QStyleOptionToolButton optionToolButton = *toolBtn;

            if (!optionToolButton.icon.isNull() && (optionToolButton.state & QStyle::State_Sunken)
                    && (optionToolButton.state & State_Enabled)) {

                    const QIcon::State state = optionToolButton.state & State_On ? QIcon::On : QIcon::Off;
                    const QPixmap pm(optionToolButton.icon.pixmap(optionToolButton.rect.size().boundedTo(optionToolButton.iconSize),
                            QIcon::Normal, state));
                    optionToolButton.icon = generatedIconPixmap(QIcon::Selected, pm, &optionToolButton);
            }

            QCommonStyle::drawControl(element, &optionToolButton, painter, widget);
        }
        break;
#endif //QT_NO_TOOLBUTTON
#ifndef QT_NO_COMBOBOX
    case CE_ComboBoxLabel:
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            QStyleOption optionComboBox = *comboBox;
            optionComboBox.palette.setColor(QPalette::Active, QPalette::WindowText,
                optionComboBox.palette.text().color() );
            optionComboBox.palette.setColor(QPalette::Inactive, QPalette::WindowText,
                optionComboBox.palette.text().color() );
            QRect editRect = subControlRect(CC_ComboBox, comboBox, SC_ComboBoxEditField, widget);
            painter->save();
            painter->setClipRect(editRect);

            if (!comboBox->currentIcon.isNull()) {
                QIcon::Mode mode = comboBox->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                QPixmap pixmap = comboBox->currentIcon.pixmap(comboBox->iconSize, mode);
                QRect iconRect(editRect);
                iconRect.setWidth(comboBox->iconSize.width() + 4);
                iconRect = alignedRect(comboBox->direction,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       iconRect.size(), editRect);
                if (comboBox->editable)
                    painter->fillRect(iconRect, optionComboBox.palette.brush(QPalette::Base));
                drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);

                if (comboBox->direction == Qt::RightToLeft)
                    editRect.translate(-4 - comboBox->iconSize.width(), 0);
                else
                    editRect.translate(comboBox->iconSize.width() + 4, 0);
            }
            if (!comboBox->currentText.isEmpty() && !comboBox->editable) {
                QCommonStyle::drawItemText(painter,
                            editRect.adjusted(QS60StylePrivate::pixelMetric(PM_Custom_FrameCornerWidth), 0, -1, 0),
                            visualAlignment(comboBox->direction, Qt::AlignLeft | Qt::AlignVCenter),
                            comboBox->palette, comboBox->state & State_Enabled, comboBox->currentText);
            }
            painter->restore();
        }
        break;
#endif //QT_NO_COMBOBOX
#ifndef QT_NO_ITEMVIEWS
    case CE_ItemViewItem:
        if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
            QStyleOptionViewItemV4 voptAdj = *vopt;
            painter->save();

            painter->setClipRect(voptAdj.rect);
            const bool isSelected = (vopt->state & QStyle::State_Selected);

            bool isScrollBarVisible = false;
            int scrollBarWidth = 0;
            QList<QScrollBar *> scrollBars = qFindChildren<QScrollBar *>(widget);
            for (int i = 0; i < scrollBars.size(); ++i) {
                QScrollBar *scrollBar = scrollBars.at(i);
                if (scrollBar && scrollBar->orientation() == Qt::Vertical) {
                    isScrollBarVisible = scrollBar->isVisible();
                    scrollBarWidth = scrollBar->size().width();
                    break;
                }
            }

            int rightValue = widget ? widget->contentsRect().right() : 0;

            if (isScrollBarVisible)
                rightValue -= scrollBarWidth;

            if (voptAdj.rect.right() > rightValue)
                voptAdj.rect.setRight(rightValue);

            const QRect iconRect = subElementRect(SE_ItemViewItemDecoration, &voptAdj, widget);
            QRect textRect = subElementRect(SE_ItemViewItemText, &voptAdj, widget);
            const QAbstractItemView *itemView = qobject_cast<const QAbstractItemView *>(widget);

            // draw themed background for table unless background brush has been defined.
            if (vopt->backgroundBrush == Qt::NoBrush) {
                if (itemView) {
                    const QModelIndex index = vopt->index;
                    //todo: Draw cell background only once - for the first cell.
                    QStyleOptionViewItemV4 voptAdj2 = voptAdj;
                    const QModelIndex indexFirst = itemView->model()->index(0,0);
                    const QModelIndex indexLast = itemView->model()->index(
                            itemView->model()->rowCount()-1,itemView->model()->columnCount()-1);
                    if (itemView->viewport())
                        voptAdj2.rect = QRect( itemView->visualRect(indexFirst).topLeft(),
                                itemView->visualRect(indexLast).bottomRight()).intersect(itemView->viewport()->rect());
                    drawPrimitive(PE_PanelItemViewItem, &voptAdj, painter, widget);
                }
            } else { QCommonStyle::drawPrimitive(PE_PanelItemViewItem, &voptAdj, painter, widget);}

            // draw the focus rect
            if (isSelected) {
                QRect highlightRect = option->rect.adjusted(1,1,-1,-1);
                QAbstractItemView::SelectionBehavior selectionBehavior =
                    itemView ? itemView->selectionBehavior() : QAbstractItemView::SelectItems;
                if (selectionBehavior != QAbstractItemView::SelectItems) {
                    // set highlight rect so that it is continuous from cell to cell, yet sligthly
                    // smaller than cell rect
                    int xBeginning = 0, yBeginning = 0, xEnd = 0, yEnd = 0;
                    if (selectionBehavior == QAbstractItemView::SelectRows) {
                        yBeginning = 1; yEnd = -1;
                        if (vopt->viewItemPosition == QStyleOptionViewItemV4::Beginning)
                            xBeginning = 1;
                        else if (vopt->viewItemPosition == QStyleOptionViewItemV4::End)
                            xEnd = -1;
                    } else if (selectionBehavior == QAbstractItemView::SelectColumns) {
                        xBeginning = 1; xEnd = -1;
                        if (vopt->viewItemPosition == QStyleOptionViewItemV4::Beginning)
                            yBeginning = 1;
                        else if (vopt->viewItemPosition == QStyleOptionViewItemV4::End)
                            yEnd = -1;
                    }
                    highlightRect = option->rect.adjusted(xBeginning, yBeginning, xEnd, yEnd);
                }
                if (vopt->showDecorationSelected &&
                    (vopt->palette.highlight().color() == QS60StylePrivate::themePalette()->highlight().color()))
                    QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ListHighlight, painter, highlightRect, flags);
                else
                    painter->fillRect(highlightRect, vopt->palette.highlight());
            }

             // draw the icon
             const QIcon::Mode mode = (voptAdj.state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled;
             const QIcon::State state = voptAdj.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
             voptAdj.icon.paint(painter, iconRect, voptAdj.decorationAlignment, mode, state);

             // Draw selection check mark. Show check mark only in multi selection modes.
             if (itemView) {
                 const bool singleSelection =
                     (itemView->selectionMode() == QAbstractItemView::SingleSelection ||
                      itemView->selectionMode() == QAbstractItemView::NoSelection);
                 const QRect selectionRect = subElementRect(SE_ItemViewItemCheckIndicator, &voptAdj, widget);

                 QStyleOptionViewItemV4 checkMarkOption(voptAdj);
                 // Draw selection mark.
                 if (voptAdj.state & QStyle::State_Selected && !singleSelection) {
                     checkMarkOption.rect = selectionRect;
                     drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &checkMarkOption, painter, widget);
                     if ( textRect.right() > selectionRect.left() )
                         textRect.setRight(selectionRect.left());
                 } else if (singleSelection &&
                     voptAdj.features & QStyleOptionViewItemV2::HasCheckIndicator &&
                     selectionRect.isValid()) {
                     checkMarkOption.rect = selectionRect;
                     checkMarkOption.state = checkMarkOption.state & ~QStyle::State_HasFocus;

                     switch (vopt->checkState) {
                     case Qt::Unchecked:
                         checkMarkOption.state |= QStyle::State_Off;
                         break;
                     case Qt::PartiallyChecked:
                         checkMarkOption.state |= QStyle::State_NoChange;
                         break;
                     case Qt::Checked:
                         checkMarkOption.state |= QStyle::State_On;
                         break;
                     }
                     drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &checkMarkOption, painter, widget);
                 }
             }

             // draw the text
            if (!voptAdj.text.isEmpty()) {
                if (isSelected) {
                    if (qobject_cast<const QTableView *>(widget))
                        voptAdj.palette.setColor(
                            QPalette::Text, QS60StylePrivate::s60Color(QS60StyleEnums::CL_QsnTextColors, 11, 0));
                    else
                        voptAdj.palette.setColor(
                            QPalette::Text, QS60StylePrivate::s60Color(QS60StyleEnums::CL_QsnTextColors, 10, 0));
                }
                painter->setPen(voptAdj.palette.text().color());
                d->viewItemDrawText(painter, &voptAdj, textRect);
            }
            painter->restore();
        }
        break;
#endif // QT_NO_ITEMVIEWS
#ifndef QT_NO_TABBAR
    case CE_TabBarTabShape:
        if (const QStyleOptionTabV3 *optionTab = qstyleoption_cast<const QStyleOptionTabV3 *>(option)) {
            QStyleOptionTabV3 optionTabAdj = *optionTab;
            const bool isSelected = optionTab->state & QStyle::State_Selected;
            const bool directionMirrored = (optionTab->direction == Qt::RightToLeft);
            QS60StylePrivate::SkinElements skinElement;
            switch (optionTab->shape) {
                case QTabBar::TriangularEast:
                case QTabBar::RoundedEast:
                    skinElement = isSelected ? QS60StylePrivate::SE_TabBarTabEastActive:
                        QS60StylePrivate::SE_TabBarTabEastInactive;
                    break;
                case QTabBar::TriangularSouth:
                case QTabBar::RoundedSouth:
                    skinElement = isSelected ? QS60StylePrivate::SE_TabBarTabSouthActive:
                        QS60StylePrivate::SE_TabBarTabSouthInactive;
                    break;
                case QTabBar::TriangularWest:
                case QTabBar::RoundedWest:
                    skinElement = isSelected ? QS60StylePrivate::SE_TabBarTabWestActive:
                        QS60StylePrivate::SE_TabBarTabWestInactive;
                    break;
                case QTabBar::TriangularNorth:
                case QTabBar::RoundedNorth:
                default:
                    skinElement = isSelected ? QS60StylePrivate::SE_TabBarTabNorthActive:
                        QS60StylePrivate::SE_TabBarTabNorthInactive;
                    break;
            }
            if (skinElement==QS60StylePrivate::SE_TabBarTabEastInactive||
                    skinElement==QS60StylePrivate::SE_TabBarTabNorthInactive||
                    skinElement==QS60StylePrivate::SE_TabBarTabSouthInactive||
                    skinElement==QS60StylePrivate::SE_TabBarTabWestInactive||
                    skinElement==QS60StylePrivate::SE_TabBarTabEastActive||
                    skinElement==QS60StylePrivate::SE_TabBarTabNorthActive||
                    skinElement==QS60StylePrivate::SE_TabBarTabSouthActive||
                    skinElement==QS60StylePrivate::SE_TabBarTabWestActive) {
                const int borderThickness =
                    QS60StylePrivate::pixelMetric(QStyle::PM_DefaultFrameWidth);
                const int tabOverlap =
                    QS60StylePrivate::pixelMetric(QStyle::PM_TabBarTabOverlap) - borderThickness;
                //todo: draw navi wipe behind tabbar - must be drawn with first draw

                if (skinElement==QS60StylePrivate::SE_TabBarTabEastInactive||
                        skinElement==QS60StylePrivate::SE_TabBarTabEastActive||
                        skinElement==QS60StylePrivate::SE_TabBarTabWestInactive||
                        skinElement==QS60StylePrivate::SE_TabBarTabWestActive){
                    optionTabAdj.rect.adjust(0, 0, 0, tabOverlap);
                } else {
                    if (directionMirrored)
                        optionTabAdj.rect.adjust(-tabOverlap, 0, 0, 0);
                    else
                        optionTabAdj.rect.adjust(0, 0, tabOverlap, 0);
                    }
            }
            QS60StylePrivate::drawSkinElement(skinElement, painter, optionTabAdj.rect, flags);
        }
        break;
    case CE_TabBarTabLabel:
        if (const QStyleOptionTabV3 *tab = qstyleoption_cast<const QStyleOptionTabV3 *>(option)) {
            QStyleOptionTabV3 optionTab = *tab;
            QRect tr = optionTab.rect;
            const bool directionMirrored = (optionTab.direction == Qt::RightToLeft);
            const int borderThickness = QS60StylePrivate::pixelMetric(QStyle::PM_DefaultFrameWidth);
            const int tabOverlap =
                QS60StylePrivate::pixelMetric(QStyle::PM_TabBarTabOverlap) - borderThickness;
            const QRect windowRect = painter->window();

            switch (tab->shape) {
                case QTabBar::TriangularWest:
                case QTabBar::RoundedWest:
                case QTabBar::TriangularEast:
                case QTabBar::RoundedEast:
                    tr.adjust(0, 0, 0, tabOverlap);
                    break;
                case QTabBar::TriangularSouth:
                case QTabBar::RoundedSouth:
                case QTabBar::TriangularNorth:
                case QTabBar::RoundedNorth:
                default:
                    if (directionMirrored)
                        tr.adjust(-tabOverlap, 0, 0, 0);
                    else
                        tr.adjust(0, 0, tabOverlap, 0);
                    break;
            }
            painter->save();
            QFont f = painter->font();
            f.setPointSizeF(f.pointSizeF() * KTabFontMul);
            painter->setFont(f);

            if (option->state & QStyle::State_Selected){
                optionTab.palette.setColor(QPalette::Active, QPalette::WindowText,
                    QS60StylePrivate::s60Color(QS60StyleEnums::CL_QsnTextColors, 3, option));
            }

            const bool verticalTabs = optionTab.shape == QTabBar::RoundedEast
                                || optionTab.shape == QTabBar::RoundedWest
                                || optionTab.shape == QTabBar::TriangularEast
                                || optionTab.shape == QTabBar::TriangularWest;
            const bool selected = optionTab.state & State_Selected;
            if (verticalTabs) {
                painter->save();
                int newX, newY, newRotation;
                if (optionTab.shape == QTabBar::RoundedEast || optionTab.shape == QTabBar::TriangularEast) {
                    newX = tr.width();
                    newY = tr.y();
                    newRotation = 90;
                } else {
                    newX = 0;
                    newY = tr.y() + tr.height();
                    newRotation = -90;
                }
                tr.setRect(0, 0, tr.height(), tr.width());
                QTransform m;
                m.translate(newX, newY);
                m.rotate(newRotation);
                painter->setTransform(m, true);
            }
            tr.adjust(0, 0, pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab, widget),
                            pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab, widget));

            if (selected) {
                tr.setBottom(tr.bottom() - pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab, widget));
                tr.setRight(tr.right() - pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab, widget));
            }

            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!styleHint(SH_UnderlineShortcut, &optionTab, widget))
                alignment |= Qt::TextHideMnemonic;
            if (!optionTab.icon.isNull()) {
                QSize iconSize = optionTab.iconSize;
                int iconExtent = pixelMetric(PM_TabBarIconSize);
                if (iconSize.height() > iconExtent || iconSize.width() > iconExtent)
                    iconSize = QSize(iconExtent, iconExtent);
                QPixmap tabIcon = optionTab.icon.pixmap(iconSize,
                    (optionTab.state & State_Enabled) ? QIcon::Normal : QIcon::Disabled);
                if (tab->text.isEmpty())
                    painter->drawPixmap(tr.center().x() - (tabIcon.height() >>1),
                                        tr.center().y() - (tabIcon.height() >>1),
                                        tabIcon);
                else
                    painter->drawPixmap(tr.left() + tabOverlap,
                                        tr.center().y() - (tabIcon.height() >>1),
                                        tabIcon);
                tr.setLeft(tr.left() + iconSize.width() + 4);
            }

            QCommonStyle::drawItemText(painter, tr, alignment, optionTab.palette, tab->state & State_Enabled, tab->text, QPalette::WindowText);
            if (verticalTabs)
                painter->restore();

            painter->restore();
        }
        break;
#endif // QT_NO_TABBAR
#ifndef QT_NO_PROGRESSBAR
    case CE_ProgressBarContents:
        if (const QStyleOptionProgressBarV2 *optionProgressBar = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option)) {
            QRect progressRect = optionProgressBar->rect;

            if (optionProgressBar->minimum == optionProgressBar->maximum && optionProgressBar->minimum == 0) {
                // busy indicator
                const QS60StylePrivate::SkinElementFlag orientationFlag = optionProgressBar->orientation == Qt::Horizontal ?
                    QS60StylePrivate::SF_PointNorth : QS60StylePrivate::SF_PointWest;
                QS60StylePrivate::drawSkinPart(QS60StyleEnums::SP_QgnGrafBarWait, painter, progressRect, flags | orientationFlag);
            } else {
                const qreal progressFactor = (optionProgressBar->minimum == optionProgressBar->maximum) ? 1.0
                    : (qreal)optionProgressBar->progress / optionProgressBar->maximum;
                if (optionProgressBar->orientation == Qt::Horizontal) {
                    progressRect.setWidth(int(progressRect.width() * progressFactor));
                    if(optionProgressBar->direction == Qt::RightToLeft)
                        progressRect.translate(optionProgressBar->rect.width()-progressRect.width(),0);
                    progressRect.adjust(1, 0, -1, 0);
                } else {
                    progressRect.adjust(0, 1, 0, -1);
                    progressRect.setTop(progressRect.bottom() - int(progressRect.height() * progressFactor));
                }

                const QS60StylePrivate::SkinElements skinElement = optionProgressBar->orientation == Qt::Horizontal ?
                    QS60StylePrivate::SE_ProgressBarIndicatorHorizontal : QS60StylePrivate::SE_ProgressBarIndicatorVertical;
                QS60StylePrivate::drawSkinElement(skinElement, painter, progressRect, flags);
            }
        }
        break;
    case CE_ProgressBarGroove:
        if (const QStyleOptionProgressBarV2 *optionProgressBar = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option)) {
            const QS60StylePrivate::SkinElements skinElement = optionProgressBar->orientation == Qt::Horizontal ?
                QS60StylePrivate::SE_ProgressBarGrooveHorizontal : QS60StylePrivate::SE_ProgressBarGrooveVertical;
            QS60StylePrivate::drawSkinElement(skinElement, painter, option->rect, flags);
        }
        break;
    case CE_ProgressBarLabel:
        if (const QStyleOptionProgressBarV2 *progressbar = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option)) {
            QStyleOptionProgressBarV2 optionProgressBar = *progressbar;
            QCommonStyle::drawItemText(painter, progressbar->rect, flags | Qt::AlignCenter | Qt::TextSingleLine, optionProgressBar.palette,
                progressbar->state & State_Enabled, progressbar->text, QPalette::WindowText);
        }
        break;
#endif // QT_NO_PROGRESSBAR
#ifndef QT_NO_MENU
    case CE_MenuItem:
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            QStyleOptionMenuItem optionMenuItem = *menuItem;

            bool drawSubMenuIndicator = false;
            switch(menuItem->menuItemType) {
                case QStyleOptionMenuItem::Scroller:
                case QStyleOptionMenuItem::Separator:
                    return; // no separators or scrollers in S60 menus
                case QStyleOptionMenuItem::SubMenu:
                    drawSubMenuIndicator = true;
                    break;
                default:
                    break;
            }
            const bool enabled = optionMenuItem.state & State_Enabled;
            const bool checkable = optionMenuItem.checkType != QStyleOptionMenuItem::NotCheckable;

            uint text_flags = Qt::AlignLeading | Qt::TextShowMnemonic | Qt::TextDontClip
                            | Qt::TextSingleLine | Qt::AlignVCenter;
            if (!styleHint(SH_UnderlineShortcut, menuItem, widget))
                text_flags |= Qt::TextHideMnemonic;

            if ((option->state & State_Selected) && (option->state & State_Enabled))
                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ListHighlight, painter, option->rect, flags);

            QRect iconRect = subElementRect(SE_ItemViewItemDecoration, &optionMenuItem, widget);
            QRect textRect = subElementRect(SE_ItemViewItemText, &optionMenuItem, widget);

            //todo: move the vertical spacing stuff into subElementRect
            const int vSpacing = QS60StylePrivate::pixelMetric(QStyle::PM_LayoutVerticalSpacing);
            if (checkable){
                const int hSpacing = QS60StylePrivate::pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
                QStyleOptionMenuItem optionCheckBox;
                optionCheckBox.QStyleOptionMenuItem::operator=(*menuItem);
                optionCheckBox.rect.setWidth(pixelMetric(PM_IndicatorWidth));
                optionCheckBox.rect.setHeight(pixelMetric(PM_IndicatorHeight));
                const int moveByX = optionCheckBox.rect.width()+vSpacing;
                if (optionMenuItem.direction == Qt::LeftToRight) {
                    textRect.translate(moveByX,0);
                    iconRect.translate(moveByX, 0);
                    iconRect.setWidth(iconRect.width()+vSpacing);
                    textRect.setWidth(textRect.width()-moveByX-vSpacing);
                    optionCheckBox.rect.translate(vSpacing/2, hSpacing/2);
                } else {
                    textRect.setWidth(textRect.width()-moveByX);
                    iconRect.setWidth(iconRect.width()+vSpacing);
                    iconRect.translate(-optionCheckBox.rect.width()-vSpacing, 0);
                    optionCheckBox.rect.translate(textRect.width()+iconRect.width(),0);
                }
                drawPrimitive(PE_IndicatorMenuCheckMark, &optionCheckBox, painter, widget);
            }
            //draw icon and/or checkState
            QPixmap pix = menuItem->icon.pixmap(pixelMetric(PM_SmallIconSize),
                enabled ? QIcon::Normal : QIcon::Disabled);
            const bool itemWithIcon = !pix.isNull();
            if (itemWithIcon) {
                drawItemPixmap(painter, iconRect, text_flags, pix);
                if (optionMenuItem.direction == Qt::LeftToRight)
                    textRect.translate(vSpacing,0);
                else
                    textRect.translate(-vSpacing,0);
                textRect.setWidth(textRect.width()-vSpacing);
            }

            //draw indicators
            if (drawSubMenuIndicator) {
                QStyleOptionMenuItem arrowOptions;
                arrowOptions.QStyleOption::operator=(*menuItem);
                const int indicatorWidth = (pixelMetric(PM_ListViewIconSize, option, widget)>>1) +
                    pixelMetric(QStyle::PM_LayoutVerticalSpacing, option, widget);
                if (optionMenuItem.direction == Qt::LeftToRight)
                    arrowOptions.rect.setLeft(textRect.right());
                arrowOptions.rect.setWidth(indicatorWidth);
                //by default sub menu indicator in S60 points to east,so here icon
                // direction is set to north (and south when in RightToLeft)
                const QS60StylePrivate::SkinElementFlag arrowDirection = (arrowOptions.direction == Qt::LeftToRight) ?
                    QS60StylePrivate::SF_PointNorth : QS60StylePrivate::SF_PointSouth;
                painter->save();
                painter->setPen(option->palette.windowText().color());
                QS60StylePrivate::drawSkinPart(QS60StyleEnums::SP_QgnIndiSubMenu, painter, arrowOptions.rect,
                    (flags | QS60StylePrivate::SF_ColorSkinned | arrowDirection));
                painter->restore();
            }

            //draw text
            if (!enabled){
                //In s60, if something becomes disabled, it is removed from menu, so no native look-alike available.
                optionMenuItem.palette.setColor(QPalette::Disabled, QPalette::Text, QS60StylePrivate::lighterColor(
                        optionMenuItem.palette.color(QPalette::Disabled, QPalette::Text)));
                painter->save();
                painter->setOpacity(0.5);
            }
            QCommonStyle::drawItemText(painter, textRect, text_flags,
                    optionMenuItem.palette, enabled,
                    optionMenuItem.text, QPalette::Text);
            if (!enabled)
                painter->restore();
        }
        break;
    case CE_MenuEmptyArea:
        break;
#endif //QT_NO_MENU

#ifndef QT_NO_MENUBAR
    case CE_MenuBarEmptyArea:
        break;
#endif //QT_NO_MENUBAR

    case CE_HeaderSection:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            painter->save();
            QPen linePen = QPen(QS60StylePrivate::s60Color(QS60StyleEnums::CL_QsnLineColors, 1, header));
            const int penWidth = (header->orientation == Qt::Horizontal) ?
                linePen.width()+QS60StylePrivate::pixelMetric(PM_Custom_BoldLineWidth)
                : linePen.width()+QS60StylePrivate::pixelMetric(PM_Custom_ThinLineWidth);
            linePen.setWidth(penWidth);
            painter->setPen(linePen);
            if (header->orientation == Qt::Horizontal){
                painter->drawLine(header->rect.bottomLeft(), header->rect.bottomRight());
            } else {
                if ( header->direction == Qt::LeftToRight ) {
                    painter->drawLine(header->rect.topRight(), header->rect.bottomRight());
                } else {
                    painter->drawLine(header->rect.topLeft(), header->rect.bottomLeft());
                }
            }
            painter->restore();

            //Draw corner button as normal pushButton.
            if (qobject_cast<const QAbstractButton *>(widget)) {
                //Make cornerButton slightly smaller so that it is not on top of table border graphic.
                QStyleOptionHeader subopt = *header;
                const int borderTweak = 
                    QS60StylePrivate::pixelMetric(PM_Custom_FrameCornerWidth)>>1;
                if (subopt.direction == Qt::LeftToRight)
                    subopt.rect.adjust(borderTweak, borderTweak, 0, -borderTweak);
                else
                    subopt.rect.adjust(0, borderTweak, -borderTweak, -borderTweak);
                drawPrimitive(PE_PanelButtonBevel, &subopt, painter, widget);
            } else if ((header->palette.brush(QPalette::Button) != Qt::transparent)) {
                //Draw non-themed background. Background for theme is drawn in CE_ShapedFrame
                //to get continuous theme graphic across all the header cells.
                qDrawShadePanel(painter, header->rect, header->palette,
                                header->state & (State_Sunken | State_On), penWidth,
                                &header->palette.brush(QPalette::Button));
            }
        }
        break;
    case CE_HeaderEmptyArea: // no need to draw this
        break;
    case CE_Header:
        if ( const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            drawControl(CE_HeaderSection, header, painter, widget);
            QStyleOptionHeader subopt = *header;
            subopt.rect = subElementRect(SE_HeaderLabel, header, widget);
            if (subopt.rect.isValid())
                drawControl(CE_HeaderLabel, &subopt, painter, widget);
            if (header->sortIndicator != QStyleOptionHeader::None) {
                subopt.rect = subElementRect(SE_HeaderArrow, option, widget);
                drawPrimitive(PE_IndicatorHeaderArrow, &subopt, painter, widget);
            }
        }
        break;
#ifndef QT_NO_TOOLBAR
    case CE_ToolBar:
        if (const QStyleOptionToolBar *toolBar = qstyleoption_cast<const QStyleOptionToolBar *>(option)) {
            const QToolBar *tbWidget = qobject_cast<const QToolBar *>(widget);

            //toolbar within a toolbar, skip
            if (!tbWidget || (widget && qobject_cast<QToolBar *>(widget->parentWidget())))
                break;

            // Normally in S60 5.0+ there is no background for toolbar, but in some cases with versatile QToolBar,
            // it looks a bit strange. So, lets fillRect with Button.
            if (!QS60StylePrivate::isToolBarBackground()) {
                QList<QAction *> actions = tbWidget->actions();
                bool justToolButtonsInToolBar = true;
                for (int i = 0; i < actions.size(); ++i) {
                    QWidget *childWidget = tbWidget->widgetForAction(actions.at(i));
                    const QToolButton *button = qobject_cast<const QToolButton *>(childWidget);
                    if (!button){
                        justToolButtonsInToolBar = false;
                    }
                }

                // Draw frame background
                // for vertical toolbars with text only and
                // for toolbars with extension buttons and
                // for toolbars with widgets in them.
                if (!justToolButtonsInToolBar ||
                        (tbWidget &&
                         (tbWidget->orientation() == Qt::Vertical) &&
                         (tbWidget->toolButtonStyle() == Qt::ToolButtonTextOnly))) {
                        painter->save();
                        if (widget)
                            painter->setBrush(widget->palette().button());
                        painter->setOpacity(0.3);
                        painter->fillRect(toolBar->rect, painter->brush());
                        painter->restore();
                }
            } else {
                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_ToolBar, painter, toolBar->rect, flags);
            }
        }
        break;
#endif //QT_NO_TOOLBAR
    case CE_ShapedFrame:
        if (const QTextEdit *textEdit = qobject_cast<const QTextEdit *>(widget)) {
            const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(option);
            if (QS60StylePrivate::canDrawThemeBackground(frame->palette.base()))
                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_Editor, painter, option->rect, flags);
            else
                QCommonStyle::drawControl(element, option, painter, widget);
        } else if (qobject_cast<const QTableView *>(widget)) {
            QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_TableItem, painter, option->rect, flags);
        } else if (const QHeaderView *header = qobject_cast<const QHeaderView *>(widget)) {
            //QS60style draws header background here instead of in each headersection, to get
            //continuous graphic from section to section.
            QS60StylePrivate::SkinElementFlags adjustableFlags = flags;
            QRect headerRect = option->rect;
            if (header->orientation() != Qt::Horizontal) {
                //todo: update to horizontal table graphic
                adjustableFlags = (adjustableFlags | QS60StylePrivate::SF_PointWest);
            } else {
                const int frameWidth = QS60StylePrivate::pixelMetric(PM_DefaultFrameWidth);
                if (option->direction == Qt::LeftToRight) 
                    headerRect.adjust(-2*frameWidth, 0, 0, 0);
                else
                    headerRect.adjust(0, 0, 2*frameWidth, 0);
            }
            if (option->palette.brush(QPalette::Button).color() == Qt::transparent)
                QS60StylePrivate::drawSkinElement(
                        QS60StylePrivate::SE_TableHeaderItem, painter, headerRect, adjustableFlags);

        } else if (qobject_cast<const QFrame *>(widget)) {
            QCommonStyle::drawControl(element, option, painter, widget);
        }
        break;
    case CE_MenuScroller:
        break;
    case CE_FocusFrame:
        {
            // The pen width should nearly fill the layoutspacings around the widget
            const int penWidth =
                qMin(pixelMetric(QS60Style::PM_LayoutVerticalSpacing), pixelMetric(QS60Style::PM_LayoutHorizontalSpacing))
                - 2; // But keep 1 pixel distance to the focus widget and 1 pixel to the adjacent widgets

#ifdef QT_KEYPAD_NAVIGATION
            bool editFocus = false;
            if (const QFocusFrame *focusFrame = qobject_cast<const QFocusFrame*>(widget)) {
                if (focusFrame->widget() && focusFrame->widget()->hasEditFocus())
                    editFocus = true;
            }
            const qreal opacity = editFocus ? 0.65 : 0.45; // Trial and error factors. Feel free to improve.
#else
            const qreal opacity = 0.5;
#endif
            // Because of Qts coordinate system, we need to tweak the rect by .5 pixels, otherwise it gets blurred.
            const qreal rectAdjustment = (penWidth % 2) ? -.5 : 0;

            // Make sure that the pen stroke is inside the rect
            const QRectF adjustedRect =
                QRectF(option->rect).adjusted(
                    rectAdjustment + penWidth,
                    rectAdjustment + penWidth,
                    -rectAdjustment - penWidth,
                    -rectAdjustment - penWidth
                );

            const qreal roundRectRadius = penWidth * goldenRatio;

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setOpacity(opacity);
            painter->setPen(QPen(option->palette.color(QPalette::Text), penWidth));
            painter->drawRoundedRect(adjustedRect, roundRectRadius, roundRectRadius);
            painter->restore();
        }
        break;
    case CE_Splitter:
        if (option->state & State_Sunken && option->state & State_Enabled) {
            painter->save();
            painter->setOpacity(0.5);
            painter->setBrush(QS60StylePrivate::themePalette()->light());
            painter->setRenderHint(QPainter::Antialiasing);
            const qreal roundRectRadius = 4 * goldenRatio;
            painter->drawRoundedRect(option->rect, roundRectRadius, roundRectRadius);
            painter->restore();
        }
        break;
    default:
        QCommonStyle::drawControl(element, option, painter, widget);
    }
}

/*!
  \reimp
*/
void QS60Style::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D(const QS60Style);
    const QS60StylePrivate::SkinElementFlags flags = (option->state & State_Enabled) ?  QS60StylePrivate::SF_StateEnabled : QS60StylePrivate::SF_StateDisabled;
    bool commonStyleDraws = false;

    switch (element) {
#ifndef QT_NO_LINEEDIT
    case PE_PanelLineEdit:
        if (const QStyleOptionFrame *lineEdit = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
#ifndef QT_NO_COMBOBOX
            if (widget && qobject_cast<const QComboBox *>(widget->parentWidget()))
                break;
#endif
            if (QS60StylePrivate::canDrawThemeBackground(option->palette.base()))
                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_FrameLineEdit, painter, option->rect, flags);
            else
                commonStyleDraws = true;
        }
    break;
#endif // QT_NO_LINEEDIT
    case PE_IndicatorCheckBox:
        {
            // Draw checkbox indicator as color skinned graphics.
            const QS60StyleEnums::SkinParts skinPart = (option->state & QStyle::State_On) ?
                QS60StyleEnums::SP_QgnIndiCheckboxOn : QS60StyleEnums::SP_QgnIndiCheckboxOff;
            painter->save();

            QColor themeColor = QS60StylePrivate::themePalette()->windowText().color();
            QColor windowTextColor = option->palette.windowText().color();

            if (themeColor != windowTextColor)
                painter->setPen(windowTextColor);

            QS60StylePrivate::drawSkinPart(skinPart, painter, option->rect, flags | QS60StylePrivate::SF_ColorSkinned );
            painter->restore();
        }
        break;
    case PE_IndicatorViewItemCheck:
#ifndef QT_NO_ITEMVIEWS
        if (const QListView *listItem = (qobject_cast<const QListView *>(widget))) {
            if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
                const bool checkBoxVisible = vopt->features & QStyleOptionViewItemV2::HasCheckIndicator;
                const bool singleSelection = listItem->selectionMode() ==
                    QAbstractItemView::SingleSelection || listItem->selectionMode() == QAbstractItemView::NoSelection;
                // draw either checkbox at the beginning
                if (checkBoxVisible && singleSelection) {
                    drawPrimitive(PE_IndicatorCheckBox, option, painter, widget);
                // ... or normal "tick" selection at the end.
                } else if (option->state & QStyle::State_Selected) {
                    QRect tickRect = option->rect;
                    const int frameBorderWidth = QS60StylePrivate::pixelMetric(PM_Custom_FrameCornerWidth);
                    // adjust tickmark rect to exclude frame border
                    tickRect.adjust(0,-frameBorderWidth,0,-frameBorderWidth);
                    QS60StyleEnums::SkinParts skinPart = QS60StyleEnums::SP_QgnIndiMarkedAdd;
                    QS60StylePrivate::drawSkinPart(skinPart, painter, tickRect,
                        (flags | QS60StylePrivate::SF_ColorSkinned));
                }
            }
        }
#endif //QT_NO_ITEMVIEWS
        break;
    case PE_IndicatorRadioButton: {
            QRect buttonRect = option->rect;
            //there is empty (a. 33%) space in svg graphics for radiobutton
            const qreal reduceWidth = (qreal)buttonRect.width()/3.0;
            const qreal rectWidth = (qreal)option->rect.width() != 0 ? option->rect.width() : 1.0;
            // Try to occupy the full area
            const qreal scaler = 1 + (reduceWidth/rectWidth);
            buttonRect.setWidth((int)((buttonRect.width()-reduceWidth) * scaler));
            buttonRect.setHeight((int)(buttonRect.height() * scaler));
            // move the rect up for half of the new height-gain
            const int newY = (buttonRect.bottomRight().y() - option->rect.bottomRight().y()) >> 1 ;
            buttonRect.adjust(0,-newY,0,-newY);

            painter->save();
            QColor themeColor = d->s60Color(QS60StyleEnums::CL_QsnTextColors, 6, option);
            QColor buttonTextColor = option->palette.buttonText().color();
            if (themeColor != buttonTextColor)
                painter->setPen(buttonTextColor);
            else
                painter->setPen(themeColor);

            // Draw radiobutton indicator as color skinned graphics.
            QS60StyleEnums::SkinParts skinPart = (option->state & QStyle::State_On) ?
                QS60StyleEnums::SP_QgnIndiRadiobuttOn : QS60StyleEnums::SP_QgnIndiRadiobuttOff;
            QS60StylePrivate::drawSkinPart(skinPart, painter, buttonRect,
                (flags | QS60StylePrivate::SF_ColorSkinned));
            painter->restore();
        }
        break;
    case PE_PanelButtonCommand:
    case PE_PanelButtonTool:
    case PE_PanelButtonBevel:
    case PE_FrameButtonBevel: {
        if (QS60StylePrivate::canDrawThemeBackground(option->palette.base())) {
            const bool isPressed = option->state & QStyle::State_Sunken;
            const QS60StylePrivate::SkinElements skinElement =
                isPressed ? QS60StylePrivate::SE_ButtonPressed : QS60StylePrivate::SE_ButtonNormal;
            QS60StylePrivate::drawSkinElement(skinElement, painter, option->rect, flags);
        } else {
            commonStyleDraws = true;
            }
        }
        break;
#ifndef QT_NO_TOOLBUTTON
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowLeft:
    case PE_IndicatorArrowRight:
    case PE_IndicatorArrowUp: {
        QS60StyleEnums::SkinParts skinPart;
        if (element==PE_IndicatorArrowDown)
            skinPart = QS60StyleEnums::SP_QgnGrafScrollArrowDown;
        else if (element==PE_IndicatorArrowLeft)
            skinPart = QS60StyleEnums::SP_QgnGrafScrollArrowLeft;
        else if (element==PE_IndicatorArrowRight)
            skinPart = QS60StyleEnums::SP_QgnGrafScrollArrowRight;
        else if (element==PE_IndicatorArrowUp)
            skinPart = QS60StyleEnums::SP_QgnGrafScrollArrowUp;

        QS60StylePrivate::drawSkinPart(skinPart, painter, option->rect, flags);
        }
        break;
#endif //QT_NO_TOOLBUTTON
#ifndef QT_NO_SPINBOX
    case PE_IndicatorSpinDown:
    case PE_IndicatorSpinUp:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QStyleOptionSpinBox optionSpinBox = *spinBox;
            if (QS60StylePrivate::canDrawThemeBackground(optionSpinBox.palette.base())) {
                const QS60StyleEnums::SkinParts part = (element == PE_IndicatorSpinUp) ?
                    QS60StyleEnums::SP_QgnGrafScrollArrowUp :
                    QS60StyleEnums::SP_QgnGrafScrollArrowDown;
                const int adjustment = qMin(optionSpinBox.rect.width(), optionSpinBox.rect.height())/6;
                optionSpinBox.rect.translate(0, (element == PE_IndicatorSpinDown) ? adjustment : -adjustment );
                QS60StylePrivate::drawSkinPart(part, painter, optionSpinBox.rect,flags);
            } else {
                commonStyleDraws = true;
            }
        }
#ifndef QT_NO_COMBOBOX
        else if (const QStyleOptionFrame *cmb = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            if (QS60StylePrivate::canDrawThemeBackground( option->palette.base())) {
                // We want to draw down arrow here for comboboxes as well.
                const QS60StyleEnums::SkinParts part = QS60StyleEnums::SP_QgnGrafScrollArrowDown;
                QStyleOptionFrame comboBox = *cmb;
                const int adjustment = qMin(comboBox.rect.width(), comboBox.rect.height())/6;
                comboBox.rect.translate(0, (element == PE_IndicatorSpinDown) ? adjustment : -adjustment );
                QS60StylePrivate::drawSkinPart(part, painter, comboBox.rect,flags);
            } else {
                commonStyleDraws = true;
            }
        }
#endif //QT_NO_COMBOBOX
        break;
    case PE_IndicatorSpinMinus:
    case PE_IndicatorSpinPlus:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QStyleOptionSpinBox optionSpinBox = *spinBox;
            QCommonStyle::drawPrimitive(element, &optionSpinBox, painter, widget);
        }
#ifndef QT_NO_COMBOBOX
        else if (const QStyleOptionFrame *cmb = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            // We want to draw down arrow here for comboboxes as well.
            QStyleOptionFrame comboBox = *cmb;
            const int frameWidth = QS60StylePrivate::pixelMetric(PM_DefaultFrameWidth);
            comboBox.rect.adjust(0,frameWidth,0,-frameWidth);
            QCommonStyle::drawPrimitive(element, &comboBox, painter, widget);
        }
#endif //QT_NO_COMBOBOX
        break;
#endif //QT_NO_SPINBOX
    case PE_Widget:
        if (QS60StylePrivate::drawsOwnThemeBackground(widget)
#ifndef QT_NO_COMBOBOX
            || qobject_cast<const QComboBoxListView *>(widget)
#endif //QT_NO_COMBOBOX
#ifndef QT_NO_MENU
            || qobject_cast<const QMenu *> (widget)
#endif //QT_NO_MENU
            ) {
            if (QS60StylePrivate::canDrawThemeBackground(option->palette.base()))
                QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_OptionsMenu, painter, option->rect, flags);
            else
                commonStyleDraws = true;
        }
        break;
    case PE_FrameWindow:
    case PE_FrameTabWidget:
        if (const QStyleOptionTabWidgetFrame *tabFrame = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option)) {
            QStyleOptionTabWidgetFrame optionTabFrame = *tabFrame;
            QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_PanelBackground, painter, optionTabFrame.rect, flags);
        }
        break;
    case PE_IndicatorHeaderArrow:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            if (header->sortIndicator & QStyleOptionHeader::SortUp)
                drawPrimitive(PE_IndicatorArrowUp, header, painter, widget);
            else if (header->sortIndicator & QStyleOptionHeader::SortDown)
                drawPrimitive(PE_IndicatorArrowDown, header, painter, widget);
        } // QStyleOptionHeader::None is not drawn => not needed
        break;
#ifndef QT_NO_GROUPBOX
    case PE_FrameGroupBox:
        if (const QStyleOptionFrameV2 *frame = qstyleoption_cast<const QStyleOptionFrameV2 *>(option))
            QS60StylePrivate::drawSkinElement(QS60StylePrivate::SE_SettingsList, painter, frame->rect, flags);
        break;
#endif //QT_NO_GROUPBOX

    // Qt3 primitives are not supported
    case PE_Q3CheckListController:
    case PE_Q3CheckListExclusiveIndicator:
    case PE_Q3CheckListIndicator:
    case PE_Q3DockWindowSeparator:
    case PE_Q3Separator:
        Q_ASSERT(false);
        break;
    case PE_Frame:
        break;
#ifndef QT_NO_ITEMVIEWS
    case PE_PanelItemViewItem:
    case PE_PanelItemViewRow: // ### Qt 5: remove
        break;
#endif //QT_NO_ITEMVIEWS

    case PE_IndicatorMenuCheckMark:
        if (const QStyleOptionMenuItem *checkBox = qstyleoption_cast<const QStyleOptionMenuItem *>(option)){
            QStyleOptionMenuItem optionCheckBox = *checkBox;
            if (optionCheckBox.checked)
                optionCheckBox.state = (optionCheckBox.state | State_On);
            drawPrimitive(PE_IndicatorCheckBox, &optionCheckBox, painter, widget);
        }
        break;
#ifndef QT_NO_TOOLBAR
    case PE_IndicatorToolBarHandle:
        // no toolbar handles in S60/AVKON UI
    case PE_IndicatorToolBarSeparator:
        // no separators in S60/AVKON UI
        break;
#endif //QT_NO_TOOLBAR

    case PE_PanelMenuBar:
    case PE_FrameMenu:
        break; //disable frame in menu

    case PE_IndicatorBranch:
#if defined(Q_WS_S60)
        // 3.1 AVKON UI does not have tree view component, use common style for drawing there
        if (QSysInfo::s60Version() == QSysInfo::SV_S60_3_1) {
#else
        if (true) {
#endif
            QCommonStyle::drawPrimitive(element, option, painter, widget);
        } else {
            const bool rightLine = option->state & State_Item;
            const bool downLine = option->state & State_Sibling;
            const bool upLine = option->state & (State_Open | State_Children | State_Item | State_Sibling);

            QS60StyleEnums::SkinParts skinPart;
            bool drawSkinPart = false;
            if (rightLine && downLine && upLine) {
                skinPart = QS60StyleEnums::SP_QgnIndiHlLineBranch;
                drawSkinPart = true;
            } else if (rightLine && upLine) {
                skinPart = QS60StyleEnums::SP_QgnIndiHlLineEnd;
                drawSkinPart = true;
            } else if (upLine && downLine) {
                skinPart = QS60StyleEnums::SP_QgnIndiHlLineStraight;
                drawSkinPart = true;
            }

            if (drawSkinPart)
                QS60StylePrivate::drawSkinPart(skinPart, painter, option->rect, flags);

            if (option->state & State_Children) {
                QS60StyleEnums::SkinParts skinPart =
                        (option->state & State_Open) ? QS60StyleEnums::SP_QgnIndiHlColSuper : QS60StyleEnums::SP_QgnIndiHlExpSuper;
                int minDimension = qMin(option->rect.width(), option->rect.height());
                const int resizeValue = minDimension >> 1;
                minDimension += resizeValue; // Adjust the icon bigger because of empty space in svg icon.
                QRect iconRect(option->rect.topLeft(), QSize(minDimension, minDimension));
                int verticalMagic(0);
                // magic values for positioning svg icon.
                if (option->rect.width() <= option->rect.height())
                    verticalMagic = 3;
                iconRect.translate(3, verticalMagic - resizeValue);
                QS60StylePrivate::drawSkinPart(skinPart, painter, iconRect, flags);
            }
        }
        break;

        // todo: items are below with #ifdefs "just in case". in final version, remove all non-required cases
    case PE_FrameLineEdit:
    case PE_IndicatorDockWidgetResizeHandle:
    case PE_PanelTipLabel:
    case PE_PanelScrollAreaCorner:

#ifndef QT_NO_TABBAR
    case PE_IndicatorTabTear: // No tab tear in S60
#endif // QT_NO_TABBAR
    case PE_FrameDefaultButton:
#ifndef QT_NO_DOCKWIDGET
    case PE_FrameDockWidget:
#endif //QT_NO_DOCKWIDGET
#ifndef QT_NO_PROGRESSBAR
    case PE_IndicatorProgressChunk:
#endif //QT_NO_PROGRESSBAR
#ifndef QT_NO_TOOLBAR
    case PE_PanelToolBar:
#endif //QT_NO_TOOLBAR
#ifndef QT_NO_COLUMNVIEW
    case PE_IndicatorColumnViewArrow:
    case PE_IndicatorItemViewItemDrop:
#endif //QT_NO_COLUMNVIEW
    case PE_FrameTabBarBase: // since tabs are in S60 always in navipane, let's use common style for tab base in Qt.
    default:
        commonStyleDraws = true;
    }
    if (commonStyleDraws) {
        QCommonStyle::drawPrimitive(element, option, painter, widget);
    }
}

/*! \reimp */
int QS60Style::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    int metricValue = QS60StylePrivate::pixelMetric(metric);
    if (metricValue == KNotFound)
        metricValue = QCommonStyle::pixelMetric(metric, option, widget);

    if (metric == PM_SubMenuOverlap && widget) {
        const QMenu *menu = qobject_cast<const QMenu *>(widget);
        if (menu && menu->activeAction() && menu->activeAction()->menu()) {
            const int menuWidth = menu->activeAction()->menu()->sizeHint().width();
            metricValue = -menuWidth;
        }
    }
    return metricValue;
}

/*! \reimp */
QSize QS60Style::sizeFromContents(ContentsType ct, const QStyleOption *opt,
                                  const QSize &csz, const QWidget *widget) const
{
    QSize sz(csz);
    switch (ct) {
        case CT_ToolButton:
            sz = QCommonStyle::sizeFromContents( ct, opt, csz, widget);
            //FIXME properly - style should calculate the location of border frame-part
            sz += QSize(2*pixelMetric(PM_ButtonMargin), 2*pixelMetric(PM_ButtonMargin));
            if (const QStyleOptionToolButton *toolBtn = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
                if (toolBtn->subControls & SC_ToolButtonMenu)
                    sz += QSize(pixelMetric(PM_MenuButtonIndicator),0);
            break;
        case CT_PushButton:
            sz = QCommonStyle::sizeFromContents( ct, opt, csz, widget);
            //FIXME properly - style should calculate the location of border frame-part
            sz += QSize(2*pixelMetric(PM_ButtonMargin), 2*pixelMetric(PM_ButtonMargin));
            if (const QAbstractButton *buttonWidget = (qobject_cast<const QAbstractButton *>(widget)))
                if (buttonWidget->isCheckable())
                    sz += QSize(pixelMetric(PM_IndicatorWidth) + pixelMetric(PM_CheckBoxLabelSpacing), 0);
            break;
        case CT_LineEdit:
            if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt))
                sz += QSize(2*f->lineWidth, 4*f->lineWidth);
            break;
        case CT_TabBarTab:
            {
                const QSize naviPaneSize = QS60StylePrivate::naviPaneSize();
                sz = QCommonStyle::sizeFromContents(ct, opt, csz, widget);
                if (naviPaneSize.height() > sz.height())
                    sz.setHeight(naviPaneSize.height());
            }
            break;
        case CT_ItemViewItem:
            sz = QCommonStyle::sizeFromContents( ct, opt, csz, widget);
            if (QS60StylePrivate::isTouchSupported())
                //Make itemview easier to use in touch devices
                //QCommonStyle does not adjust height with horizontal margin, it only adjusts width
                sz.setHeight(sz.height() + 2*pixelMetric(QStyle::PM_FocusFrameVMargin));
            break;
        default:
            sz = QCommonStyle::sizeFromContents( ct, opt, csz, widget);
            break;
    }
    return sz;
}

/*! \reimp */
int QS60Style::styleHint(StyleHint sh, const QStyleOption *opt, const QWidget *widget,
                            QStyleHintReturn *hret) const
{
    int retValue = -1;
    switch (sh) {
        case SH_Table_GridLineColor:
            retValue = QS60StylePrivate::s60Color(QS60StyleEnums::CL_QsnLineColors,2,0).rgb();
            break;
        case SH_GroupBox_TextLabelColor:
            retValue = QS60StylePrivate::s60Color(QS60StyleEnums::CL_QsnTextColors,6,0).rgb();
            break;
        case SH_ScrollBar_ScrollWhenPointerLeavesControl:
            retValue = true;
            break;
        case SH_Slider_SnapToValue:
            retValue = true;
            break;
        case SH_Slider_StopMouseOverSlider:
            retValue = true;
            break;
        case SH_LineEdit_PasswordCharacter:
            retValue = '*';
            break;
        case SH_ComboBox_PopupFrameStyle:
            retValue = QFrame::NoFrame | QFrame::Plain;
            break;
        case SH_Dial_BackgroundRole:
            retValue = QPalette::Base;
            break;
        case SH_ItemView_ActivateItemOnSingleClick:
            retValue = true;
            break;
        case SH_ProgressDialog_TextLabelAlignment:
            retValue = (QApplication::layoutDirection() == Qt::LeftToRight) ?
                Qt::AlignLeft :
                Qt::AlignRight;
            break;
        case SH_Menu_SubMenuPopupDelay:
            retValue = 300;
            break;
        case SH_Menu_Scrollable:
            retValue = true;
            break;
        case SH_Menu_SelectionWrap:
            retValue = true;
            break;
        case SH_ItemView_ShowDecorationSelected:
            retValue = true;
            break;
        case SH_ToolBar_Movable:
            retValue = false;
            break;
        case SH_BlinkCursorWhenTextSelected:
            retValue = true;
            break;
        case SH_UnderlineShortcut:
            retValue = 0;
            break;
        case SH_RequestSoftwareInputPanel:
            retValue = RSIP_OnMouseClickAndAlreadyFocused;
            break;
        default:
            break;
    }
    if (retValue == -1)
        retValue = QCommonStyle::styleHint(sh, opt, widget, hret);
    return retValue;
}

/*! \reimp */
QRect QS60Style::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl scontrol, const QWidget *widget) const
{
    QRect ret;
    switch (control) {
#ifndef QT_NO_SCROLLBAR
    // This implementation of subControlRect(CC_ScrollBar..) basically just removes the SC_ScrollBarSubLine and SC_ScrollBarAddLine
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollbarOption = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            const QRect scrollBarRect = scrollbarOption->rect;
            const bool isHorizontal = scrollbarOption->orientation == Qt::Horizontal;
            const int maxlen = isHorizontal ? scrollBarRect.width() : scrollBarRect.height();
            int sliderlen;

            // calculate slider length
            if (scrollbarOption->maximum != scrollbarOption->minimum) {
                const uint range = scrollbarOption->maximum - scrollbarOption->minimum;
                sliderlen = (qint64(scrollbarOption->pageStep) * maxlen) / (range + scrollbarOption->pageStep);

                const int slidermin = pixelMetric(PM_ScrollBarSliderMin, scrollbarOption, widget);
                if (sliderlen < slidermin || range > (INT_MAX>>1))
                    sliderlen = slidermin;
                if (sliderlen > maxlen)
                    sliderlen = maxlen;
            } else {
                sliderlen = maxlen;
            }

            const int sliderstart = sliderPositionFromValue(scrollbarOption->minimum,
                                                            scrollbarOption->maximum,
                                                            scrollbarOption->sliderPosition,
                                                            maxlen - sliderlen,
                                                            scrollbarOption->upsideDown);

            switch (scontrol) {
                case SC_ScrollBarSubPage:            // between top/left button and slider
                    if (isHorizontal)
                        ret.setRect(0, 0, sliderstart, scrollBarRect.height());
                    else
                        ret.setRect(0, 0, scrollBarRect.width(), sliderstart);
                    break;
                case SC_ScrollBarAddPage: {         // between bottom/right button and slider
                    const int addPageLength = sliderstart + sliderlen;
                    if (isHorizontal)
                        ret = scrollBarRect.adjusted(addPageLength, 0, 0, 0);
                    else
                        ret = scrollBarRect.adjusted(0, addPageLength, 0, 0);
                    }
                    break;
                case SC_ScrollBarGroove:
                    ret = scrollBarRect;
                    break;
                case SC_ScrollBarSlider:
                    if (scrollbarOption->orientation == Qt::Horizontal)
                        ret.setRect(sliderstart, 0, sliderlen, scrollBarRect.height());
                    else
                        ret.setRect(0, sliderstart, scrollBarRect.width(), sliderlen);
                    break;
                case SC_ScrollBarSubLine:            // top/left button
                case SC_ScrollBarAddLine:            // bottom/right button
                default:
                    break;
            }
            ret = visualRect(scrollbarOption->direction, scrollBarRect, ret);
        }
        break;
#endif // QT_NO_SCROLLBAR
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            const int frameThickness = spinbox->frame ? pixelMetric(PM_SpinBoxFrameWidth, spinbox, widget) : 0;
            const int buttonMargin = spinbox->frame ? 2 : 0;
            const int buttonWidth = QS60StylePrivate::pixelMetric(QStyle::PM_ButtonIconSize) + 2*buttonMargin;
            QSize buttonSize;
            buttonSize.setHeight(qMax(8, spinbox->rect.height() - frameThickness));
            buttonSize.setWidth(buttonWidth);
            buttonSize = buttonSize.expandedTo(QApplication::globalStrut());

            const int y = frameThickness + spinbox->rect.y();
            const int x = spinbox->rect.x() + spinbox->rect.width() - frameThickness - 2*buttonSize.width();

            switch (scontrol) {
                case SC_SpinBoxUp:
                    if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                        return QRect();
                    ret = QRect(x, y, buttonWidth, buttonSize.height());
                    break;
                case SC_SpinBoxDown:
                    if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                        return QRect();
                    ret = QRect(x+buttonSize.width(), y, buttonWidth, buttonSize.height());
                    break;
                case SC_SpinBoxEditField:
                    if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                        ret = QRect(
                                frameThickness,
                                frameThickness,
                                spinbox->rect.width() - 2*frameThickness,
                                spinbox->rect.height() - 2*frameThickness);
                    else
                        ret = QRect(
                                frameThickness,
                                frameThickness,
                                x - frameThickness,
                                spinbox->rect.height() - 2*frameThickness);
                    break;
                case SC_SpinBoxFrame:
                    ret = spinbox->rect;
                    break;
                default:
                    break;
            }
        ret = visualRect(spinbox->direction, spinbox->rect, ret);
        }
        break;
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            ret = cmb->rect;
            const int width = cmb->rect.width();
            const int height = cmb->rect.height();
            const int buttonMargin = cmb->frame ? 2 : 0;
            // lets use spinbox frame here as well, as no combobox specific value available.
            const int frameThickness = cmb->frame ? pixelMetric(PM_SpinBoxFrameWidth, cmb, widget) : 0;
            const int buttonWidth = QS60StylePrivate::pixelMetric(QStyle::PM_ButtonIconSize);

            QSize buttonSize;
            buttonSize.setHeight(qMax(8, (cmb->rect.height()>>1) - frameThickness)); //minimum of 8 pixels
            buttonSize.setWidth(buttonWidth+2*buttonMargin);
            buttonSize = buttonSize.expandedTo(QApplication::globalStrut());
            switch (scontrol) {
                case SC_ComboBoxArrow:
                    ret.setRect(
                        ret.x() + ret.width() - buttonMargin - buttonWidth,
                        ret.y() + buttonMargin,
                        buttonWidth,
                        height - 2*buttonMargin);
                    break;
                case SC_ComboBoxEditField: {
                    ret.setRect(
                        ret.x() + frameThickness,
                        ret.y() + frameThickness,
                        ret.width() - 2*frameThickness - buttonSize.width(),
                        ret.height() - 2*frameThickness);
                    }
                break;
            default:
                break;
            }
        }
        break;
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            ret = QCommonStyle::subControlRect(control, option, scontrol, widget);
            switch (scontrol) {
                case SC_GroupBoxCheckBox: //fallthrough
                case SC_GroupBoxLabel: {
                    //slightly indent text and boxes, so that dialog border does not mess with them.
                    const int horizontalSpacing =
                        QS60StylePrivate::pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
                    ret.adjust(2,horizontalSpacing-3,0,0);
                    }
                    break;
                case SC_GroupBoxFrame: {
                    const QRect textBox = subControlRect(control, option, SC_GroupBoxLabel, widget);
                    const int tbHeight = textBox.height();
                    ret.translate(0, -ret.y());
                    // include title to within the groupBox frame
                    ret.setHeight(ret.height()+tbHeight);
                    if (widget && ret.bottom() > widget->rect().bottom())
                        ret.setBottom(widget->rect().bottom());
                    }
                    break;
                default:
                    break;
            }
        }
        break;
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolButton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            const int indicatorRect = pixelMetric(PM_MenuButtonIndicator) + 2*pixelMetric(PM_ButtonMargin);
            const int border = pixelMetric(PM_ButtonMargin) + pixelMetric(PM_DefaultFrameWidth);
            ret = toolButton->rect;
            const bool popup = (toolButton->features & 
                    (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay))
                    == QStyleOptionToolButton::MenuButtonPopup;
            switch (scontrol) {
            case SC_ToolButton:
                if (popup)
                    ret.adjust(0, 0, -indicatorRect, 0);
                break;
            case SC_ToolButtonMenu:
                if (popup)
                    ret.adjust(ret.width() - indicatorRect, border, -pixelMetric(PM_ButtonMargin), -border);
                break;
            default:
                break;
            }
            ret = visualRect(toolButton->direction, toolButton->rect, ret);
        }
        break;
    default:
        ret = QCommonStyle::subControlRect(control, option, scontrol, widget);
    }
    return ret;
}

/*!
  \reimp
*/
QRect QS60Style::subElementRect(SubElement element, const QStyleOption *opt, const QWidget *widget) const
{
    QRect ret;
    switch (element) {
        case SE_LineEditContents: {
                // in S60 the input text box doesn't start from line Edit's TL, but
                // a bit indented.
                QRect lineEditRect = opt->rect;
                const int adjustment = opt->rect.height()>>2;
                lineEditRect.adjust(adjustment,0,0,0);
                ret = lineEditRect;
            }
            break;
        case SE_TabBarTearIndicator:
            ret = QRect(0,0,0,0);
            break;
        case SE_TabWidgetTabBar:
            if (const QStyleOptionTabWidgetFrame *optionTab = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt)) {
                ret = QCommonStyle::subElementRect(element, opt, widget);

                if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt)) {
                    const int tabOverlapNoBorder =
                        QS60StylePrivate::pixelMetric(QStyle::PM_TabBarTabOverlap);
                    const int tabOverlap =
                        tabOverlapNoBorder-QS60StylePrivate::pixelMetric(QStyle::PM_DefaultFrameWidth);
                    const QTabWidget *tab = qobject_cast<const QTabWidget *>(widget);
                    int gain = (tab) ? tabOverlap * tab->count() : 0;
                    switch (twf->shape) {
                        case QTabBar::RoundedNorth:
                        case QTabBar::TriangularNorth:
                        case QTabBar::RoundedSouth:
                        case QTabBar::TriangularSouth: {
                            if (widget) {
                                // make sure that gain does not set the rect outside of widget boundaries
                                if (twf->direction == Qt::RightToLeft) {
                                    if ((ret.left() - gain) < widget->rect().left())
                                        gain = widget->rect().left()-ret.left();
                                    ret.adjust(-gain,0,0,0);
                                } else {
                                    if ((ret.right() + gain) > widget->rect().right())
                                        gain = widget->rect().right()-ret.right();
                                    ret.adjust(0,0,gain,0);
                                    }
                            }
                            break;
                        }
                        default: {
                            if (widget) {
                                if ((ret.bottom() + gain) > widget->rect().bottom())
                                    gain = widget->rect().bottom()-ret.bottom();
                                ret.adjust(0,0,0,gain);
                            }
                            break;
                        }
                    }
                }
            }
            break;
        case SE_ItemViewItemText:
        case SE_ItemViewItemDecoration:
            if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(opt)) {
                const QListWidget *listItem = qobject_cast<const QListWidget *>(widget);
                const bool multiSelection = !listItem ? false :
                    listItem->selectionMode() == QAbstractItemView::MultiSelection ||
                    listItem->selectionMode() == QAbstractItemView::ExtendedSelection ||
                    listItem->selectionMode() == QAbstractItemView::ContiguousSelection;
                ret = QCommonStyle::subElementRect(element, opt, widget);
                // If both multiselect & check-state, then remove checkbox and move
                // text and decoration towards the beginning
                if (listItem &&
                    multiSelection &&
                    (vopt->features & QStyleOptionViewItemV2::HasCheckIndicator)) {
                    const int verticalSpacing =
                        QS60StylePrivate::pixelMetric(QStyle::PM_LayoutVerticalSpacing);
                    //const int horizontalSpacing = QS60StylePrivate::pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
                    const int checkBoxRectWidth = subElementRect(SE_ItemViewItemCheckIndicator, opt, widget).width();
                    ret.adjust(-checkBoxRectWidth-verticalSpacing,0,-checkBoxRectWidth-verticalSpacing,0);
                }
            } else if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
                const bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
                const int indicatorWidth = checkable ?
                    pixelMetric(PM_ListViewIconSize, opt, widget) :
                    pixelMetric(PM_SmallIconSize, opt, widget);
                ret = menuItem->rect;

                if (element == SE_ItemViewItemDecoration) {
                    if (menuItem->direction == Qt::RightToLeft)
                        ret.translate(ret.width()-indicatorWidth, 0);
                    ret.setWidth(indicatorWidth);
                } else {
                    ret = menuItem->rect;
                    if (!menuItem->icon.isNull())
                        if (menuItem->direction == Qt::LeftToRight)
                            ret.adjust(indicatorWidth, 0, 0, 0);
                        else
                            ret.adjust(0, 0, -indicatorWidth, 0);

                    // Make room for submenu indicator
                    if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu){
                        // submenu indicator is very small, so lets halve the rect
                        if (menuItem->direction == Qt::LeftToRight)
                            ret.adjust(0,0,-(indicatorWidth >> 1),0);
                        else
                            ret.adjust((indicatorWidth >> 1),0,0,0);
                    }
                }
            }
            break;
        case SE_ItemViewItemCheckIndicator:
            if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(opt)) {
                const QListWidget *listItem = qobject_cast<const QListWidget *>(widget);

                const bool singleSelection = listItem &&
                    (listItem->selectionMode() == QAbstractItemView::SingleSelection ||
                     listItem->selectionMode() == QAbstractItemView::NoSelection);
                const bool checkBoxOnly = (vopt->features & QStyleOptionViewItemV2::HasCheckIndicator) &&
                    listItem &&
                    singleSelection;

                // Selection check mark rect.
                const int indicatorWidth = QS60StylePrivate::pixelMetric(QStyle::PM_IndicatorWidth);
                const int indicatorHeight = QS60StylePrivate::pixelMetric(QStyle::PM_IndicatorHeight);
                const int spacing = QS60StylePrivate::pixelMetric(QStyle::PM_CheckBoxLabelSpacing);

                const int itemHeight = opt->rect.height();
                int heightOffset = 0;
                if (indicatorHeight < itemHeight)
                    heightOffset = ((itemHeight - indicatorHeight)>>1);
                if (checkBoxOnly) {
                    // Move rect and make it slightly smaller, so that
                    // a) highlight border does not cross the rect
                    // b) in s60 list checkbox is smaller than normal checkbox
                    //todo; magic three
                    ret.setRect(opt->rect.left()+3, opt->rect.top() + heightOffset,
                        indicatorWidth-3, indicatorHeight-3);
                } else {
                    ret.setRect(opt->rect.right() - indicatorWidth - spacing, opt->rect.top() + heightOffset,
                        indicatorWidth, indicatorHeight);
                }
            } else  {
                ret = QCommonStyle::subElementRect(element, opt, widget);
            }
            break;
        case SE_HeaderLabel:
            ret = QCommonStyle::subElementRect(element, opt, widget);
            if (qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
                // Subtract area needed for line
                if (opt->state & State_Horizontal)
                    ret.setHeight(ret.height() - QS60StylePrivate::pixelMetric(PM_Custom_BoldLineWidth));
                else
                    ret.setWidth(ret.width() - QS60StylePrivate::pixelMetric(PM_Custom_ThinLineWidth));
                }
            ret = visualRect(opt->direction, opt->rect, ret);
            break;
        default:
            ret = QCommonStyle::subElementRect(element, opt, widget);
    }
    return ret;
}

/*!
  \reimp
 */
void QS60Style::polish(QWidget *widget)
{
    Q_D(const QS60Style);
    QCommonStyle::polish(widget);

    if (!widget)
        return;

    if (false
#ifndef QT_NO_SCROLLBAR
        || qobject_cast<QScrollBar *>(widget)
#endif
        ) {
        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    if (QS60StylePrivate::drawsOwnThemeBackground(widget)) {
        widget->setAttribute(Qt::WA_StyledBackground);
    } else if (false
#ifndef QT_NO_MENU
        || qobject_cast<const QMenu *> (widget)
#endif // QT_NO_MENU
    ) {
        widget->setAttribute(Qt::WA_StyledBackground);
    } else if (false
#ifndef QT_NO_COMBOBOX
        || qobject_cast<const QComboBoxListView *>(widget)
#endif //QT_NO_COMBOBOX
        ) {
        widget->setAttribute(Qt::WA_StyledBackground);
    }
    d->setThemePalette(widget);
    d->setFont(widget);
}

/*!
  \reimp
 */
void QS60Style::unpolish(QWidget *widget)
{
    if (false
    #ifndef QT_NO_SCROLLBAR
        || qobject_cast<QScrollBar *>(widget)
    #endif
        )
        widget->setAttribute(Qt::WA_OpaquePaintEvent);

    if (QS60StylePrivate::drawsOwnThemeBackground(widget)) {
        widget->setAttribute(Qt::WA_StyledBackground, false);
    } else if (false
#ifndef QT_NO_MENU
        || qobject_cast<const QMenu *> (widget)
#endif // QT_NO_MENU
        ) {
        widget->setAttribute(Qt::WA_StyledBackground, false);
    } else if (false
#ifndef QT_NO_COMBOBOX
        || qobject_cast<const QComboBoxListView *>(widget)
#endif //QT_NO_COMBOBOX
        ) {
        widget->setAttribute(Qt::WA_StyledBackground, false);
    }

    if (widget)
        widget->setPalette(QPalette());

    QCommonStyle::unpolish(widget);
}

/*!
  \reimp
 */
void QS60Style::polish(QApplication *application)
{
    Q_D(QS60Style);
    d->m_originalPalette = application->palette();
    d->setThemePalette(application);
}

/*!
  \reimp
 */
void QS60Style::unpolish(QApplication *application)
{
    Q_UNUSED(application)
    Q_D(QS60Style);
    const QPalette newPalette = QApplication::style()->standardPalette();
    QApplication::setPalette(newPalette);
    QApplicationPrivate::setSystemPalette(d->m_originalPalette);
}

/*!
  \reimp
 */
bool QS60Style::event(QEvent *e)
{
#ifdef QT_KEYPAD_NAVIGATION
    if (QS60StylePrivate::isTouchSupported())
        return false;
    Q_D(QS60Style);
    switch (e->type()) {
    case QEvent::FocusIn:
        if (QWidget *focusWidget = QApplication::focusWidget()) {
            if (!d->m_focusFrame)
                d->m_focusFrame = new QFocusFrame(focusWidget);
            d->m_focusFrame->setWidget(focusWidget);
        } else if (d->m_focusFrame) {
            d->m_focusFrame->setWidget(0);
        }
        break;
    case QEvent::FocusOut:
        if (d->m_focusFrame)
            d->m_focusFrame->setWidget(0);
        break;
    case QEvent::EnterEditFocus:
    case QEvent::LeaveEditFocus:
        if (d->m_focusFrame)
            d->m_focusFrame->update();
        break;
    default:
        break;
    }
#else
    Q_UNUSED(e)
#endif
    return false;
}

/*!
    \internal
 */
QIcon QS60Style::standardIconImplementation(StandardPixmap standardIcon,
    const QStyleOption *option, const QWidget *widget) const
{
    const int iconDimension = QS60StylePrivate::pixelMetric(QStyle::PM_ToolBarIconSize);
    const QRect iconSize = (!option) ? QRect(0,0,iconDimension,iconDimension) : option->rect;
    QS60StyleEnums::SkinParts part;
    QS60StylePrivate::SkinElementFlags adjustedFlags;
    if (option)
        adjustedFlags = (option->state & State_Enabled || option->state == 0) ?
            QS60StylePrivate::SF_StateEnabled :
            QS60StylePrivate::SF_StateDisabled;

    switch(standardIcon) {
        case QStyle::SP_MessageBoxWarning:
            part = QS60StyleEnums::SP_QgnNoteWarning;
            break;
        case QStyle::SP_MessageBoxInformation:
            part = QS60StyleEnums::SP_QgnNoteInfo;
            break;
        case QStyle::SP_MessageBoxCritical:
            part = QS60StyleEnums::SP_QgnNoteError;
            break;
        case QStyle::SP_MessageBoxQuestion:
            part = QS60StyleEnums::SP_QgnNoteQuery;
            break;
        case QStyle::SP_ArrowRight:
            part = QS60StyleEnums::SP_QgnIndiNaviArrowRight;
            break;
        case QStyle::SP_ArrowLeft:
            part = QS60StyleEnums::SP_QgnIndiNaviArrowLeft;
            break;
        case QStyle::SP_ArrowUp:
            part = QS60StyleEnums::SP_QgnIndiNaviArrowLeft;
            adjustedFlags |= QS60StylePrivate::SF_PointEast;
            break;
        case QStyle::SP_ArrowDown:
            part = QS60StyleEnums::SP_QgnIndiNaviArrowLeft;
            adjustedFlags |= QS60StylePrivate::SF_PointWest;
            break;
        case QStyle::SP_ArrowBack:
            if (QApplication::layoutDirection() == Qt::RightToLeft)
                return QS60Style::standardIcon(SP_ArrowRight, option, widget);
            return QS60Style::standardIcon(SP_ArrowLeft, option, widget);
        case QStyle::SP_ArrowForward:
            if (QApplication::layoutDirection() == Qt::RightToLeft)
                return QS60Style::standardIcon(SP_ArrowLeft, option, widget);
            return QS60Style::standardIcon(SP_ArrowRight, option, widget);
        case QStyle::SP_ComputerIcon:
            part = QS60StyleEnums::SP_QgnPropPhoneMemcLarge;
            break;
        case QStyle::SP_DirClosedIcon:
            part = QS60StyleEnums::SP_QgnPropFolderSmall;
            break;
        case QStyle::SP_DirOpenIcon:
            part = QS60StyleEnums::SP_QgnPropFolderCurrent;
            break;
        case QStyle::SP_DirIcon:
            part = QS60StyleEnums::SP_QgnPropFolderSmall;
            break;
        case QStyle::SP_FileDialogNewFolder:
            part = QS60StyleEnums::SP_QgnPropFolderSmallNew;
            break;
        case QStyle::SP_FileIcon:
            part = QS60StyleEnums::SP_QgnPropFileSmall;
            break;
        case QStyle::SP_TrashIcon:
            part = QS60StyleEnums::SP_QgnNoteErased;
            break;
        case QStyle::SP_ToolBarHorizontalExtensionButton:
            part = QS60StyleEnums::SP_QgnIndiSubMenu;
            if (QApplication::layoutDirection() == Qt::RightToLeft)
                adjustedFlags |= QS60StylePrivate::SF_PointSouth;
            break;
        case QStyle::SP_ToolBarVerticalExtensionButton:
            adjustedFlags |= QS60StylePrivate::SF_PointEast;
            part = QS60StyleEnums::SP_QgnIndiSubMenu;
            break;

        default:
            return QCommonStyle::standardIconImplementation(standardIcon, option, widget);
    }
    const QS60StylePrivate::SkinElementFlags flags = adjustedFlags;
    const QPixmap cachedPixMap(QS60StylePrivate::cachedPart(part, iconSize.size(), 0, flags));
    return cachedPixMap.isNull() ?
        QCommonStyle::standardIconImplementation(standardIcon, option, widget) : QIcon(cachedPixMap);
}

extern QPoint qt_s60_fill_background_offset(const QWidget *targetWidget);

bool qt_s60_fill_background(QPainter *painter, const QRegion &rgn, const QBrush &brush)
{
    const QPixmap backgroundTexture(QS60StylePrivate::backgroundTexture());
    if (backgroundTexture.cacheKey() != brush.texture().cacheKey())
        return false;

    const QPaintDevice *target = painter->device();
    if (target->devType() == QInternal::Widget) {
        const QWidget *widget = static_cast<const QWidget *>(target);
        const QVector<QRect> &rects = rgn.rects();
        for (int i = 0; i < rects.size(); ++i) {
            const QRect rect(rects.at(i));
            painter->drawPixmap(rect.topLeft(), backgroundTexture,
                                rect.translated(qt_s60_fill_background_offset(widget)));
        }
    }
    return true;
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_S60 || QT_PLUGIN
