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

#include "qmacdefines_mac.h"
#ifdef QT_MAC_USE_COCOA
#include <qaction.h>
#include <qcoreapplication.h>
#include <private/qcocoamenuloader_mac_p.h>
#include <private/qapplication_p.h>
#include <private/qt_mac_p.h>
#include <qmenubar.h>

QT_FORWARD_DECLARE_CLASS(QCFString)
QT_FORWARD_DECLARE_CLASS(QString)

QT_USE_NAMESPACE

@implementation QT_MANGLE_NAMESPACE(QCocoaMenuLoader)

- (void)awakeFromNib
{
    // Get the names in the nib to match the app name set by Qt.
    NSString *appName = reinterpret_cast<const NSString*>(QCFString::toCFStringRef(qAppName()));
    [quitItem setTitle:[[quitItem title] stringByReplacingOccurrencesOfString:@"NewApplication"
                                                                   withString:appName]];
    [hideItem setTitle:[[hideItem title] stringByReplacingOccurrencesOfString:@"NewApplication"
                                                                   withString:appName]];
    [aboutItem setTitle:[[aboutItem title] stringByReplacingOccurrencesOfString:@"NewApplication"
                                                                   withString:appName]];
    [appName release];
    // Disable the items that don't do anything. If someone associates a QAction with them
    // They should get synced back in.
    [preferencesItem setEnabled:NO];
    [preferencesItem setHidden:YES];
    [aboutItem setEnabled:NO];
    [aboutItem setHidden:YES];
}

- (void)ensureAppMenuInMenu:(NSMenu *)menu
{
    NSMenu *mainMenu = [NSApp mainMenu];
    if ([NSApp mainMenu] == menu)
        return; // nothing to do!
#ifndef QT_NAMESPACE
    Q_ASSERT(mainMenu);
#endif
    // Grab the app menu out of the current menu.
    int numItems = [mainMenu numberOfItems];
    NSMenuItem *oldAppMenuItem = 0;
    for (int i = 0; i < numItems; ++i) {
        NSMenuItem *item = [mainMenu itemAtIndex:i];
        if ([item submenu] == appMenu) {
            oldAppMenuItem = item;
            [oldAppMenuItem retain];
            [mainMenu removeItemAtIndex:i];
            break;
        }
    }

    if (oldAppMenuItem) {
        [oldAppMenuItem setSubmenu:nil];
        [oldAppMenuItem release];
        NSMenuItem *appMenuItem = [[NSMenuItem alloc] initWithTitle:@"Apple"
            action:nil keyEquivalent:@""];
        [appMenuItem setSubmenu:appMenu];
        [menu insertItem:appMenuItem atIndex:0];
    }
}

- (void)dealloc
{
    [lastAppSpecificItem release];
    [theMenu release];
    [appMenu release];
    [super dealloc];
}

- (NSMenu *)menu
{
    return [[theMenu retain] autorelease];
}

- (NSMenu *)applicationMenu
{
    return [[appMenu retain] autorelease];
}

- (NSMenuItem *)quitMenuItem
{
    return [[quitItem retain] autorelease];
}

- (NSMenuItem *)preferencesMenuItem
{
    return [[preferencesItem retain] autorelease];
}

- (NSMenuItem *)aboutMenuItem
{
    return [[aboutItem retain] autorelease];
}

- (NSMenuItem *)aboutQtMenuItem
{
    return [[aboutQtItem retain] autorelease];
}

- (NSMenuItem *)hideMenuItem;
{
    return [[hideItem retain] autorelease];
}

- (NSMenuItem *)appSpecificMenuItem;
{
    // Create an App-Specific menu item, insert it into the menu and return
    // it as an autorelease item.
    NSMenuItem *item = [[NSMenuItem alloc] init];

    NSInteger location;
    if (lastAppSpecificItem == nil) {
        location = [appMenu indexOfItem:aboutQtItem];
    } else {
        location = [appMenu indexOfItem:lastAppSpecificItem];
        [lastAppSpecificItem release];
    }
    lastAppSpecificItem = item;  // Keep track of this for later (i.e., don't release it)
    [appMenu insertItem:item atIndex:location + 1];

    return [[item retain] autorelease];
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void)terminate:(id)sender
{
    [NSApp terminate:sender];
}

- (void)orderFrontStandardAboutPanel:(id)sender
{
    [NSApp orderFrontStandardAboutPanel:sender];
}

- (void)hideOtherApplications:(id)sender
{
    [NSApp hideOtherApplications:sender];
}

- (void)unhideAllApplications:(id)sender
{
    [NSApp unhideAllApplications:sender];
}

- (void)hide:(id)sender
{
    [NSApp hide:sender];
}

- (IBAction)qtDispatcherToQAction:(id)sender
{
    QScopedLoopLevelCounter loopLevelCounter(QApplicationPrivate::instance()->threadData);
    NSMenuItem *item = static_cast<NSMenuItem *>(sender);
    if (QAction *action = reinterpret_cast<QAction *>([item tag])) {
        action->trigger();
    } else if (item == quitItem) {
        // We got here because someone was once the quitItem, but it has been
        // abandoned (e.g., the menubar was deleted). In the meantime, just do
        // normal QApplication::quit().
        qApp->quit();
    }
}
@end
#endif // QT_MAC_USE_COCOA
