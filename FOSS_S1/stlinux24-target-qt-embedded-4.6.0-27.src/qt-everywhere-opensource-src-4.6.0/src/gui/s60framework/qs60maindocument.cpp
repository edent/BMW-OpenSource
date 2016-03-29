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

#include "qs60mainappui.h"
#include "qs60maindocument.h"

#include <exception>

QT_BEGIN_NAMESPACE

/*!
  \class QS60MainDocument
  \since 4.6
  \brief The QS60MainDocument class is a helper class for S60 migration.

  \warning This class is provided only to get access to S60 specific
  functionality in the application framework classes. It is not
  portable. We strongly recommend against using it in new applications.

  The QS60MainDocument provides a helper class for use in migrating
  from existing S60 based applications to Qt based applications. It is
  used in the exact same way as the \c CAknDocument class from
  Symbian, but internally provides extensions used by Qt.

  When modifying old S60 applications that rely on implementing
  functions in \c CAknDocument, the class should be modified to
  inherit from this class instead of \c CAknDocument. Then the
  application can choose to override only certain functions.

  For more information on \c CAknDocument, please see the S60
  documentation.

  Unlike other Qt classes, QS60MainDocument behaves like an S60 class,
  and can throw Symbian leaves.

  \sa QS60MainApplication, QS60MainAppUi
 */

/*!
 * \brief Constructs an instance of QS60MainDocument.
 *
 * \a mainApplication should contain a pointer to a QS60MainApplication instance.
 */
QS60MainDocument::QS60MainDocument(CEikApplication& mainApplication)
    : CAknDocument(mainApplication)
{
    // No implementation required
}

/*!
 * \brief Destroys the QS60MainDocument.
 */
QS60MainDocument::~QS60MainDocument()
{
    // No implementation required
}

/*!
 * \brief Creates an instance of QS60MainAppUi.
 *
 * \sa QS60MainAppUi
 */
CEikAppUi *QS60MainDocument::CreateAppUiL()
{
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return (static_cast <CEikAppUi*>(new(ELeave)QS60MainAppUi));
}

QT_END_NAMESPACE
