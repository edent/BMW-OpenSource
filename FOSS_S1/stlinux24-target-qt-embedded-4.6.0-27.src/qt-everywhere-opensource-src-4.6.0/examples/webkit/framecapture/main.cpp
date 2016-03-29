/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "framecapture.h"

#include <iostream>
#include <QtGui>

int main(int argc, char * argv[])
{
    if (argc != 3) {
        std::cout << "Capture a web page and save its internal frames in different images" << std::endl << std::endl;
        std::cout << "  framecapture <url> <outputfile>" << std::endl;
        std::cout << std::endl;
        std::cout << "Notes:" << std::endl;
        std::cout << "  'url' is the URL of the web page to be captured" << std::endl;
        std::cout << "  'outputfile' is the prefix of the image files to be generated" << std::endl;
        std::cout << std::endl;
        std::cout << "Example: " << std::endl;
        std::cout << "  framecapture qt.nokia.com trolltech.png" << std::endl;
        std::cout << std::endl;
        std::cout << "Result:" << std::endl;
        std::cout << "  trolltech.png (full page)" << std::endl;
        std::cout << "  trolltech_frame1.png (...) trolltech_frameN.png ('N' number of internal frames)" << std::endl;
        return 0;
    }

    QUrl url = QWebView::guessUrlFromString(QString::fromLatin1(argv[1]));
    QString fileName = QString::fromLatin1(argv[2]);

    QApplication a(argc, argv);
    FrameCapture capture;
    QObject::connect(&capture, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
    capture.load(url, fileName);

    return a.exec();
}

