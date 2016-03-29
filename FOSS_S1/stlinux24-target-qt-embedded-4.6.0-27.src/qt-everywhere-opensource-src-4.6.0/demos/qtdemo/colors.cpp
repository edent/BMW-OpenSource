/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "colors.h"

#ifndef QT_NO_OPENGL
    #include <QGLWidget>
#endif
//#define QT_NO_OPENGL

// Colors:
QColor Colors::sceneBg1(QColor(91, 91, 91));
QColor Colors::sceneBg1Line(QColor(114, 108, 104));
QColor Colors::sceneBg2(QColor(0, 0, 0));
QColor Colors::sceneLine(255, 255, 255);
QColor Colors::paperBg(QColor(100, 100, 100));
QColor Colors::menuTextFg(QColor(255, 0, 0));
QColor Colors::buttonBgLow(QColor(255, 255, 255, 90));
QColor Colors::buttonBgHigh(QColor(255, 255, 255, 20));
QColor Colors::buttonText(QColor(255, 255, 255));
QColor Colors::tt_green(QColor(166, 206, 57));
QColor Colors::fadeOut(QColor(206, 246, 117, 0));
QColor Colors::heading(QColor(190,230,80));
QString Colors::contentColor("<font color='#eeeeee'>");
QString Colors::glVersion("Not detected!");

// Guides:
int Colors::stageStartY = 8;
int Colors::stageHeight = 536;
int Colors::stageStartX = 8;
int Colors::stageWidth = 785;
int Colors::contentStartY = 22;
int Colors::contentHeight = 510;

// Properties:
bool Colors::openGlRendering = false;
bool Colors::softwareRendering = false;
bool Colors::openGlAvailable = true;
bool Colors::xRenderPresent = true;

bool Colors::noTicker = false;
bool Colors::noRescale = false;
bool Colors::noAnimations = false;
bool Colors::noBlending = false;
bool Colors::noScreenSync = false;
bool Colors::fullscreen = false;
bool Colors::usePixmaps = false;
bool Colors::useLoop = false;
bool Colors::showBoundingRect = false;
bool Colors::showFps = false;
bool Colors::noAdapt = false;
bool Colors::noWindowMask = true;
bool Colors::useButtonBalls = false;
bool Colors::useEightBitPalette = false;
bool Colors::noTimerUpdate = false;
bool Colors::noTickerMorph = false;
bool Colors::adapted = false;
bool Colors::verbose = false;
bool Colors::pause = true;
int Colors::fps = 60;
int Colors::menuCount = 18;
float Colors::animSpeed = 1.0;
float Colors::animSpeedButtons = 1.0;
float Colors::benchmarkFps = -1;
int Colors::tickerLetterCount = 80;
float Colors::tickerMoveSpeed = 0.4f;
float Colors::tickerMorphSpeed = 2.5f;
QString Colors::tickerText = ".EROM ETAERC .SSEL EDOC";
QString Colors::rootMenuName = "Qt Examples and Demos";

QFont Colors::contentFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
#if defined(Q_OS_MAC)
    font.setPixelSize(14);
    font.setFamily("Arial");
#else
    font.setPixelSize(13);
    font.setFamily("Verdana");
#endif
    return font;
}

QFont Colors::headingFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(23);
    font.setBold(true);
    font.setFamily("Verdana");
    return font;
}

QFont Colors::buttonFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
#if 0//defined(Q_OS_MAC)
    font.setPixelSize(11);
    font.setFamily("Silom");
#else
    font.setPixelSize(11);
    font.setFamily("Verdana");
#endif
    return font;
}

QFont Colors::tickerFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
#if defined(Q_OS_MAC)
    font.setPixelSize(11);
    font.setBold(true);
    font.setFamily("Arial");
#else
    font.setPixelSize(10);
    font.setBold(true);
    font.setFamily("sans serif");
#endif
    return font;
}

float parseFloat(const QString &argument, const QString &name)
{
    if (name.length() == argument.length()){
        QMessageBox::warning(0, "Arguments",
                                 QString("No argument number found for ")
                                 + name
                                 + ". Remember to put name and value adjacent! (e.g. -fps100)");
        exit(0);
    }
    float value = argument.mid(name.length()).toFloat();
    return value;
}

QString parseText(const QString &argument, const QString &name)
{
    if (name.length() == argument.length()){
        QMessageBox::warning(0, "Arguments",
                                 QString("No argument number found for ")
                                 + name
                                 + ". Remember to put name and value adjacent! (e.g. -fps100)");
        exit(0);
    }
    QString value = argument.mid(name.length());
    return value;
}

void Colors::parseArgs(int argc, char *argv[])
{
    // some arguments should be processed before
    // others. Handle them now:
    for (int i=1; i<argc; i++){
        QString s(argv[i]);
        if (s == "-verbose")
            Colors::verbose = true;
    }

    Colors::detectSystemResources();

    // Handle the rest of the arguments. They may
    // override attributes already set:
    for (int i=1; i<argc; i++){
        QString s(argv[i]);
        if (s == "-opengl")
            Colors::openGlRendering = true;
        else if (s == "-software")
            Colors::softwareRendering = true;
        else if (s == "-no-opengl") // support old style
            Colors::softwareRendering = true;
        else if (s == "-no-ticker") // support old style
            Colors::noTicker = true;
        else if (s.startsWith("-ticker"))
            Colors::noTicker = !bool(parseFloat(s, "-ticker"));
        else if (s == "-no-animations")
            Colors::noAnimations = true; // support old style
        else if (s.startsWith("-animations"))
            Colors::noAnimations = !bool(parseFloat(s, "-animations"));
        else if (s == "-no-adapt")
            Colors::noAdapt = true;
        else if (s == "-low")
            Colors::setLowSettings();
        else if (s == "-no-rescale")
            Colors::noRescale = true;
        else if (s == "-use-pixmaps")
            Colors::usePixmaps = true;
        else if (s == "-fullscreen")
            Colors::fullscreen = true;
        else if (s == "-show-br")
            Colors::showBoundingRect = true;
        else if (s == "-show-fps")
            Colors::showFps = true;
        else if (s == "-no-blending")
            Colors::noBlending = true;
        else if (s == "-no-sync")
            Colors::noScreenSync = true;
        else if (s.startsWith("-menu"))
            Colors::menuCount = int(parseFloat(s, "-menu"));
        else if (s.startsWith("-use-timer-update"))
            Colors::noTimerUpdate = !bool(parseFloat(s, "-use-timer-update"));
        else if (s.startsWith("-pause"))
            Colors::pause = bool(parseFloat(s, "-pause"));
        else if (s == "-no-ticker-morph")
            Colors::noTickerMorph = true;
        else if (s == "-use-window-mask")
            Colors::noWindowMask = false;
        else if (s == "-use-loop")
            Colors::useLoop = true;
        else if (s == "-use-8bit")
            Colors::useEightBitPalette = true;
        else if (s.startsWith("-8bit"))
            Colors::useEightBitPalette = bool(parseFloat(s, "-8bit"));
        else if (s == "-use-balls")
            Colors::useButtonBalls = true;
        else if (s.startsWith("-ticker-letters"))
            Colors::tickerLetterCount = int(parseFloat(s, "-ticker-letters"));
        else if (s.startsWith("-ticker-text"))
            Colors::tickerText = parseText(s, "-ticker-text");
        else if (s.startsWith("-ticker-speed"))
            Colors::tickerMoveSpeed = parseFloat(s, "-ticker-speed");
        else if (s.startsWith("-ticker-morph-speed"))
            Colors::tickerMorphSpeed = parseFloat(s, "-ticker-morph-speed");
        else if (s.startsWith("-animation-speed"))
            Colors::animSpeed = parseFloat(s, "-animation-speed");
        else if (s.startsWith("-fps"))
            Colors::fps = int(parseFloat(s, "-fps"));
        else if (s.startsWith("-h") || s.startsWith("-help")){
            QMessageBox::warning(0, "Arguments",
                                 QString("Usage: qtdemo [-verbose] [-no-adapt] [-opengl] [-software] [-fullscreen] [-ticker[0|1]] ")
                                 + "[-animations[0|1]] [-no-blending] [-no-sync] [-use-timer-update[0|1]] [-pause[0|1]]  "
                                 + "[-use-window-mask] [-no-rescale] "
                                 + "[-use-pixmaps] [-show-fps] [-show-br] [-8bit[0|1]] [-menu<int>] [-use-loop] [-use-balls] "
                                 + "[-animation-speed<float>] [-fps<int>] "
                                 + "[-low] [-ticker-letters<int>] [-ticker-speed<float>] [-no-ticker-morph] "
                                 + "[-ticker-morph-speed<float>] [-ticker-text<string>]");
            exit(0);
        } else if (s == "-verbose") {
            // this option was already handled above
        } else{
            QMessageBox::warning(0, "QtDemo", QString("Unrecognized argument:\n") + s);
            exit(0);
        }
    }

    Colors::postConfigure();
}

void Colors::setLowSettings()
{
    Colors::openGlRendering = false;
    Colors::softwareRendering = true;
    Colors::noTicker = true;
    Colors::noTimerUpdate = true;
    Colors::fps = 30;
    Colors::usePixmaps = true;
    Colors::noAnimations = true;
    Colors::noBlending = true;
}

void Colors::detectSystemResources()
{
#ifndef QT_NO_OPENGL
    if (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0)
        Colors::glVersion = "2.0 or higher";
    else if (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5)
        Colors::glVersion = "1.5";
    else if (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_4)
        Colors::glVersion = "1.4";
    else if (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_3)
        Colors::glVersion = "1.3 or lower";
    if (Colors::verbose)
        qDebug() << "- OpenGL version:" << Colors::glVersion;

    QGLWidget glw;
    if (!QGLFormat::hasOpenGL()
        || !glw.format().directRendering()
        || !(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5)
        || glw.depth() < 24
    )
#else
    if (Colors::verbose)
        qDebug() << "- OpenGL not supported by current build of Qt";
#endif
    {
        Colors::openGlAvailable = false;
        if (Colors::verbose)
            qDebug("- OpenGL not recommended on this system");
    }

#if defined(Q_WS_X11)
    // check if X render is present:
    QPixmap tmp(1, 1);
    if (!tmp.x11PictureHandle() && tmp.paintEngine()->type() == QPaintEngine::X11){
        Colors::xRenderPresent = false;
        if (Colors::verbose)
            qDebug("- X render not present");
    }

#endif

    QWidget w;
    if (Colors::verbose)
        qDebug() << "- Color depth: " << QString::number(w.depth());
}

void Colors::postConfigure()
{
    if (!Colors::noAdapt){
        QWidget w;
        if (w.depth() < 16){
            Colors::useEightBitPalette = true;
            Colors::adapted = true;
            if (Colors::verbose)
                qDebug() << "- Adapt: Color depth less than 16 bit. Using 8 bit palette";
        }

        if (!Colors::xRenderPresent){
            Colors::setLowSettings();
            Colors::adapted = true;
            if (Colors::verbose)
                qDebug() << "- Adapt: X renderer not present. Using low settings";
        }
    }

    if (!Colors::openGlRendering && !Colors::softwareRendering){
        // The user has not decided rendering system. So we do it instead:
        if (Colors::openGlAvailable)
            Colors::openGlRendering = true;
        else
            Colors::softwareRendering = true;
    }
}


