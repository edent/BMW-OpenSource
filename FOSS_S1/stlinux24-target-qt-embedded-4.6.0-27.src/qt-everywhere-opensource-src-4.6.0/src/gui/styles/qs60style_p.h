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

#ifndef QS60STYLE_P_H
#define QS60STYLE_P_H

#include "qs60style.h"
#include "qcommonstyle_p.h"
#include <QtCore/qhash.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

const int MAX_NON_CUSTOM_PIXELMETRICS = 92;
const int CUSTOMVALUESCOUNT = 4;
enum {
    PM_Custom_FrameCornerWidth = MAX_NON_CUSTOM_PIXELMETRICS,
    PM_Custom_FrameCornerHeight,
    PM_Custom_BoldLineWidth,
    PM_Custom_ThinLineWidth
    };
const int MAX_PIXELMETRICS = MAX_NON_CUSTOM_PIXELMETRICS + CUSTOMVALUESCOUNT;

typedef struct {
    unsigned short height;
    unsigned short width;
    int major_version;
    int minor_version;
    bool mirroring; // TODO: (nice to have) Use Qt::LayoutDirection
    const char* layoutName;
} layoutHeader;

#ifdef Q_OS_SYMBIAN
NONSHARABLE_CLASS (QS60StyleEnums)
#else
class QS60StyleEnums
#endif
: public QObject
{
#ifndef Q_WS_S60
    Q_OBJECT
    Q_ENUMS(FontCategories)
    Q_ENUMS(SkinParts)
    Q_ENUMS(ColorLists)
#endif // !Q_WS_S60

public:
    // S60 look-and-feel font categories
    enum FontCategories {
        FC_Undefined,
        FC_Primary,
        FC_Secondary,
        FC_Title,
        FC_PrimarySmall,
        FC_Digital
    };

    enum SkinParts {
        SP_QgnGrafBarWait,
        SP_QgnGrafBarFrameCenter,
        SP_QgnGrafBarFrameSideL,
        SP_QgnGrafBarFrameSideR,
        SP_QgnGrafBarProgress,
        SP_QgnGrafScrollArrowDown,
        SP_QgnGrafScrollArrowLeft,
        SP_QgnGrafScrollArrowRight,
        SP_QgnGrafScrollArrowUp,
        SP_QgnGrafTabActiveL,
        SP_QgnGrafTabActiveM,
        SP_QgnGrafTabActiveR,
        SP_QgnGrafTabPassiveL,
        SP_QgnGrafTabPassiveM,
        SP_QgnGrafTabPassiveR,
        SP_QgnIndiCheckboxOff,
        SP_QgnIndiCheckboxOn,
        SP_QgnIndiHlColSuper,     // Available in S60 release 3.2 and later.
        SP_QgnIndiHlExpSuper,     // Available in S60 release 3.2 and later.
        SP_QgnIndiHlLineBranch,   // Available in S60 release 3.2 and later.
        SP_QgnIndiHlLineEnd,      // Available in S60 release 3.2 and later.
        SP_QgnIndiHlLineStraight, // Available in S60 release 3.2 and later.
        SP_QgnIndiMarkedAdd,
        SP_QgnIndiNaviArrowLeft,
        SP_QgnIndiNaviArrowRight,
        SP_QgnIndiRadiobuttOff,
        SP_QgnIndiRadiobuttOn,
        SP_QgnIndiSliderEdit,
        SP_QgnIndiSubMenu,
        SP_QgnNoteErased,
        SP_QgnNoteError,
        SP_QgnNoteInfo,
        SP_QgnNoteOk,
        SP_QgnNoteQuery,
        SP_QgnNoteWarning,
        SP_QgnPropFileSmall,
        SP_QgnPropFolderCurrent,
        SP_QgnPropFolderSmall,
        SP_QgnPropFolderSmallNew,
        SP_QgnPropPhoneMemcLarge,
        SP_QsnCpScrollHandleBottomPressed, //ScrollBar handle, pressed state
        SP_QsnCpScrollHandleMiddlePressed,
        SP_QsnCpScrollHandleTopPressed,
        SP_QsnBgScreen,
        SP_QsnCpScrollBgBottom,
        SP_QsnCpScrollBgMiddle,
        SP_QsnCpScrollBgTop,
        SP_QsnCpScrollHandleBottom,
        SP_QsnCpScrollHandleMiddle,
        SP_QsnCpScrollHandleTop,
        SP_QsnFrButtonTbCornerTl,           // Button, normal state
        SP_QsnFrButtonTbCornerTr,
        SP_QsnFrButtonTbCornerBl,
        SP_QsnFrButtonTbCornerBr,
        SP_QsnFrButtonTbSideT,
        SP_QsnFrButtonTbSideB,
        SP_QsnFrButtonTbSideL,
        SP_QsnFrButtonTbSideR,
        SP_QsnFrButtonTbCenter,
        SP_QsnFrButtonTbCornerTlPressed,    // Button, pressed state
        SP_QsnFrButtonTbCornerTrPressed,
        SP_QsnFrButtonTbCornerBlPressed,
        SP_QsnFrButtonTbCornerBrPressed,
        SP_QsnFrButtonTbSideTPressed,
        SP_QsnFrButtonTbSideBPressed,
        SP_QsnFrButtonTbSideLPressed,
        SP_QsnFrButtonTbSideRPressed,
        SP_QsnFrButtonTbCenterPressed,
        SP_QsnFrCaleCornerTl,               // calendar grid item
        SP_QsnFrCaleCornerTr,
        SP_QsnFrCaleCornerBl,
        SP_QsnFrCaleCornerBr,
        SP_QsnFrCaleGSideT,
        SP_QsnFrCaleGSideB,
        SP_QsnFrCaleGSideL,
        SP_QsnFrCaleGSideR,
        SP_QsnFrCaleCenter,
        SP_QsnFrCaleHeadingCornerTl,        // calendar grid header
        SP_QsnFrCaleHeadingCornerTr,
        SP_QsnFrCaleHeadingCornerBl,
        SP_QsnFrCaleHeadingCornerBr,
        SP_QsnFrCaleHeadingSideT,
        SP_QsnFrCaleHeadingSideB,
        SP_QsnFrCaleHeadingSideL,
        SP_QsnFrCaleHeadingSideR,
        SP_QsnFrCaleHeadingCenter,
        SP_QsnFrInputCornerTl,              // Text input field
        SP_QsnFrInputCornerTr,
        SP_QsnFrInputCornerBl,
        SP_QsnFrInputCornerBr,
        SP_QsnFrInputSideT,
        SP_QsnFrInputSideB,
        SP_QsnFrInputSideL,
        SP_QsnFrInputSideR,
        SP_QsnFrInputCenter,
        SP_QsnFrListCornerTl,               // List background
        SP_QsnFrListCornerTr,
        SP_QsnFrListCornerBl,
        SP_QsnFrListCornerBr,
        SP_QsnFrListSideT,
        SP_QsnFrListSideB,
        SP_QsnFrListSideL,
        SP_QsnFrListSideR,
        SP_QsnFrListCenter,
        SP_QsnFrPopupCornerTl,              // Option menu background
        SP_QsnFrPopupCornerTr,
        SP_QsnFrPopupCornerBl,
        SP_QsnFrPopupCornerBr,
        SP_QsnFrPopupSideT,
        SP_QsnFrPopupSideB,
        SP_QsnFrPopupSideL,
        SP_QsnFrPopupSideR,
        SP_QsnFrPopupCenter,
        SP_QsnFrPopupPreviewCornerTl,       // tool tip background
        SP_QsnFrPopupPreviewCornerTr,
        SP_QsnFrPopupPreviewCornerBl,
        SP_QsnFrPopupPreviewCornerBr,
        SP_QsnFrPopupPreviewSideT,
        SP_QsnFrPopupPreviewSideB,
        SP_QsnFrPopupPreviewSideL,
        SP_QsnFrPopupPreviewSideR,
        SP_QsnFrPopupPreviewCenter,
        SP_QsnFrSetOptCornerTl,             // Settings list
        SP_QsnFrSetOptCornerTr,
        SP_QsnFrSetOptCornerBl,
        SP_QsnFrSetOptCornerBr,
        SP_QsnFrSetOptSideT,
        SP_QsnFrSetOptSideB,
        SP_QsnFrSetOptSideL,
        SP_QsnFrSetOptSideR,
        SP_QsnFrSetOptCenter,
        SP_QsnFrPopupSubCornerTl,           // Toolbar background
        SP_QsnFrPopupSubCornerTr,
        SP_QsnFrPopupSubCornerBl,
        SP_QsnFrPopupSubCornerBr,
        SP_QsnFrPopupSubSideT,
        SP_QsnFrPopupSubSideB,
        SP_QsnFrPopupSubSideL,
        SP_QsnFrPopupSubSideR,
        SP_QsnFrPopupSubCenter,
        SP_QsnFrSctrlButtonCornerTl,        // Toolbar button
        SP_QsnFrSctrlButtonCornerTr,
        SP_QsnFrSctrlButtonCornerBl,
        SP_QsnFrSctrlButtonCornerBr,
        SP_QsnFrSctrlButtonSideT,
        SP_QsnFrSctrlButtonSideB,
        SP_QsnFrSctrlButtonSideL,
        SP_QsnFrSctrlButtonSideR,
        SP_QsnFrSctrlButtonCenter,
        SP_QsnFrSctrlButtonCornerTlPressed,    // Toolbar button, pressed
        SP_QsnFrSctrlButtonCornerTrPressed,
        SP_QsnFrSctrlButtonCornerBlPressed,
        SP_QsnFrSctrlButtonCornerBrPressed,
        SP_QsnFrSctrlButtonSideTPressed,
        SP_QsnFrSctrlButtonSideBPressed,
        SP_QsnFrSctrlButtonSideLPressed,
        SP_QsnFrSctrlButtonSideRPressed,
        SP_QsnFrSctrlButtonCenterPressed,
        SP_QsnFrButtonCornerTlInactive,     // Inactive button
        SP_QsnFrButtonCornerTrInactive,
        SP_QsnFrButtonCornerBlInactive,
        SP_QsnFrButtonCornerBrInactive,
        SP_QsnFrButtonSideTInactive,
        SP_QsnFrButtonSideBInactive,
        SP_QsnFrButtonSideLInactive,
        SP_QsnFrButtonSideRInactive,
        SP_QsnFrButtonCenterInactive,
        SP_QsnFrNotepadCornerTl,
        SP_QsnFrNotepadCornerTr,
        SP_QsnFrNotepadCornerBl,
        SP_QsnFrNotepadCornerBr,
        SP_QsnFrNotepadSideT,
        SP_QsnFrNotepadSideB,
        SP_QsnFrNotepadSideL,
        SP_QsnFrNotepadSideR,
        SP_QsnFrNotepadCenter
    };

    enum ColorLists {
        CL_QsnHighlightColors,
        CL_QsnIconColors,
        CL_QsnLineColors,
        CL_QsnOtherColors,
        CL_QsnParentColors,
        CL_QsnTextColors
    };
};

class QFocusFrame;

// Private class
#ifdef Q_OS_SYMBIAN
NONSHARABLE_CLASS (QS60StylePrivate)
#else
class QS60StylePrivate
#endif
: public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(QS60Style)

public:
    QS60StylePrivate();
    ~QS60StylePrivate();

    enum SkinElements {
        SE_ButtonNormal,
        SE_ButtonPressed,
        SE_FrameLineEdit,
        SE_ProgressBarGrooveHorizontal,
        SE_ProgressBarIndicatorHorizontal,
        SE_ProgressBarGrooveVertical,
        SE_ProgressBarIndicatorVertical,
        SE_ScrollBarGrooveHorizontal,
        SE_ScrollBarGrooveVertical,
        SE_ScrollBarHandleHorizontal,
        SE_ScrollBarHandleVertical,
        SE_SliderHandleHorizontal,
        SE_SliderHandleVertical,
        SE_TabBarTabEastActive,
        SE_TabBarTabEastInactive,
        SE_TabBarTabNorthActive,
        SE_TabBarTabNorthInactive,
        SE_TabBarTabSouthActive,
        SE_TabBarTabSouthInactive,
        SE_TabBarTabWestActive,
        SE_TabBarTabWestInactive,
        SE_ListHighlight,
        SE_OptionsMenu,
        SE_SettingsList,
        SE_TableItem,
        SE_TableHeaderItem,
        SE_ToolTip, //own graphic available on 3.2+ releases,
        SE_ToolBar,
        SE_ToolBarButton,
        SE_ToolBarButtonPressed,
        SE_PanelBackground,
        SE_ScrollBarHandlePressedHorizontal, //only for 5.0+
        SE_ScrollBarHandlePressedVertical,
        SE_ButtonInactive,
        SE_Editor,
    };

    enum SkinFrameElements {
        SF_ButtonNormal,
        SF_ButtonPressed,
        SF_FrameLineEdit,
        SF_ListHighlight,
        SF_OptionsMenu,
        SF_SettingsList,
        SF_TableItem,
        SF_TableHeaderItem,
        SF_ToolTip,
        SF_ToolBar,
        SF_ToolBarButton,
        SF_ToolBarButtonPressed,
        SF_PanelBackground,
        SF_ButtonInactive,
    };

    enum SkinElementFlag {
        SF_PointNorth =       0x0001, // North = the default
        SF_PointEast =        0x0002,
        SF_PointSouth =       0x0004,
        SF_PointWest =        0x0008,

        SF_StateEnabled =     0x0010, // Enabled = the default
        SF_StateDisabled =    0x0020,
        SF_ColorSkinned =     0x0040, // pixmap is colored with foreground pen color
    };

    enum CacheClearReason {
        CC_UndefinedChange = 0,
        CC_LayoutChange,
        CC_ThemeChange
    };

    Q_DECLARE_FLAGS(SkinElementFlags, SkinElementFlag)

    // draws skin element
    static void drawSkinElement(SkinElements element, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    // draws a specific skin part
    static void drawSkinPart(QS60StyleEnums::SkinParts part, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    // gets pixel metrics value
    static short pixelMetric(int metric);
    // gets color. 'index' is NOT 0-based.
    // It corresponds to the enum key 1-based numbers of TAknsQsnXYZColorsIndex, not the values.
    static QColor s60Color(QS60StyleEnums::ColorLists list,
        int index, const QStyleOption *option);
    // gets state specific color
    static QColor stateColor(const QColor &color, const QStyleOption *option);
    // gets lighter color than base color
    static QColor lighterColor(const QColor &baseColor);
    //deduces if the given widget should have separately themeable background
    static bool drawsOwnThemeBackground(const QWidget *widget);

    QFont s60Font(QS60StyleEnums::FontCategories fontCategory,
        int pointSize = -1) const;
    // clears all style caches (fonts, colors, pixmaps)
    void clearCaches(CacheClearReason reason = CC_UndefinedChange);

    // themed main background oprations
    void setBackgroundTexture(QApplication *application) const;
    static void deleteBackground();

    static bool isTouchSupported();
    static bool isToolBarBackground();

    // calculates average color based on button skin graphics (minus borders).
    QColor colorFromFrameGraphics(SkinFrameElements frame) const;

    //set theme palette for application
    void setThemePalette(QApplication *application) const;
    //set theme palette for style option
    void setThemePalette(QStyleOption *option) const;
    //access to theme palette
    static QPalette* themePalette();

    static const layoutHeader m_layoutHeaders[];
    static const short data[][MAX_PIXELMETRICS];

    void setCurrentLayout(int layoutIndex);
    void setActiveLayout();
    // Pointer
    static short const *m_pmPointer;
    // number of layouts supported by the style
    static const int m_numberOfLayouts;

    mutable QHash<QPair<QS60StyleEnums::FontCategories , int>, QFont> m_mappedFontsCache;
    mutable QHash<SkinFrameElements, QColor> m_colorCache;

    // Has one entry per SkinFrameElements
    static const struct frameElementCenter {
        SkinElements element;
        QS60StyleEnums::SkinParts center;
    } m_frameElementsData[];

    static QPixmap frame(SkinFrameElements frame, const QSize &size,
        SkinElementFlags flags = KDefaultSkinElementFlags);
    static QPixmap backgroundTexture();

#ifdef Q_WS_S60
    void handleDynamicLayoutVariantSwitch();
    void handleSkinChange();
#endif // Q_WS_S60

    static QSize naviPaneSize();

    //Checks that the current brush is transparent or has BrushStyle NoBrush,
    //so that theme graphic background can be drawn. 
    static bool canDrawThemeBackground(const QBrush &backgroundBrush);

private:
    static void drawPart(QS60StyleEnums::SkinParts part, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    static void drawRow(QS60StyleEnums::SkinParts start, QS60StyleEnums::SkinParts middle,
        QS60StyleEnums::SkinParts end, Qt::Orientation orientation, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    static void drawFrame(SkinFrameElements frame, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);

    static QPixmap cachedPart(QS60StyleEnums::SkinParts part, const QSize &size,
        QPainter *painter, SkinElementFlags flags = KDefaultSkinElementFlags);
    static QPixmap cachedFrame(SkinFrameElements frame, const QSize &size,
        SkinElementFlags flags = KDefaultSkinElementFlags);

    static void refreshUI();

    // set S60 font for widget
    void setFont(QWidget *widget) const;
    void setThemePalette(QWidget *widget) const;
    void setThemePalette(QPalette *palette) const;
    void setThemePaletteHash(QPalette *palette) const;
    static void storeThemePalette(QPalette *palette);
    static void deleteThemePalette();

    static QSize partSize(QS60StyleEnums::SkinParts part,
        SkinElementFlags flags = KDefaultSkinElementFlags);
    static QPixmap part(QS60StyleEnums::SkinParts part, const QSize &size,
        QPainter *painter, SkinElementFlags flags = KDefaultSkinElementFlags);

    static QFont s60Font_specific(QS60StyleEnums::FontCategories fontCategory, int pointSize);

    static QSize screenSize();

    // Contains background texture.
    static QPixmap *m_background;
    const static SkinElementFlags KDefaultSkinElementFlags;
    // defined theme palette
    static QPalette *m_themePalette;
    QPalette m_originalPalette;

    QPointer<QFocusFrame> m_focusFrame;
};

QT_END_NAMESPACE

#endif // QS60STYLE_P_H
