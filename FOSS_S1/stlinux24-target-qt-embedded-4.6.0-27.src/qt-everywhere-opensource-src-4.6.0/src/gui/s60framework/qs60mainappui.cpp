/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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

// INCLUDE FILES
#include <exception>
#include <avkon.hrh>
#include <eikmenub.h>
#include <eikmenup.h>
#include <barsread.h>
#include <s60main.rsg>
#include <avkon.rsg>

#include "qs60mainappui.h"
#include <QtGui/qapplication.h>
#include <QtGui/qsymbianevent.h>
#include <QtGui/qmenu.h>
#include <private/qmenu_p.h>
#include <private/qt_s60_p.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class QS60MainAppUi
  \since 4.6
  \brief The QS60MainAppUi class is a helper class for S60 migration.

  \warning This class is provided only to get access to S60 specific
  functionality in the application framework classes. It is not
  portable. We strongly recommend against using it in new applications.

  The QS60MainAppUi provides a helper class for use in migrating from
  existing S60 based applications to Qt based applications. It is used
  in the exact same way as the \c CAknAppUi class from Symbian, but
  internally provides extensions used by Qt.

  When modifying old S60 applications that rely on implementing
  functions in \c CAknAppUi, the class should be modified to inherit
  from this class instead of \c CAknAppUi. Then the application can
  choose to override only certain functions.

  For more information on \c CAknAppUi, please see the S60
  documentation.

  Unlike other Qt classes, QS60MainAppUi behaves like an S60 class,
  and can throw Symbian leaves.

  \sa QS60MainDocument, QS60MainApplication
 */

/*!
 * \brief Second phase Symbian constructor.
 *
 * Constructs all the elements of the class that can cause a leave to happen.
 *
 * If you override this function, you should call the base class implementation as well.
 */
void QS60MainAppUi::ConstructL()
{
    // Cone's heap and handle checks on app destruction are not suitable for Qt apps, as many
    // objects can still exist in static data at that point. Instead we will print relevant information
    // so that comparative checks may be made for memory leaks, using ~SPrintExitInfo in corelib.
    iEikonEnv->DisableExitChecks(ETrue);

    // Initialise app UI with standard value.
    // ENoAppResourceFile and ENonStandardResourceFile makes UI to work without
    // resource files in most SDKs. S60 3rd FP1 public seems to require resource file
    // even these flags are defined
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    CEikButtonGroupContainer* nativeContainer = Cba();
    nativeContainer->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY_WITH_IDS);
}

/*!
 * \brief Contructs an instance of QS60MainAppUi.
 */
QS60MainAppUi::QS60MainAppUi()
{
    // No implementation required
}

/*!
 * \brief Destroys the QS60MainAppUi.
 */
QS60MainAppUi::~QS60MainAppUi()
{
}

/*!
 * \brief Handles commands produced by the S60 framework.
 *
 * \a command holds the ID of the command to handle, and is S60 specific.
 *
 * If you override this function, you should call the base class implementation if you do not
 * handle the command.
 */
void QS60MainAppUi::HandleCommandL(TInt command)
{
    if (qApp) {
        QSymbianEvent event(QSymbianEvent::CommandEvent, command);
        QT_TRYCATCH_LEAVING(qApp->symbianProcessEvent(&event));
    }
}

/*!
 * \brief Handles a resource change in the S60 framework.
 *
 * Resource changes include layout switches. \a type holds the type of resource change that
 * occurred.
 *
 * If you override this function, you should call the base class implementation if you do not
 * handle the resource change.
 */
void QS60MainAppUi::HandleResourceChangeL(TInt type)
{
    CAknAppUi::HandleResourceChangeL(type);

    if (qApp) {
        QSymbianEvent event(QSymbianEvent::ResourceChangeEvent, type);
        QT_TRYCATCH_LEAVING(qApp->symbianProcessEvent(&event));
    }
}

/*!
 * \brief Handles raw window server events.
 *
 * The event type and information is passed in \a wsEvent, while the receiving control is passed in
 * \a destination.
 *
 * If you override this function, you should call the base class implementation if you do not
 * handle the event.
 */
void QS60MainAppUi::HandleWsEventL(const TWsEvent& wsEvent, CCoeControl *destination)
{
    int result = 0;
    if (qApp) {
        QSymbianEvent event(&wsEvent);
        QT_TRYCATCH_LEAVING(
            result = qApp->symbianProcessEvent(&event)
        );
    }

    if (result <= 0)
        CAknAppUi::HandleWsEventL(wsEvent, destination);
}


/*!
 * \brief Handles changes to the status pane size.
 *
 * Called by the framework when the application status pane size is changed.
 *
 * If you override this function, you should call the base class implementation if you do not
 * handle the size change.
 */
void QS60MainAppUi::HandleStatusPaneSizeChange()
{
    TRAP_IGNORE(HandleResourceChangeL(KInternalStatusPaneChange));
    HandleStackedControlsResourceChange(KInternalStatusPaneChange);
}

/*!
 * \brief Dynamically initializes a menu bar.
 *
 * The resource associated with the menu is given in \a resourceId, and the actual menu bar is
 * passed in \a menuBar.
 *
 * If you override this function, you should call the base class implementation as well.
 */
void QS60MainAppUi::DynInitMenuBarL(TInt /* resourceId */, CEikMenuBar * /* menuBar */)
{
}

/*!
 * \brief Dynamically initializes a menu pane.
 *
 * The resource associated with the menu is given in \a resourceId, and the actual menu pane is
 * passed in \a menuPane.
 *
 * If you override this function, you should call the base class implementation as well.
 */
void QS60MainAppUi::DynInitMenuPaneL(TInt resourceId, CEikMenuPane *menuPane)
{
    if (resourceId == R_QT_WRAPPERAPP_MENU) {
        if (menuPane->NumberOfItemsInPane() <= 1)
            QT_TRYCATCH_LEAVING(qt_symbian_show_toplevel(menuPane));

    } else if (resourceId != R_AVKON_MENUPANE_FEP_DEFAULT
            && resourceId != R_AVKON_MENUPANE_EDITTEXT_DEFAULT
            && resourceId != R_AVKON_MENUPANE_LANGUAGE_DEFAULT) {
        QT_TRYCATCH_LEAVING(qt_symbian_show_submenu(menuPane, resourceId));
    }
}

/*!
 * \brief Restores a menu window.
 *
 * The menu window to restore is given in \a menuWindow. The resource ID and type of menu is given
 * in \a resourceId and \a menuType, respectively.
 *
 * If you override this function, you should call the base class implementation as well.
 */
void QS60MainAppUi::RestoreMenuL(CCoeControl* menuWindow, TInt resourceId, TMenuType menuType)
{
    if (resourceId >= QT_SYMBIAN_FIRST_MENU_ITEM && resourceId <= QT_SYMBIAN_LAST_MENU_ITEM) {
        if (menuType == EMenuPane)
            DynInitMenuPaneL(resourceId, (CEikMenuPane*)menuWindow);
        else
            DynInitMenuBarL(resourceId, (CEikMenuBar*)menuWindow);
    } else {
        CAknAppUi::RestoreMenuL(menuWindow, resourceId, menuType);
    }
}

QT_END_NAMESPACE
