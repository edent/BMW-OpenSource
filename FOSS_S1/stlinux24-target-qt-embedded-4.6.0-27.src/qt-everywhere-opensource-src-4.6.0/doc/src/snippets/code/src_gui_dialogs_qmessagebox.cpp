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

//! [0]
int ret = QMessageBox::warning(this, tr("My Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel,
                               QMessageBox::Save);
//! [0]


//! [1]
QMessageBox msgBox;
msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
switch (msgBox.exec()) {
case QMessageBox::Yes:
    // yes was clicked
    break;
case QMessageBox::No:
    // no was clicked
    break;
default:
    // should never be reached
    break;
}
//! [1]


//! [2]
QMessageBox msgBox;
QPushButton *connectButton = msgBox.addButton(tr("Connect"), QMessageBox::ActionRole);
QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);

msgBox.exec();

if (msgBox.clickedButton() == connectButton) {
    // connect
} else if (msgBox.clickedButton() == abortButton) {
    // abort
}
//! [2]


//! [3]
QMessageBox messageBox(this);
QAbstractButton *disconnectButton =
      messageBox.addButton(tr("Disconnect"), QMessageBox::ActionRole);
...
messageBox.exec();
if (messageBox.clickedButton() == disconnectButton) {
    ...
}
//! [3]


//! [4]
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QT_REQUIRE_VERSION(argc, argv, "4.0.2")

    QApplication app(argc, argv);
    ...
    return app.exec();
}
//! [4]

//! [5]
QMessageBox msgBox;
msgBox.setText("The document has been modified.");
msgBox.exec();
//! [5]

//! [6]
QMessageBox msgBox;
msgBox.setText("The document has been modified.");
msgBox.setInformativeText("Do you want to save your changes?");
msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
msgBox.setDefaultButton(QMessageBox::Save);
int ret = msgBox.exec();
//! [6]

//! [7]
switch (ret) {
  case QMessageBox::Save:
      // Save was clicked
      break;
  case QMessageBox::Discard:
      // Don't Save was clicked
      break;
  case QMessageBox::Cancel:
      // Cancel was clicked
      break;
  default:
      // should never be reached
      break;
}
//! [7]

//! [9]
QMessageBox msgBox(this);
msgBox.setText(tr("The document has been modified.\n"
                  "Do you want to save your changes?"));
msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel);
msgBox.setDefaultButton(QMessageBox::Save);
//! [9]
