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
#include "qs60maindocument.h"
#include "qs60mainapplication_p.h"
#include "qs60mainapplication.h"
#include <bautils.h>
#include <coemain.h>

QT_BEGIN_NAMESPACE

/**
 * factory function to create the QS60Main application class
 */
CApaApplication *newS60Application()
{
    return new QS60MainApplication;
}

_LIT(KQtWrapperResourceFile, "\\resource\\apps\\s60main.rsc");

/*!
  \class QS60MainApplication
  \since 4.6
  \brief The QS60MainApplication class provides support for migration from S60.
 
  \warning This class is provided only to get access to S60 specific
  functionality in the application framework classes. It is not
  portable. We strongly recommend against using it in new applications.
 
  The QS60MainApplication provides a helper class for use in migrating
  from existing S60 based applications to Qt based applications. It is
  used in the exact same way as the \c CAknApplication class from
  Symbian, but internally provides extensions used by Qt.

  When modifying old S60 applications that rely on implementing
  functions in \c CAknApplication, the class should be modified to
  inherit from this class instead of \c CAknApplication. Then the
  application can choose to override only certain functions. To make
  Qt use the custom application objects, pass a factory function to
  \c{QApplication::QApplication(QApplication::QS60MainApplicationFactory, int &, char **)}.

  For more information on \c CAknApplication, please see the S60 documentation.

  Unlike other Qt classes, QS60MainApplication behaves like an S60 class, and can throw Symbian
  leaves.

  \sa QS60MainDocument, QS60MainAppUi, QApplication::QS60MainApplicationFactory
 */

/*!
 * \brief Contructs an instance of QS60MainApplication.
 */
QS60MainApplication::QS60MainApplication()
{
}

/*!
 * \brief Destroys the QS60MainApplication.
 */
QS60MainApplication::~QS60MainApplication()
{
}

/*!
 * \brief Creates an instance of QS60MainDocument.
 *
 * \sa QS60MainDocument
 */
CApaDocument *QS60MainApplication::CreateDocumentL()
{
    // Create an QtS60Main document, and return a pointer to it
    return new (ELeave) QS60MainDocument(*this);
}


/*!
 * \brief Returns the UID of the application.
 */
TUid QS60MainApplication::AppDllUid() const
{
    // Return the UID for the QtS60Main application
    return RProcess().SecureId().operator TUid();
}

/*!
 * \brief Returns the resource file name.
 */
TFileName QS60MainApplication::ResourceFileName() const
{
    TFindFile finder(iCoeEnv->FsSession());
    TInt err = finder.FindByDir(KQtWrapperResourceFile, KNullDesC);
    if (err == KErrNone)
        return finder.File();
    return KNullDesC();
}

QT_END_NAMESPACE
