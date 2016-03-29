/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui/QtGui>
#include <QtGui/qmacnativewidget_mac.h>
#ifdef QT_MAC_USE_COCOA
#import <Cocoa/Cocoa.h>
#else
#include <Carbon/Carbon.h>
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
#ifdef QT_MAC_USE_COCOA
//![0]
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(200, app.desktop()->height() - 200, 239, 200)
                        styleMask:NSTitledWindowMask | NSClosableWindowMask
                                  | NSMiniaturizableWindowMask | NSResizableWindowMask
                        backing:NSBackingStoreBuffered defer:NO];

    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->move(0, 0);
    nativeWidget->setPalette(QPalette(Qt::red));
    nativeWidget->setAutoFillBackground(true);
    QVBoxLayout *layout = new QVBoxLayout();
    QPushButton *pushButton = new QPushButton("An Embedded Qt Button!", nativeWidget);
    pushButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // Don't use the layout rect calculated from QMacStyle.
    layout->addWidget(pushButton);
    nativeWidget->setLayout(layout);

    // Adjust Cocoa layouts
    NSView *nativeWidgetView = reinterpret_cast<NSView *>(nativeWidget->winId());
    NSView *contentView = [window contentView];
    [contentView setAutoresizesSubviews:YES];
    [nativeWidgetView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [nativeWidgetView setAutoresizesSubviews:YES];
    NSView *pushButtonView = reinterpret_cast<NSView *>(pushButton->winId());
    [pushButtonView setAutoresizingMask:NSViewWidthSizable];

    // Add the nativeWidget to the window.
    [contentView addSubview:nativeWidgetView positioned:NSWindowAbove relativeTo:nil];
    nativeWidget->show();
    pushButton->show();

    // Show the window.
    [window makeKeyAndOrderFront:window];
    [pool release];
//![0]
#else
//![1]
    Rect contentRect;
    SetRect(&contentRect, 200, 200, 400, 400);
    HIWindowRef windowRef;
    CreateNewWindow(kDocumentWindowClass, kWindowStandardDocumentAttributes | kWindowCompositingAttribute | kWindowStandardHandlerAttribute | kWindowLiveResizeAttribute, &contentRect, &windowRef);
    HIViewRef contentView = 0;
    GetRootControl(windowRef, &contentView);

    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->move(0, 0);
    nativeWidget->setPalette(QPalette(Qt::red));
    nativeWidget->setAutoFillBackground(true);
    QVBoxLayout *layout = new QVBoxLayout();
    QPushButton *pushButton = new QPushButton("An Embedded Qt Button!", nativeWidget);
    pushButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // Don't use the layout rect calculated from QMacStyle.
    layout->addWidget(pushButton);
    nativeWidget->setLayout(layout);
    HIViewRef nativeWidgetView = reinterpret_cast<HIViewRef>(nativeWidget->winId());
    // Add the nativeWidget to the window.
    HIViewAddSubview(contentView, nativeWidgetView);

    // Adjust Carbon layouts
    HILayoutInfo layoutInfo;
    layoutInfo.version = kHILayoutInfoVersionZero;
    HIViewGetLayoutInfo(nativeWidgetView, &layoutInfo);

    layoutInfo.binding.top.toView = contentView;
    layoutInfo.binding.top.kind = kHILayoutBindTop;
    layoutInfo.binding.left.toView = contentView;
    layoutInfo.binding.left.kind = kHILayoutBindLeft;
    layoutInfo.binding.right.toView = contentView;
    layoutInfo.binding.right.kind = kHILayoutBindRight;
    layoutInfo.binding.bottom.toView = contentView;
    layoutInfo.binding.bottom.kind = kHILayoutBindBottom;

    HIViewSetLayoutInfo(nativeWidgetView, &layoutInfo);
    HIViewApplyLayout(nativeWidgetView);

    pushButton->show();
    nativeWidget->show();
    // Show the window.
    ShowWindow(windowRef);
//![1]
#endif
    return app.exec(); // gives us the same behavior in both
}
