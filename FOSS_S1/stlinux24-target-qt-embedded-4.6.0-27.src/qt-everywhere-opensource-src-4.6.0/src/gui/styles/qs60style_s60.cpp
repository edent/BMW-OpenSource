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

#include "qs60style.h"
#include "qs60style_p.h"
#include "qpainter.h"
#include "qstyleoption.h"
#include "qstyle.h"
#include "private/qt_s60_p.h"
#include "private/qpixmap_s60_p.h"
#include "private/qcore_symbian_p.h"
#include "qapplication.h"

#include <w32std.h>
#include <aknsconstants.h>
#include <aknconsts.h>
#include <aknsitemid.h>
#include <aknsutils.h>
#include <aknsdrawutils.h>
#include <aknsskininstance.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <avkon.mbg>
#include <AknFontAccess.h>
#include <AknLayoutFont.h>
#include <aknutils.h>
#include <aknnavi.h>

#if !defined(QT_NO_STYLE_S60) || defined(QT_PLUGIN)

QT_BEGIN_NAMESPACE

enum TDrawType {
    EDrawIcon,
    EDrawBackground,
    ENoDraw
};

enum TSupportRelease {
    ES60_None     = 0x0000, //indicates that the commonstyle should draw the graphics
    ES60_3_1      = 0x0001,
    ES60_3_2      = 0x0002,
    ES60_5_0      = 0x0004,
    ES60_5_1      = 0x0008,
    ES60_5_2      = 0x0010,
    // Add all new releases here
    ES60_AllReleases = ES60_3_1 | ES60_3_2 | ES60_5_0 | ES60_5_1 | ES60_5_2
};

typedef struct {
    const TAknsItemID &skinID;
    TDrawType drawType;
    int supportInfo;
    int newMajorSkinId;
    int newMinorSkinId;
} partMapEntry;

class QS60StyleModeSpecifics
{
public:
    static QPixmap skinnedGraphics(QS60StyleEnums::SkinParts stylepart,
        const QSize &size, QS60StylePrivate::SkinElementFlags flags);
    static QPixmap skinnedGraphics(QS60StylePrivate::SkinFrameElements frameElement, const QSize &size, QS60StylePrivate::SkinElementFlags flags);
    static QPixmap colorSkinnedGraphics(const QS60StyleEnums::SkinParts &stylepart,
        const QSize &size, QPainter *painter, QS60StylePrivate::SkinElementFlags flags);
    static QColor colorValue(const TAknsItemID &colorGroup, int colorIndex);
    static QPixmap fromFbsBitmap(CFbsBitmap *icon, CFbsBitmap *mask, QS60StylePrivate::SkinElementFlags flags, QImage::Format format);
    static bool disabledPartGraphic(QS60StyleEnums::SkinParts &part);
    static bool disabledFrameGraphic(QS60StylePrivate::SkinFrameElements &frame);
    static QPixmap generateMissingThemeGraphic(QS60StyleEnums::SkinParts &part, const QSize &size, QS60StylePrivate::SkinElementFlags flags);
    static QSize naviPaneSize();

private:
    static QPixmap createSkinnedGraphicsLX(QS60StyleEnums::SkinParts part,
        const QSize &size, QS60StylePrivate::SkinElementFlags flags);
    static QPixmap createSkinnedGraphicsLX(QS60StylePrivate::SkinFrameElements frameElement, const QSize &size, QS60StylePrivate::SkinElementFlags flags);
    static QPixmap colorSkinnedGraphicsLX(const QS60StyleEnums::SkinParts &stylepart,
        const QSize &size, QPainter *painter, QS60StylePrivate::SkinElementFlags flags);
    static void frameIdAndCenterId(QS60StylePrivate::SkinFrameElements frameElement, TAknsItemID &frameId, TAknsItemID &centerId);
    static TRect innerRectFromElement(QS60StylePrivate::SkinFrameElements frameElement, const TRect &outerRect);
    static void checkAndUnCompressBitmapL(CFbsBitmap*& aOriginalBitmap);
    static void checkAndUnCompressBitmap(CFbsBitmap*& aOriginalBitmap);
    static void unCompressBitmapL(const TRect& aTrgRect, CFbsBitmap* aTrgBitmap, CFbsBitmap* aSrcBitmap);
    static void fallbackInfo(const QS60StyleEnums::SkinParts &stylepart, TDes& fallbackFileName, TInt& fallbackIndex);
    static bool checkSupport(const int supportedRelease);
    static TAknsItemID checkAndUpdateReleaseSpecificGraphics(int part);
    // Array to match the skin ID, fallback graphics and Qt widget graphics.
    static const partMapEntry m_partMap[];
};

const partMapEntry QS60StyleModeSpecifics::m_partMap[] = {
    /* SP_QgnGrafBarWait */             {KAknsIIDQgnGrafBarWaitAnim,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafBarFrameCenter */      {KAknsIIDQgnGrafBarFrameCenter,         EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafBarFrameSideL */       {KAknsIIDQgnGrafBarFrameSideL,          EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafBarFrameSideR */       {KAknsIIDQgnGrafBarFrameSideR,          EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafBarProgress */         {KAknsIIDQgnGrafBarProgress,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafScrollArrowDown */     {KAknsIIDQgnGrafScrollArrowDown,        EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafScrollArrowLeft */     {KAknsIIDQgnGrafScrollArrowLeft,        EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafScrollArrowRight */    {KAknsIIDQgnGrafScrollArrowRight,       EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafScrollArrowUp */       {KAknsIIDQgnGrafScrollArrowUp,          EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafTabActiveL */          {KAknsIIDQgnGrafTabActiveL,             EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafTabActiveM */          {KAknsIIDQgnGrafTabActiveM,             EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafTabActiveR */          {KAknsIIDQgnGrafTabActiveR,             EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafTabPassiveL */         {KAknsIIDQgnGrafTabPassiveL,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafTabPassiveM */         {KAknsIIDQgnGrafTabPassiveM,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnGrafTabPassiveR */         {KAknsIIDQgnGrafTabPassiveR,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiCheckboxOff */         {KAknsIIDQgnIndiCheckboxOff,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiCheckboxOn */          {KAknsIIDQgnIndiCheckboxOn,             EDrawIcon,   ES60_AllReleases,    -1,-1},
    // Following 5 items (SP_QgnIndiHlColSuper - SP_QgnIndiHlLineStraight) are available starting from S60 release 3.2.
    // In 3.1 CommonStyle drawing is used for these QTreeView elements, since no similar icons in AVKON UI.
    /* SP_QgnIndiHlColSuper */          {KAknsIIDNone,                          EDrawIcon,   ES60_3_1,            EAknsMajorGeneric, 0x17d5 /* KAknsIIDQgnIndiHlColSuper */},
    /* SP_QgnIndiHlExpSuper */          {KAknsIIDNone,                          EDrawIcon,   ES60_3_1,            EAknsMajorGeneric, 0x17d6 /* KAknsIIDQgnIndiHlExpSuper */},
    /* SP_QgnIndiHlLineBranch */        {KAknsIIDNone,                          EDrawIcon,   ES60_3_1,            EAknsMajorGeneric, 0x17d7 /* KAknsIIDQgnIndiHlLineBranch */},
    /* SP_QgnIndiHlLineEnd */           {KAknsIIDNone,                          EDrawIcon,   ES60_3_1,            EAknsMajorGeneric, 0x17d8 /* KAknsIIDQgnIndiHlLineEnd */},
    /* SP_QgnIndiHlLineStraight */      {KAknsIIDNone,                          EDrawIcon,   ES60_3_1,            EAknsMajorGeneric, 0x17d9 /* KAknsIIDQgnIndiHlLineStraight */},
    /* SP_QgnIndiMarkedAdd */           {KAknsIIDQgnIndiMarkedAdd,              EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiNaviArrowLeft */       {KAknsIIDQgnGrafScrollArrowLeft,        EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiNaviArrowRight */      {KAknsIIDQgnGrafScrollArrowRight,       EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiRadiobuttOff */        {KAknsIIDQgnIndiRadiobuttOff,           EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiRadiobuttOn */         {KAknsIIDQgnIndiRadiobuttOn,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiSliderEdit */          {KAknsIIDQgnIndiSliderEdit,             EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnIndiSubMenu */             {KAknsIIDQgnIndiSubmenu,                EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnNoteErased */              {KAknsIIDQgnNoteErased,                 EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnNoteError */               {KAknsIIDQgnNoteError,                  EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnNoteInfo */                {KAknsIIDQgnNoteInfo,                   EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnNoteOk */                  {KAknsIIDQgnNoteOk,                     EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnNoteQuery */               {KAknsIIDQgnNoteQuery,                  EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnNoteWarning */             {KAknsIIDQgnNoteWarning,                EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnPropFileSmall */           {KAknsIIDQgnPropFileSmall,              EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnPropFolderCurrent */       {KAknsIIDQgnPropFolderCurrent,          EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnPropFolderSmall */         {KAknsIIDQgnPropFolderSmall,            EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnPropFolderSmallNew */      {KAknsIIDQgnPropFolderSmallNew,         EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QgnPropPhoneMemcLarge */      {KAknsIIDQgnPropPhoneMemcLarge,         EDrawIcon,   ES60_AllReleases,    -1,-1},

    // 3.1 & 3.2 do not have pressed state for scrollbar, so use normal scrollbar graphics instead.
    /* SP_QsnCpScrollHandleBottomPressed*/ {KAknsIIDQsnCpScrollHandleBottom,    EDrawIcon,   ES60_3_1 | ES60_3_2, EAknsMajorGeneric, 0x20f8}, /*KAknsIIDQsnCpScrollHandleBottomPressed*/
    /* SP_QsnCpScrollHandleMiddlePressed*/ {KAknsIIDQsnCpScrollHandleMiddle,    EDrawIcon,   ES60_3_1 | ES60_3_2, EAknsMajorGeneric, 0x20f9}, /*KAknsIIDQsnCpScrollHandleMiddlePressed*/
    /* SP_QsnCpScrollHandleTopPressed*/    {KAknsIIDQsnCpScrollHandleTop,       EDrawIcon,   ES60_3_1 | ES60_3_2, EAknsMajorGeneric, 0x20fa}, /*KAknsIIDQsnCpScrollHandleTopPressed*/

    /* SP_QsnBgScreen */                {KAknsIIDQsnBgScreen,              EDrawBackground,  ES60_AllReleases,    -1,-1},

    /* SP_QsnCpScrollBgBottom */        {KAknsIIDQsnCpScrollBgBottom,           EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QsnCpScrollBgMiddle */        {KAknsIIDQsnCpScrollBgMiddle,           EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QsnCpScrollBgTop */           {KAknsIIDQsnCpScrollBgTop,              EDrawIcon,   ES60_AllReleases,    -1,-1},

    /* SP_QsnCpScrollHandleBottom */    {KAknsIIDQsnCpScrollHandleBottom,       EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QsnCpScrollHandleMiddle */    {KAknsIIDQsnCpScrollHandleMiddle,       EDrawIcon,   ES60_AllReleases,    -1,-1},
    /* SP_QsnCpScrollHandleTop */       {KAknsIIDQsnCpScrollHandleTop,          EDrawIcon,   ES60_AllReleases,    -1,-1},

    /* SP_QsnFrButtonTbCornerTl */      {KAknsIIDQsnFrButtonTbCornerTl,         ENoDraw,     ES60_AllReleases,    -1,-1}, //todo: use "normal button" from 5.0 onwards
    /* SP_QsnFrButtonTbCornerTr */      {KAknsIIDQsnFrButtonTbCornerTr,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCornerBl */      {KAknsIIDQsnFrButtonTbCornerBl,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCornerBr */      {KAknsIIDQsnFrButtonTbCornerBr,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideT */         {KAknsIIDQsnFrButtonTbSideT,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideB */         {KAknsIIDQsnFrButtonTbSideB,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideL */         {KAknsIIDQsnFrButtonTbSideL,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideR */         {KAknsIIDQsnFrButtonTbSideR,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCenter */        {KAknsIIDQsnFrButtonTbCenter,           EDrawIcon,   ES60_AllReleases,    -1,-1},

    /* SP_QsnFrButtonTbCornerTlPressed */{KAknsIIDQsnFrButtonTbCornerTlPressed, ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCornerTrPressed */{KAknsIIDQsnFrButtonTbCornerTrPressed, ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCornerBlPressed */{KAknsIIDQsnFrButtonTbCornerBlPressed, ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCornerBrPressed */{KAknsIIDQsnFrButtonTbCornerBrPressed, ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideTPressed */   {KAknsIIDQsnFrButtonTbSideTPressed,    ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideBPressed */   {KAknsIIDQsnFrButtonTbSideBPressed,    ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideLPressed */   {KAknsIIDQsnFrButtonTbSideLPressed,    ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbSideRPressed */   {KAknsIIDQsnFrButtonTbSideRPressed,    ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrButtonTbCenterPressed */  {KAknsIIDQsnFrButtonTbCenterPressed,   EDrawIcon,   ES60_AllReleases,    -1,-1},

    /* SP_QsnFrCaleCornerTl */          {KAknsIIDQsnFrCaleCornerTl,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleCornerTr */          {KAknsIIDQsnFrCaleCornerTr,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleCornerBl */          {KAknsIIDQsnFrCaleCornerBl,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleCornerBr */          {KAknsIIDQsnFrCaleCornerBr,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleGSideT */            {KAknsIIDQsnFrCaleSideT,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleGSideB */            {KAknsIIDQsnFrCaleSideB,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleGSideL */            {KAknsIIDQsnFrCaleSideL,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleGSideR */            {KAknsIIDQsnFrCaleSideR,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleCenter */            {KAknsIIDQsnFrCaleCenter,               ENoDraw,     ES60_AllReleases,    -1,-1},

    /* SP_QsnFrCaleHeadingCornerTl */   {KAknsIIDQsnFrCaleHeadingCornerTl,      ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingCornerTr */   {KAknsIIDQsnFrCaleHeadingCornerTr,      ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingCornerBl */   {KAknsIIDQsnFrCaleHeadingCornerBl,      ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingCornerBr */   {KAknsIIDQsnFrCaleHeadingCornerBr,      ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingSideT */      {KAknsIIDQsnFrCaleHeadingSideT,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingSideB */      {KAknsIIDQsnFrCaleHeadingSideB,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingSideL */      {KAknsIIDQsnFrCaleHeadingSideL,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingSideR */      {KAknsIIDQsnFrCaleHeadingSideR,         ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrCaleHeadingCenter */     {KAknsIIDQsnFrCaleHeadingCenter,        ENoDraw,     ES60_AllReleases,    -1,-1},

    /* SP_QsnFrInputCornerTl */         {KAknsIIDQsnFrInputCornerTl,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputCornerTr */         {KAknsIIDQsnFrInputCornerTr,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputCornerBl */         {KAknsIIDQsnFrInputCornerBl,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputCornerBr */         {KAknsIIDQsnFrInputCornerBr,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputSideT */            {KAknsIIDQsnFrInputSideT,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputSideB */            {KAknsIIDQsnFrInputSideB,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputSideL */            {KAknsIIDQsnFrInputSideL,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputSideR */            {KAknsIIDQsnFrInputSideR,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrInputCenter */           {KAknsIIDQsnFrInputCenter,              ENoDraw,     ES60_AllReleases,    -1,-1},

    /* SP_QsnFrListCornerTl */          {KAknsIIDQsnFrListCornerTl,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListCornerTr */          {KAknsIIDQsnFrListCornerTr,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListCornerBl */          {KAknsIIDQsnFrListCornerBl,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListCornerBr */          {KAknsIIDQsnFrListCornerBr,             ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListSideT */             {KAknsIIDQsnFrListSideT,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListSideB */             {KAknsIIDQsnFrListSideB,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListSideL */             {KAknsIIDQsnFrListSideL,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListSideR */             {KAknsIIDQsnFrListSideR,                ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrListCenter */            {KAknsIIDQsnFrListCenter,               ENoDraw,     ES60_AllReleases,    -1,-1},

    /* SP_QsnFrPopupCornerTl */         {KAknsIIDQsnFrPopupCornerTl,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupCornerTr */         {KAknsIIDQsnFrPopupCornerTr,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupCornerBl */         {KAknsIIDQsnFrPopupCornerBl,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupCornerBr */         {KAknsIIDQsnFrPopupCornerBr,            ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupSideT */            {KAknsIIDQsnFrPopupSideT,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupSideB */            {KAknsIIDQsnFrPopupSideB,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupSideL */            {KAknsIIDQsnFrPopupSideL,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupSideR */            {KAknsIIDQsnFrPopupSideR,               ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrPopupCenter */           {KAknsIIDQsnFrPopupCenter,              ENoDraw,     ES60_AllReleases,    -1,-1},

    // ToolTip graphics different in 3.1 vs. 3.2+.
    /* SP_QsnFrPopupPreviewCornerTl */  {KAknsIIDQsnFrPopupCornerTl,            ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c5}, /* KAknsIIDQsnFrPopupPreviewCornerTl */
    /* SP_QsnFrPopupPreviewCornerTr */  {KAknsIIDQsnFrPopupCornerTr,            ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c6},
    /* SP_QsnFrPopupPreviewCornerBl */  {KAknsIIDQsnFrPopupCornerBl,            ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c3},
    /* SP_QsnFrPopupPreviewCornerBr */  {KAknsIIDQsnFrPopupCornerBr,            ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c4},
    /* SP_QsnFrPopupPreviewSideT */     {KAknsIIDQsnFrPopupSideT,               ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19ca},
    /* SP_QsnFrPopupPreviewSideB */     {KAknsIIDQsnFrPopupSideB,               ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c7},
    /* SP_QsnFrPopupPreviewSideL */     {KAknsIIDQsnFrPopupSideL,               ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c8},
    /* SP_QsnFrPopupPreviewSideR */     {KAknsIIDQsnFrPopupSideR,               ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c9},
    /* SP_QsnFrPopupPreviewCenter */    {KAknsIIDQsnFrPopupCenter,              ENoDraw,     ES60_3_1,            EAknsMajorSkin, 0x19c2},

    /* SP_QsnFrSetOptCornerTl */        {KAknsIIDQsnFrSetOptCornerTl,           ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptCornerTr */        {KAknsIIDQsnFrSetOptCornerTr,           ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptCornerBl */        {KAknsIIDQsnFrSetOptCornerBl,           ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptCornerBr */        {KAknsIIDQsnFrSetOptCornerBr,           ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptSideT */           {KAknsIIDQsnFrSetOptSideT,              ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptSideB */           {KAknsIIDQsnFrSetOptSideB,              ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptSideL */           {KAknsIIDQsnFrSetOptSideL,              ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptSideR */           {KAknsIIDQsnFrSetOptSideR,              ENoDraw,     ES60_AllReleases,    -1,-1},
    /* SP_QsnFrSetOptCenter */          {KAknsIIDQsnFrSetOptCenter,             ENoDraw,     ES60_AllReleases,    -1,-1},

    // No toolbar frame for 5.0+ releases.
    /* SP_QsnFrPopupSubCornerTl */      {KAknsIIDQsnFrPopupSubCornerTl,         ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubCornerTr */      {KAknsIIDQsnFrPopupSubCornerTr,         ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubCornerBl */      {KAknsIIDQsnFrPopupSubCornerBl,         ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubCornerBr */      {KAknsIIDQsnFrPopupSubCornerBr,         ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubSideT */         {KAknsIIDQsnFrPopupSubSideT,            ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubSideB */         {KAknsIIDQsnFrPopupSubSideB,            ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubSideL */         {KAknsIIDQsnFrPopupSubSideL,            ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubSideR */         {KAknsIIDQsnFrPopupSubSideR,            ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},
    /* SP_QsnFrPopupSubCenter */        {KAknsIIDQsnFrPopupCenterSubmenu,       ENoDraw,     ES60_3_1 | ES60_3_2, -1,-1},

    // Toolbar graphics is different in 3.1/3.2 vs. 5.0
    /* SP_QsnFrSctrlButtonCornerTl */   {KAknsIIDQsnFrButtonTbCornerTl,         ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2301}, /* KAknsIIDQgnFrSctrlButtonCornerTl*/
    /* SP_QsnFrSctrlButtonCornerTr */   {KAknsIIDQsnFrButtonTbCornerTr,         ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2302},
    /* SP_QsnFrSctrlButtonCornerBl */   {KAknsIIDQsnFrButtonTbCornerBl,         ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2303},
    /* SP_QsnFrSctrlButtonCornerBr */   {KAknsIIDQsnFrButtonTbCornerBr,         ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2304},
    /* SP_QsnFrSctrlButtonSideT */      {KAknsIIDQsnFrButtonTbSideT,            ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2305},
    /* SP_QsnFrSctrlButtonSideB */      {KAknsIIDQsnFrButtonTbSideB,            ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2306},
    /* SP_QsnFrSctrlButtonSideL */      {KAknsIIDQsnFrButtonTbSideL,            ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2307},
    /* SP_QsnFrSctrlButtonSideR */      {KAknsIIDQsnFrButtonTbSideR,            ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2308},
    /* SP_QsnFrSctrlButtonCenter */     {KAknsIIDQsnFrButtonTbCenter,           ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2309}, /*KAknsIIDQgnFrSctrlButtonCenter*/

    // No pressed state for toolbar button in 3.1/3.2.
    /* SP_QsnFrSctrlButtonCornerTlPressed */ {KAknsIIDQsnFrButtonTbCornerTl,    ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2621},  /*KAknsIIDQsnFrSctrlButtonCornerTlPressed*/
    /* SP_QsnFrSctrlButtonCornerTrPressed */ {KAknsIIDQsnFrButtonTbCornerTr,    ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2622},
    /* SP_QsnFrSctrlButtonCornerBlPressed */ {KAknsIIDQsnFrButtonTbCornerBl,    ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2623},
    /* SP_QsnFrSctrlButtonCornerBrPressed */ {KAknsIIDQsnFrButtonTbCornerBr,    ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2624},
    /* SP_QsnFrSctrlButtonSideTPressed */    {KAknsIIDQsnFrButtonTbSideT,       ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2625},
    /* SP_QsnFrSctrlButtonSideBPressed */    {KAknsIIDQsnFrButtonTbSideB,       ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2626},
    /* SP_QsnFrSctrlButtonSideLPressed */    {KAknsIIDQsnFrButtonTbSideL,       ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2627},
    /* SP_QsnFrSctrlButtonSideRPressed */    {KAknsIIDQsnFrButtonTbSideR,       ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2628},
    /* SP_QsnFrSctrlButtonCenterPressed */   {KAknsIIDQsnFrButtonTbCenter,      ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x2629},

    // No inactive button graphics in 3.1/3.2
    /* SP_QsnFrButtonCornerTlInactive */ {KAknsIIDQsnFrButtonTbCornerTl,        ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b1}, /*KAknsIIDQsnFrButtonCornerTlInactive*/
    /* SP_QsnFrButtonCornerTrInactive */ {KAknsIIDQsnFrButtonTbCornerTr,        ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b2},
    /* SP_QsnFrButtonCornerBlInactive */ {KAknsIIDQsnFrButtonTbCornerBl,        ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b3},
    /* SP_QsnFrButtonCornerTrInactive */ {KAknsIIDQsnFrButtonTbCornerBr,        ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b4},
    /* SP_QsnFrButtonSideTInactive */    {KAknsIIDQsnFrButtonTbSideT,           ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b5},
    /* SP_QsnFrButtonSideBInactive */    {KAknsIIDQsnFrButtonTbSideB,           ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b6},
    /* SP_QsnFrButtonSideLInactive */    {KAknsIIDQsnFrButtonTbSideL,           ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b7},
    /* SP_QsnFrButtonSideRInactive */    {KAknsIIDQsnFrButtonTbSideR,           ENoDraw,     ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b8},
    /* SP_QsnFrButtonCenterInactive */   {KAknsIIDQsnFrButtonTbCenter,          EDrawIcon,   ES60_3_1 | ES60_3_2, EAknsMajorSkin, 0x21b9},

};

QPixmap QS60StyleModeSpecifics::skinnedGraphics(
    QS60StyleEnums::SkinParts stylepart, const QSize &size,
    QS60StylePrivate::SkinElementFlags flags)
{
    QPixmap themedImage;
    TRAPD( error, QT_TRYCATCH_LEAVING({
            const QPixmap skinnedImage = createSkinnedGraphicsLX(stylepart, size, flags);
            themedImage = skinnedImage;
    }));
    if (error)
        return themedImage = QPixmap();
    return themedImage;
}

QPixmap QS60StyleModeSpecifics::skinnedGraphics(
    QS60StylePrivate::SkinFrameElements frame, const QSize &size, QS60StylePrivate::SkinElementFlags flags)
{
    QPixmap themedImage;
    TRAPD( error, QT_TRYCATCH_LEAVING({
            const QPixmap skinnedImage = createSkinnedGraphicsLX(frame, size, flags);
            themedImage = skinnedImage;
    }));
    if (error)
        return themedImage = QPixmap();
    return themedImage;
}

QPixmap QS60StyleModeSpecifics::colorSkinnedGraphics(
    const QS60StyleEnums::SkinParts &stylepart, const QSize &size, QPainter *painter, 
    QS60StylePrivate::SkinElementFlags flags)
{
    QPixmap colorGraphics;
    TRAPD(error, QT_TRYCATCH_LEAVING(colorGraphics = colorSkinnedGraphicsLX(stylepart, size, painter, flags)));
    return error ? QPixmap() : colorGraphics;
}

void QS60StyleModeSpecifics::fallbackInfo(const QS60StyleEnums::SkinParts &stylepart, TDes& fallbackFileName, TInt& fallbackIndex)
{
    switch(stylepart) {
        case QS60StyleEnums::SP_QgnGrafBarWait:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_bar_wait_1;
            break;
        case QS60StyleEnums::SP_QgnGrafBarFrameCenter:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_bar_frame_center;
            break;
        case QS60StyleEnums::SP_QgnGrafBarFrameSideL:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_bar_frame_side_l;
            break;
        case QS60StyleEnums::SP_QgnGrafBarFrameSideR:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_bar_frame_side_r;
            break;
        case QS60StyleEnums::SP_QgnGrafBarProgress:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_bar_progress;
            break;
        case QS60StyleEnums::SP_QgnGrafTabActiveL:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_tab_active_l;
            break;
        case QS60StyleEnums::SP_QgnGrafTabActiveM:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_tab_active_m;
            break;
        case QS60StyleEnums::SP_QgnGrafTabActiveR:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_tab_active_r;
            break;
        case QS60StyleEnums::SP_QgnGrafTabPassiveL:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_tab_passive_l;
            break;
        case QS60StyleEnums::SP_QgnGrafTabPassiveM:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_tab_passive_m;
            break;
        case QS60StyleEnums::SP_QgnGrafTabPassiveR:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_graf_tab_passive_r;
            break;
        case QS60StyleEnums::SP_QgnIndiCheckboxOff:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_checkbox_off;
            break;
        case QS60StyleEnums::SP_QgnIndiCheckboxOn:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_checkbox_on;
            break;
        case QS60StyleEnums::SP_QgnIndiHlColSuper:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = 0x4456; /* EMbmAvkonQgn_indi_hl_col_super */
            break;
        case QS60StyleEnums::SP_QgnIndiHlExpSuper:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = 0x4458; /* EMbmAvkonQgn_indi_hl_exp_super */
            break;
        case QS60StyleEnums::SP_QgnIndiHlLineBranch:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = 0x445A; /* EMbmAvkonQgn_indi_hl_line_branch */
            break;
        case QS60StyleEnums::SP_QgnIndiHlLineEnd:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = 0x445C; /* EMbmAvkonQgn_indi_hl_line_end */
            break;
        case QS60StyleEnums::SP_QgnIndiHlLineStraight:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = 0x445E; /* EMbmAvkonQgn_indi_hl_line_straight */
            break;
        case QS60StyleEnums::SP_QgnIndiMarkedAdd:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_marked_add;
            break;
        case QS60StyleEnums::SP_QgnIndiNaviArrowLeft:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_navi_arrow_left;
            break;
        case QS60StyleEnums::SP_QgnIndiNaviArrowRight:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_navi_arrow_right;
            break;
        case QS60StyleEnums::SP_QgnIndiRadiobuttOff:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_radiobutt_off;
            break;
        case QS60StyleEnums::SP_QgnIndiRadiobuttOn:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_radiobutt_on;
            break;
        case QS60StyleEnums::SP_QgnIndiSliderEdit:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_slider_edit;
            break;
        case QS60StyleEnums::SP_QgnIndiSubMenu:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_indi_submenu;
            break;
        case QS60StyleEnums::SP_QgnNoteErased:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_note_erased;
            break;
        case QS60StyleEnums::SP_QgnNoteError:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_note_error;
            break;
        case QS60StyleEnums::SP_QgnNoteInfo:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_note_info;
            break;
        case QS60StyleEnums::SP_QgnNoteOk:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_note_ok;
            break;
        case QS60StyleEnums::SP_QgnNoteQuery:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_note_query;
            break;
        case QS60StyleEnums::SP_QgnNoteWarning:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_note_warning;
            break;
        case QS60StyleEnums::SP_QgnPropFileSmall:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_prop_file_small;
            break;
        case QS60StyleEnums::SP_QgnPropFolderCurrent:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_prop_folder_current;
            break;
        case QS60StyleEnums::SP_QgnPropFolderSmall:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_prop_folder_small;
            break;
        case QS60StyleEnums::SP_QgnPropFolderSmallNew:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_prop_folder_small_new;
            break;
        case QS60StyleEnums::SP_QgnPropPhoneMemcLarge:
            fallbackFileName = KAvkonBitmapFile();
            fallbackIndex = EMbmAvkonQgn_prop_phone_memc_large;
            break;
        default:
            fallbackFileName = KNullDesC();
            fallbackIndex = -1;
            break;
    }
}

QPixmap QS60StyleModeSpecifics::colorSkinnedGraphicsLX(
    const QS60StyleEnums::SkinParts &stylepart,
    const QSize &size, QPainter *painter, QS60StylePrivate::SkinElementFlags flags)
{
    // this function can throw both exceptions and leaves. There are no cleanup dependencies between Qt and Symbian parts.
    const int stylepartIndex = (int)stylepart;
    const TAknsItemID skinId = m_partMap[stylepartIndex].skinID;

    TInt fallbackGraphicID = -1;
    HBufC* iconFile = HBufC::NewLC( KMaxFileName );
    TPtr fileNamePtr = iconFile->Des();
    fallbackInfo(stylepart, fileNamePtr, fallbackGraphicID);

    TAknsItemID colorGroup = KAknsIIDQsnIconColors;
    TRgb defaultColor = KRgbBlack;
    int colorIndex = -1; //set a bogus value to color index to ensure that painter color is used
                         //to color the icon
    if (painter) {
        QRgb widgetColor = painter->pen().color().rgb();
        defaultColor = TRgb(qRed(widgetColor), qGreen(widgetColor), qBlue(widgetColor));
    }

    const bool rotatedBy90or270 =
        (flags & (QS60StylePrivate::SF_PointEast | QS60StylePrivate::SF_PointWest));
    const TSize targetSize =
        rotatedBy90or270?TSize(size.height(), size.width()):TSize(size.width(), size.height());
    CFbsBitmap *icon = 0;
    CFbsBitmap *iconMask = 0;
    const TInt fallbackGraphicsMaskID =
        fallbackGraphicID == KErrNotFound?KErrNotFound:fallbackGraphicID+1; //masks are auto-generated as next in mif files
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    AknsUtils::CreateColorIconLC(
        skinInstance, skinId, colorGroup, colorIndex, icon, iconMask, fileNamePtr, fallbackGraphicID , fallbackGraphicsMaskID, defaultColor);
    User::LeaveIfError(AknIconUtils::SetSize(icon, targetSize, EAspectRatioNotPreserved));
    User::LeaveIfError(AknIconUtils::SetSize(iconMask, targetSize, EAspectRatioNotPreserved));
    QPixmap result = fromFbsBitmap(icon, iconMask, flags, qt_TDisplayMode2Format(icon->DisplayMode()));
    CleanupStack::PopAndDestroy(3); //icon, iconMask, iconFile
    return result;
}

QColor QS60StyleModeSpecifics::colorValue(const TAknsItemID &colorGroup, int colorIndex)
{
    TRgb skinnedColor;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor(skin, skinnedColor, colorGroup, colorIndex);
    return QColor(skinnedColor.Red(),skinnedColor.Green(),skinnedColor.Blue());
}

struct QAutoFbsBitmapHeapLock
{
    QAutoFbsBitmapHeapLock(CFbsBitmap* aBmp) : mBmp(aBmp) { mBmp->LockHeap(); }
    ~QAutoFbsBitmapHeapLock() { mBmp->UnlockHeap(); }
    CFbsBitmap* mBmp;
};

QPixmap QS60StyleModeSpecifics::fromFbsBitmap(CFbsBitmap *icon, CFbsBitmap *mask, QS60StylePrivate::SkinElementFlags flags, QImage::Format format)
{
    Q_ASSERT(icon);
    const TSize iconSize = icon->SizeInPixels();
    const int iconBytesPerLine = CFbsBitmap::ScanLineLength(iconSize.iWidth, icon->DisplayMode());
    const int iconBytesCount = iconBytesPerLine * iconSize.iHeight;

    QImage iconImage(qt_TSize2QSize(iconSize), format);
    if (iconImage.isNull())
        return QPixmap();

    checkAndUnCompressBitmap(icon);
    if (!icon) //checkAndUnCompressBitmap might set icon to NULL
        return QPixmap();

    icon->LockHeap();
    const uchar *const iconBytes = (uchar*)icon->DataAddress();
    // The icon data needs to be copied, since the color format will be
    // automatically converted to Format_ARGB32 when setAlphaChannel is called.
    memcpy(iconImage.bits(), iconBytes, iconBytesCount);
    icon->UnlockHeap();
    if (mask) {
        checkAndUnCompressBitmap(mask);
        if (mask) { //checkAndUnCompressBitmap might set mask to NULL
            const TSize maskSize = icon->SizeInPixels();
            const int maskBytesPerLine = CFbsBitmap::ScanLineLength(maskSize.iWidth, mask->DisplayMode());
            // heap lock object required because QImage ctor might throw
            QAutoFbsBitmapHeapLock maskHeapLock(mask);
            const uchar *const maskBytes = (uchar *)mask->DataAddress();
            // Since no other bitmap should be locked, we can just "borrow" the mask data for setAlphaChannel
            const QImage maskImage(maskBytes, maskSize.iWidth, maskSize.iHeight, maskBytesPerLine, QImage::Format_Indexed8);
            if (!maskImage.isNull())
                iconImage.setAlphaChannel(maskImage);
        }
    }

    QTransform imageTransform;
    if (flags & QS60StylePrivate::SF_PointEast) {
        imageTransform.rotate(90);
    } else if (flags & QS60StylePrivate::SF_PointSouth) {
        imageTransform.rotate(180);
        iconImage = iconImage.transformed(imageTransform);
    } else if (flags & QS60StylePrivate::SF_PointWest) {
        imageTransform.rotate(270);
    }
    if (imageTransform.isRotating())
        iconImage = iconImage.transformed(imageTransform);

    return QPixmap::fromImage(iconImage);
}

bool QS60StylePrivate::isTouchSupported()
{
    return bool(AknLayoutUtils::PenEnabled());
}

bool QS60StylePrivate::isToolBarBackground()
{
    return (QSysInfo::s60Version() == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2);
}

QPoint qt_s60_fill_background_offset(const QWidget *targetWidget)
{
    CCoeControl *control = targetWidget->effectiveWinId();
    TPoint pos(0,0);
    if (control) {
        // FIXME properly: S60 3.1 has a bug that CCoeControl::PositionRelativeToScreen sometimes
        // freezes the device, possibly in cases where we run out of memory.
        // We use CCoeControl::Position instead in S60 3.1, which returns same values
        // in most cases.
        if (QSysInfo::s60Version() == QSysInfo::SV_S60_3_1)
            pos = control->Position();
        else
            pos = control->PositionRelativeToScreen();
    }
    return QPoint(pos.iX, pos.iY);
}

QPixmap QS60StyleModeSpecifics::createSkinnedGraphicsLX(
    QS60StyleEnums::SkinParts part, const QSize &size, 
    QS60StylePrivate::SkinElementFlags flags)
{
    // this function can throw both exceptions and leaves. There are no cleanup dependencies between Qt and Symbian parts.
    if (!size.isValid())
        return QPixmap();

    // Check release support and change part, if necessary.
    const TAknsItemID skinId = checkAndUpdateReleaseSpecificGraphics((int)part);
    const int stylepartIndex = (int)part;
    const TDrawType drawType = m_partMap[stylepartIndex].drawType;
    Q_ASSERT(drawType != ENoDraw);
    const bool rotatedBy90or270 =
        (flags & (QS60StylePrivate::SF_PointEast | QS60StylePrivate::SF_PointWest));
    const TSize targetSize =
        rotatedBy90or270 ? TSize(size.height(), size.width()) : qt_QSize2TSize(size);

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    QPixmap result;

    switch (drawType) {
        case EDrawIcon: {
            TInt fallbackGraphicID = -1;
            HBufC* iconFile = HBufC::NewLC( KMaxFileName );
            TPtr fileNamePtr = iconFile->Des();
            fallbackInfo(part, fileNamePtr, fallbackGraphicID);
            // todo: could we instead use   AknIconUtils::AvkonIconFileName(); to avoid allocating each time?

            CFbsBitmap *icon = 0;
            CFbsBitmap *iconMask = 0;
            const TInt fallbackGraphicsMaskID =
                fallbackGraphicID == KErrNotFound?KErrNotFound:fallbackGraphicID+1; //masks are auto-generated as next in mif files
    //        QS60WindowSurface::unlockBitmapHeap();
            AknsUtils::CreateIconLC(skinInstance, skinId, icon, iconMask, fileNamePtr, fallbackGraphicID , fallbackGraphicsMaskID);
            User::LeaveIfError(AknIconUtils::SetSize(icon, targetSize, EAspectRatioNotPreserved));
            User::LeaveIfError(AknIconUtils::SetSize(iconMask, targetSize, EAspectRatioNotPreserved));
            result = fromFbsBitmap(icon, iconMask, flags, qt_TDisplayMode2Format(icon->DisplayMode()));
            CleanupStack::PopAndDestroy(3); // iconMask, icon, iconFile
    //        QS60WindowSurface::lockBitmapHeap();
            break;
        }
        case EDrawBackground: {
    //        QS60WindowSurface::unlockBitmapHeap();
            CFbsBitmap *background = new (ELeave) CFbsBitmap(); //offscreen
            CleanupStack::PushL(background);
            User::LeaveIfError(background->Create(targetSize, EColor16MA));

            CFbsBitmapDevice *dev = CFbsBitmapDevice::NewL(background);
            CleanupStack::PushL(dev);
            CFbsBitGc *gc = NULL;
            User::LeaveIfError(dev->CreateContext(gc));
            CleanupStack::PushL(gc);

            CAknsBasicBackgroundControlContext *bgContext = CAknsBasicBackgroundControlContext::NewL(
                skinId,
                targetSize,
                EFalse);
            CleanupStack::PushL(bgContext);

            const TBool drawn = AknsDrawUtils::DrawBackground(
                skinInstance,
                bgContext,
                NULL,
                *gc,
                TPoint(),
                targetSize,
                KAknsDrawParamDefault | KAknsDrawParamRGBOnly);

            if (drawn)
                result = fromFbsBitmap(background, NULL, flags, QImage::Format_RGB32);

            CleanupStack::PopAndDestroy(4, background); //background, dev, gc, bgContext
    //        QS60WindowSurface::lockBitmapHeap();
            break;
        }
    }

    return result;
}

QPixmap QS60StyleModeSpecifics::createSkinnedGraphicsLX(QS60StylePrivate::SkinFrameElements frameElement,
    const QSize &size, QS60StylePrivate::SkinElementFlags flags)
{
    // this function can throw both exceptions and leaves. There are no cleanup dependencies between Qt and Symbian parts.
    if (!size.isValid())
        return QPixmap();

    const bool rotatedBy90or270 =
        (flags & (QS60StylePrivate::SF_PointEast | QS60StylePrivate::SF_PointWest));
    const TSize targetSize =
        rotatedBy90or270 ? TSize(size.height(), size.width()) : qt_QSize2TSize(size);

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    QPixmap result;

//        QS60WindowSurface::unlockBitmapHeap();
    static const TDisplayMode displayMode = S60->supportsPremultipliedAlpha ? Q_SYMBIAN_ECOLOR16MAP : EColor16MA;
    static const TInt drawParam = S60->supportsPremultipliedAlpha ? KAknsDrawParamDefault : KAknsDrawParamNoClearUnderImage|KAknsDrawParamRGBOnly;

    CFbsBitmap *frame = new (ELeave) CFbsBitmap(); //offscreen
    CleanupStack::PushL(frame);
    User::LeaveIfError(frame->Create(targetSize, displayMode));

    CFbsBitmapDevice* bitmapDev = CFbsBitmapDevice::NewL(frame);
    CleanupStack::PushL(bitmapDev);
    CFbsBitGc* bitmapGc = NULL;
    User::LeaveIfError(bitmapDev->CreateContext(bitmapGc));
    CleanupStack::PushL(bitmapGc);

    frame->LockHeap();
    memset(frame->DataAddress(), 0, frame->SizeInPixels().iWidth * frame->SizeInPixels().iHeight * 4);  // 4: argb bytes
    frame->UnlockHeap();

    const TRect outerRect(TPoint(0, 0), targetSize);
    const TRect innerRect = innerRectFromElement(frameElement, outerRect);

    TAknsItemID frameSkinID, centerSkinID;
    frameSkinID = centerSkinID = checkAndUpdateReleaseSpecificGraphics(QS60StylePrivate::m_frameElementsData[frameElement].center);
    frameIdAndCenterId(frameElement, frameSkinID, centerSkinID);
    const TBool drawn = AknsDrawUtils::DrawFrame( skinInstance,
                           *bitmapGc, outerRect, innerRect,
                           frameSkinID, centerSkinID,
                           drawParam );

    if (S60->supportsPremultipliedAlpha) {
        if (drawn)
            result = fromFbsBitmap(frame, NULL, flags, QImage::Format_ARGB32_Premultiplied);
    } else {
        TDisplayMode maskDepth = EGray2;
        // Query the skin item for possible frame graphics mask details.
        if (skinInstance) {
            CAknsMaskedBitmapItemData* skinMaskedBmp = static_cast<CAknsMaskedBitmapItemData*>(
                    skinInstance->GetCachedItemData(frameSkinID,EAknsITMaskedBitmap));
            if (skinMaskedBmp && skinMaskedBmp->Mask())
                maskDepth = skinMaskedBmp->Mask()->DisplayMode();
        }
        if (maskDepth != ENone) {
            CFbsBitmap *frameMask = new (ELeave) CFbsBitmap(); //offscreen
            CleanupStack::PushL(frameMask);
            User::LeaveIfError(frameMask->Create(targetSize, maskDepth));

            CFbsBitmapDevice* maskBitmapDevice = CFbsBitmapDevice::NewL(frameMask);
            CleanupStack::PushL(maskBitmapDevice);
            CFbsBitGc* maskBitGc = NULL;
            User::LeaveIfError(maskBitmapDevice->CreateContext(maskBitGc));
            CleanupStack::PushL(maskBitGc);

            if (drawn) {
                //ensure that the mask is really transparent
                maskBitGc->Activate( maskBitmapDevice );
                maskBitGc->SetPenStyle(CGraphicsContext::ENullPen);
                maskBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
                maskBitGc->SetBrushColor(KRgbWhite);
                maskBitGc->Clear();
                maskBitGc->SetBrushStyle(CGraphicsContext::ENullBrush);

                AknsDrawUtils::DrawFrame(skinInstance,
                                           *maskBitGc, outerRect, innerRect,
                                           frameSkinID, centerSkinID,
                                           KAknsSDMAlphaOnly |KAknsDrawParamNoClearUnderImage);
                result = fromFbsBitmap(frame, frameMask, flags, QImage::Format_ARGB32);
            }
            CleanupStack::PopAndDestroy(3, frameMask);
        }
    }
    CleanupStack::PopAndDestroy(3, frame); //frame, bitmapDev, bitmapGc
    return result;
}

void QS60StyleModeSpecifics::frameIdAndCenterId(QS60StylePrivate::SkinFrameElements frameElement, TAknsItemID &frameId, TAknsItemID &centerId)
{
// There are some major mix-ups in skin declarations for some frames.
// First, the frames are not declared in sequence.
// Second, the parts use different major than the frame-master.

    switch(frameElement) {
        case QS60StylePrivate::SF_ToolTip:
            if (QSysInfo::s60Version()!=QSysInfo::SV_S60_3_1) {
                centerId.Set(EAknsMajorGeneric, 0x19c2);
                frameId.Set(EAknsMajorSkin, 0x5300);
            } else {
                centerId.Set(KAknsIIDQsnFrPopupCenter);
                frameId.iMinor = centerId.iMinor - 9;
            }
            break;
        case QS60StylePrivate::SF_ToolBar:
            if (QSysInfo::s60Version()==QSysInfo::SV_S60_3_1 || QSysInfo::s60Version()==QSysInfo::SV_S60_3_2) {
                centerId.Set(KAknsIIDQsnFrPopupCenterSubmenu);
                frameId.Set(KAknsIIDQsnFrPopupSub);
            }
            break;
        case QS60StylePrivate::SF_PanelBackground:
            // remove center piece for panel graphics, so that only border is drawn
            centerId.Set(KAknsIIDNone);
            frameId.Set(KAknsIIDQsnFrSetOpt);
            break;
        default:
            // center should be correct here
            frameId.iMinor = centerId.iMinor - 9;
            break;
    }
}

TRect QS60StyleModeSpecifics::innerRectFromElement(QS60StylePrivate::SkinFrameElements frameElement, const TRect &outerRect)
{
    TInt widthShrink = QS60StylePrivate::pixelMetric(PM_Custom_FrameCornerWidth);
    TInt heightShrink = QS60StylePrivate::pixelMetric(PM_Custom_FrameCornerHeight);
    switch(frameElement) {
        case QS60StylePrivate::SF_PanelBackground:
            // panel should have slightly slimmer border to enable thin line of background graphics between closest component
            widthShrink = widthShrink-2;
            heightShrink = heightShrink-2;
            break;
        case QS60StylePrivate::SF_ToolTip:
            widthShrink = widthShrink>>1;
            heightShrink = heightShrink>>1;
            break;
        case QS60StylePrivate::SF_ListHighlight:
            widthShrink = widthShrink-2;
            heightShrink = heightShrink-2;
            break;
        default:
            break;
    }
    TRect innerRect(outerRect);
    innerRect.Shrink(widthShrink, heightShrink);
    return innerRect;
}

bool QS60StyleModeSpecifics::checkSupport(const int supportedRelease)
{
    const QSysInfo::S60Version currentRelease = QSysInfo::s60Version();
    return ( (currentRelease == QSysInfo::SV_S60_3_1 && supportedRelease & ES60_3_1) ||
             (currentRelease == QSysInfo::SV_S60_3_2 && supportedRelease & ES60_3_2) ||
             (currentRelease == QSysInfo::SV_S60_5_0 && supportedRelease & ES60_5_0));
}

TAknsItemID QS60StyleModeSpecifics::checkAndUpdateReleaseSpecificGraphics(int part)
{
    TAknsItemID newSkinId;
    if (!checkSupport(m_partMap[(int)part].supportInfo))
        newSkinId.Set(m_partMap[(int)part].newMajorSkinId, m_partMap[(int)part].newMinorSkinId);
    else
        newSkinId.Set(m_partMap[(int)part].skinID);
    return newSkinId;
}

void QS60StyleModeSpecifics::checkAndUnCompressBitmap(CFbsBitmap*& aOriginalBitmap)
{
    TRAPD(error, checkAndUnCompressBitmapL(aOriginalBitmap));
    if (error)
        aOriginalBitmap = NULL;
}

void QS60StyleModeSpecifics::checkAndUnCompressBitmapL(CFbsBitmap*& aOriginalBitmap)
{
    const TSize iconSize = aOriginalBitmap->SizeInPixels();
    const int iconBytesPerLine = CFbsBitmap::ScanLineLength(iconSize.iWidth, aOriginalBitmap->DisplayMode());
    const int iconBytesCount = iconBytesPerLine * iconSize.iHeight;
    if (aOriginalBitmap->IsCompressedInRAM() || aOriginalBitmap->Header().iBitmapSize < iconBytesCount) {
        const TSize iconSize(aOriginalBitmap->SizeInPixels().iWidth,
            aOriginalBitmap->SizeInPixels().iHeight);
        CFbsBitmap* uncompressedBitmap = new (ELeave) CFbsBitmap();
        CleanupStack::PushL(uncompressedBitmap);
        User::LeaveIfError(uncompressedBitmap->Create(iconSize,
            aOriginalBitmap->DisplayMode()));
        unCompressBitmapL(iconSize, uncompressedBitmap, aOriginalBitmap);
        CleanupStack::Pop(uncompressedBitmap);
        User::LeaveIfError(aOriginalBitmap->Duplicate(
            uncompressedBitmap->Handle()));
        delete uncompressedBitmap;
    }
}

QFont QS60StylePrivate::s60Font_specific(
    QS60StyleEnums::FontCategories fontCategory, int pointSize)
{
    TAknFontCategory aknFontCategory = EAknFontCategoryUndefined;
    switch (fontCategory) {
        case QS60StyleEnums::FC_Primary:
            aknFontCategory = EAknFontCategoryPrimary;
            break;
        case QS60StyleEnums::FC_Secondary:
            aknFontCategory = EAknFontCategorySecondary;
            break;
        case QS60StyleEnums::FC_Title:
            aknFontCategory = EAknFontCategoryTitle;
            break;
        case QS60StyleEnums::FC_PrimarySmall:
            aknFontCategory = EAknFontCategoryPrimarySmall;
            break;
        case QS60StyleEnums::FC_Digital:
            aknFontCategory = EAknFontCategoryDigital;
            break;
        case QS60StyleEnums::FC_Undefined:
        default:
            break;
    }

    // Create AVKON font according the given parameters
    CWsScreenDevice* dev = CCoeEnv::Static()->ScreenDevice();
    TAknFontSpecification spec(aknFontCategory, TFontSpec(), NULL);
    if (pointSize > 0) {
        const TInt pixelSize = dev->VerticalTwipsToPixels(pointSize * KTwipsPerPoint);
        spec.SetTextPaneHeight(pixelSize + 4); // TODO: Is 4 a reasonable top+bottom margin?
    }

    QFont result;
    TRAPD( error, QT_TRYCATCH_LEAVING({
        const CAknLayoutFont* aknFont =
            AknFontAccess::CreateLayoutFontFromSpecificationL(*dev, spec);

        result = qt_TFontSpec2QFontL(aknFont->DoFontSpecInTwips());
        if (result.pointSize() != pointSize)
            result.setPointSize(pointSize); // Correct the font size returned by CreateLayoutFontFromSpecificationL()

        delete aknFont;
    }));
    if (error) result = QFont();
    return result;
}

void QS60StylePrivate::setActiveLayout()
{
    const QSize activeScreenSize(screenSize());
    int activeLayoutIndex = -1;
    const bool mirrored = !QApplication::isLeftToRight();
    const short screenHeight = (short)activeScreenSize.height();
    const short screenWidth = (short)activeScreenSize.width();
    for (int i=0; i<m_numberOfLayouts; i++) {
        if (screenHeight==m_layoutHeaders[i].height &&
            screenWidth==m_layoutHeaders[i].width &&
            mirrored==m_layoutHeaders[i].mirroring) {
            activeLayoutIndex = i;
            break;
        }
    }

    //not found, lets try without mirroring info
    if (activeLayoutIndex==-1){
        for (int i=0; i<m_numberOfLayouts; i++) {
            if (screenHeight==m_layoutHeaders[i].height &&
                screenWidth==m_layoutHeaders[i].width) {
                activeLayoutIndex = i;
                break;
            }
        }
    }

    //not found, lets try with either of dimensions
    if (activeLayoutIndex==-1){
        const QSysInfo::S60Version currentRelease = QSysInfo::s60Version();
        const bool landscape = screenHeight < screenWidth;

        activeLayoutIndex = (currentRelease == QSysInfo::SV_S60_3_1 || currentRelease == QSysInfo::SV_S60_3_2) ? 0 : 4;
        activeLayoutIndex += (!landscape) ? 2 : 0;
        activeLayoutIndex += (!mirrored) ? 1 : 0;
    }

    m_pmPointer = data[activeLayoutIndex];
}

QS60StylePrivate::QS60StylePrivate()
{
    // No need to set active layout, if dynamic metrics API is available
    setActiveLayout();
}

QColor QS60StylePrivate::s60Color(QS60StyleEnums::ColorLists list,
    int index, const QStyleOption *option)
{
    static const TAknsItemID *idMap[] = {
        &KAknsIIDQsnHighlightColors,
        &KAknsIIDQsnIconColors,
        &KAknsIIDQsnLineColors,
        &KAknsIIDQsnOtherColors,
        &KAknsIIDQsnParentColors,
        &KAknsIIDQsnTextColors
    };
    Q_ASSERT((int)list <= (int)sizeof(idMap)/sizeof(idMap[0]));
    const QColor color = QS60StyleModeSpecifics::colorValue(*idMap[(int) list], index - 1);
    return option ? QS60StylePrivate::stateColor(color, option) : color;
}

// In some cases, the AVKON UI themegraphic is already in 'disabled state'.
// If so, return true for these parts.
bool QS60StyleModeSpecifics::disabledPartGraphic(QS60StyleEnums::SkinParts &part)
{
    bool disabledGraphic = false;
    switch(part){
        // inactive button graphics are available from 5.0 onwards
        case QS60StyleEnums::SP_QsnFrButtonCornerTlInactive:
        case QS60StyleEnums::SP_QsnFrButtonCornerTrInactive:
        case QS60StyleEnums::SP_QsnFrButtonCornerBlInactive:
        case QS60StyleEnums::SP_QsnFrButtonCornerBrInactive:
        case QS60StyleEnums::SP_QsnFrButtonSideTInactive:
        case QS60StyleEnums::SP_QsnFrButtonSideBInactive:
        case QS60StyleEnums::SP_QsnFrButtonSideLInactive:
        case QS60StyleEnums::SP_QsnFrButtonSideRInactive:
        case QS60StyleEnums::SP_QsnFrButtonCenterInactive:
            if (!(QSysInfo::s60Version()==QSysInfo::SV_S60_3_1 ||
                  QSysInfo::s60Version()==QSysInfo::SV_S60_3_2))
                disabledGraphic = true;
            break;
        default:
            break;
    }
    return disabledGraphic;
}

// In some cases, the AVKON UI themegraphic is already in 'disabled state'.
// If so, return true for these frames.
bool QS60StyleModeSpecifics::disabledFrameGraphic(QS60StylePrivate::SkinFrameElements &frame)
{
    bool disabledGraphic = false;
    switch(frame){
        // inactive button graphics are available from 5.0 onwards
        case QS60StylePrivate::SF_ButtonInactive:
            if (!(QSysInfo::s60Version()==QSysInfo::SV_S60_3_1 ||
                  QSysInfo::s60Version()==QSysInfo::SV_S60_3_2))
                disabledGraphic = true;
            break;
        default:
            break;
    }
    return disabledGraphic;
}

QPixmap QS60StyleModeSpecifics::generateMissingThemeGraphic(QS60StyleEnums::SkinParts &part,
        const QSize &size, QS60StylePrivate::SkinElementFlags flags)
{
    if (!QS60StylePrivate::isTouchSupported())
        return QPixmap();

    QS60StyleEnums::SkinParts updatedPart = part;
    switch(part){
    // AVKON UI has a abnormal handling for scrollbar graphics. It is possible that the root
    // skin does not contain mandatory graphics for scrollbar pressed states. Therefore, AVKON UI
    // creates dynamically these graphics by modifying the normal state scrollbar graphics slightly.
    // S60Style needs to work similarly. Therefore if skingraphics call provides to be a miss
    // (i.e. result is not valid), style needs to draw normal graphics instead and apply some
    // modifications (similar to generatedIconPixmap()) to the result.
    case QS60StyleEnums::SP_QsnCpScrollHandleBottomPressed:
        updatedPart = QS60StyleEnums::SP_QsnCpScrollHandleBottom;
        break;
    case QS60StyleEnums::SP_QsnCpScrollHandleMiddlePressed:
        updatedPart = QS60StyleEnums::SP_QsnCpScrollHandleMiddle;
        break;
    case QS60StyleEnums::SP_QsnCpScrollHandleTopPressed:
        updatedPart = QS60StyleEnums::SP_QsnCpScrollHandleTop;
        break;
    default:
        break;
    }
    if (part==updatedPart) {
        return QPixmap();
    } else {
        QPixmap result = skinnedGraphics(updatedPart, size, flags);
        QStyleOption opt;
        QPalette *themePalette = QS60StylePrivate::themePalette();
        if (themePalette)
            opt.palette = *themePalette;

        // For now, always generate new icon based on "selected". In the future possibly, expand
        // this to consist other possibilities as well.
        result = QApplication::style()->generatedIconPixmap(QIcon::Selected, result, &opt);
        return result;
    }
}

QPixmap QS60StylePrivate::part(QS60StyleEnums::SkinParts part,
    const QSize &size, QPainter *painter, SkinElementFlags flags)
{
    QSymbianFbsHeapLock lock(QSymbianFbsHeapLock::Unlock);

    QPixmap result = (flags & SF_ColorSkinned)?
          QS60StyleModeSpecifics::colorSkinnedGraphics(part, size, painter, flags)
        : QS60StyleModeSpecifics::skinnedGraphics(part, size, flags);

    lock.relock();

    if (flags & SF_StateDisabled && !QS60StyleModeSpecifics::disabledPartGraphic(part)) {
        QStyleOption opt;
        QPalette *themePalette = QS60StylePrivate::themePalette();
        if (themePalette)
            opt.palette = *themePalette;
        result = QApplication::style()->generatedIconPixmap(QIcon::Disabled, result, &opt);
    }

    if (!result)
        result = QS60StyleModeSpecifics::generateMissingThemeGraphic(part, size, flags);

    return result;
}

QPixmap QS60StylePrivate::frame(SkinFrameElements frame, const QSize &size, SkinElementFlags flags)
{
    QSymbianFbsHeapLock lock(QSymbianFbsHeapLock::Unlock);
    QPixmap result = QS60StyleModeSpecifics::skinnedGraphics(frame, size, flags);
    lock.relock();

    if (flags & SF_StateDisabled && !QS60StyleModeSpecifics::disabledFrameGraphic(frame)) {
        QStyleOption opt;
        QPalette *themePalette = QS60StylePrivate::themePalette();
        if (themePalette)
            opt.palette = *themePalette;
        result = QApplication::style()->generatedIconPixmap(QIcon::Disabled, result, &opt);
    }
    return result;
}

QPixmap QS60StylePrivate::backgroundTexture()
{
    if (!m_background) {
        QPixmap background = part(QS60StyleEnums::SP_QsnBgScreen,
                QSize(S60->screenWidthInPixels, S60->screenHeightInPixels), 0, SkinElementFlags());
        m_background = new QPixmap(background);
    }
    return *m_background;
}

// If the public SDK returns compressed images, please let us also uncompress those!
void QS60StyleModeSpecifics::unCompressBitmapL(const TRect& aTrgRect, CFbsBitmap* aTrgBitmap, CFbsBitmap* aSrcBitmap)
{
    if (!aSrcBitmap)
        User::Leave(KErrArgument);
    if (!aTrgBitmap)
        User::Leave(KErrArgument);

   // Note! aSrcBitmap->IsCompressedInRAM() is always ETrue, since this method is called only if that applies!
   // Extra note! this function is also being used when bitmaps appear to be compressed (because DataSize is too small)
   // even when they pretend they are not. Assert removed.
//   ASSERT(aSrcBitmap->IsCompressedInRAM());

    TDisplayMode displayMode = aSrcBitmap->DisplayMode();

    if (displayMode != aTrgBitmap->DisplayMode())
        User::Leave(KErrArgument);

    const TSize trgSize = aTrgBitmap->SizeInPixels();
    const TSize srcSize = aSrcBitmap->SizeInPixels();

    // calculate the valid drawing area
    TRect drawRect = aTrgRect;
    drawRect.Intersection(TRect(TPoint(0, 0), trgSize));

    if (drawRect.IsEmpty())
        return;

    CFbsBitmap* realSource = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(realSource);
    User::LeaveIfError(realSource->Create(srcSize, displayMode));
    CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(realSource);
    CleanupStack::PushL(dev);
    CFbsBitGc* gc = NULL;
    User::LeaveIfError(dev->CreateContext(gc));
    CleanupStack::PushL(gc);
    gc->BitBlt(TPoint(0, 0), aSrcBitmap);
    CleanupStack::PopAndDestroy(2); // dev, gc

    // Heap lock for FBServ large chunk is only needed with large bitmaps.
    if (realSource->IsLargeBitmap() || aTrgBitmap->IsLargeBitmap()) {
        aTrgBitmap->LockHeapLC(ETrue); // fbsheaplock
    } else {
        CleanupStack::PushL((TAny*) NULL);
    }

    TUint32* srcAddress = realSource->DataAddress();
    TUint32* trgAddress = aTrgBitmap->DataAddress();

    const TInt xSkip = (srcSize.iWidth << 8) / aTrgRect.Width();
    const TInt ySkip = (srcSize.iHeight << 8) / aTrgRect.Height();

    const TInt drawWidth = drawRect.Width();
    const TInt drawHeight = drawRect.Height();

    const TRect offsetRect(aTrgRect.iTl, drawRect.iTl);
    const TInt yPosOffset = ySkip * offsetRect.Height();
    const TInt xPosOffset = xSkip * offsetRect.Width();

    if ((displayMode == EGray256) || (displayMode == EColor256)) {
        const TInt srcScanLen8 = CFbsBitmap::ScanLineLength(srcSize.iWidth,
            displayMode);
        const TInt trgScanLen8 = CFbsBitmap::ScanLineLength(trgSize.iWidth,
            displayMode);

        TUint8* trgAddress8 = reinterpret_cast<TUint8*> (trgAddress);

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress8 += trgScanLen8 * drawRect.iTl.iY + drawRect.iTl.iX;

        for (TInt y = 0; y < drawHeight; y++) {
            const TUint8* srcAddress8 = reinterpret_cast<const TUint8*> (srcAddress)
                + (srcScanLen8 * (yPos >> 8));

            TInt xPos = xPosOffset;
            for (TInt x = 0; x < drawWidth; x++) {
                *(trgAddress8++) = srcAddress8[xPos >> 8];
                xPos += xSkip;
            }

            yPos += ySkip;

            trgAddress8 += trgScanLen8 - drawWidth;
        }
    } else if (displayMode == EColor4K || displayMode == EColor64K) {
        const TInt srcScanLen16 = CFbsBitmap::ScanLineLength(srcSize.iWidth,
            displayMode) >>1;
        const TInt trgScanLen16 = CFbsBitmap::ScanLineLength(trgSize.iWidth,
            displayMode) >>1;

        TUint16* trgAddress16 = reinterpret_cast<TUint16*> (trgAddress);

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress16 += trgScanLen16 * drawRect.iTl.iY + drawRect.iTl.iX;

        for (TInt y = 0; y < drawHeight; y++) {
            const TUint16* srcAddress16 = reinterpret_cast<const TUint16*> (srcAddress)
                + (srcScanLen16 * (yPos >> 8));

            TInt xPos = xPosOffset;
            for (TInt x = 0; x < drawWidth; x++) {
                *(trgAddress16++) = srcAddress16[xPos >> 8];
                xPos += xSkip;
            }

            yPos += ySkip;

            trgAddress16 += trgScanLen16 - drawWidth;
        }
    } else if (displayMode == EColor16MU || displayMode == EColor16MA) {
        const TInt srcScanLen32 = CFbsBitmap::ScanLineLength(srcSize.iWidth,
            displayMode) >>2;
        const TInt trgScanLen32 = CFbsBitmap::ScanLineLength(trgSize.iWidth,
            displayMode) >>2;

        TUint32* trgAddress32 = reinterpret_cast<TUint32*> (trgAddress);

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress32 += trgScanLen32 * drawRect.iTl.iY + drawRect.iTl.iX;

        for (TInt y = 0; y < drawHeight; y++) {
            const TUint32* srcAddress32 = reinterpret_cast<const TUint32*> (srcAddress)
                + (srcScanLen32 * (yPos >> 8));

            TInt xPos = xPosOffset;
            for (TInt x = 0; x < drawWidth; x++) {
                *(trgAddress32++) = srcAddress32[xPos >> 8];
                xPos += xSkip;
            }

            yPos += ySkip;

            trgAddress32 += trgScanLen32 - drawWidth;
        }
    } else { User::Leave(KErrUnknown);}

    CleanupStack::PopAndDestroy(2); // fbsheaplock, realSource
}

QSize QS60StylePrivate::screenSize()
{
    const TSize screenSize = QS60Data::screenDevice()->SizeInPixels();
    return QSize(screenSize.iWidth, screenSize.iHeight);
}

QS60Style::QS60Style()
    : QCommonStyle(*new QS60StylePrivate)
{
}

#ifdef Q_WS_S60
void QS60StylePrivate::handleDynamicLayoutVariantSwitch()
{
    clearCaches(QS60StylePrivate::CC_LayoutChange);
    setBackgroundTexture(qApp);
    setActiveLayout();
    refreshUI();
    foreach (QWidget *widget, QApplication::allWidgets())
        widget->ensurePolished();
}

void QS60StylePrivate::handleSkinChange()
{
    clearCaches(QS60StylePrivate::CC_ThemeChange);
    setThemePalette(qApp);
    foreach (QWidget *topLevelWidget, QApplication::allWidgets()){
        QEvent e(QEvent::StyleChange);
        QApplication::sendEvent(topLevelWidget, &e);
        setThemePalette(topLevelWidget);
        topLevelWidget->ensurePolished();
    }
}

QSize QS60StylePrivate::naviPaneSize()
{
    return QS60StyleModeSpecifics::naviPaneSize();
}

QSize QS60StyleModeSpecifics::naviPaneSize()
{
    CAknNavigationControlContainer* naviContainer;
    if (S60->statusPane()) {
        TRAPD(err, naviContainer = static_cast<CAknNavigationControlContainer*>
            (S60->statusPane()->ControlL(TUid::Uid(EEikStatusPaneUidNavi))));
        if (err==KErrNone)
            return QSize(naviContainer->Size().iWidth, naviContainer->Size().iHeight);
    }
    return QSize(0,0);
}

#endif // Q_WS_S60

QT_END_NAMESPACE

#endif // QT_NO_STYLE_S60 || QT_PLUGIN
