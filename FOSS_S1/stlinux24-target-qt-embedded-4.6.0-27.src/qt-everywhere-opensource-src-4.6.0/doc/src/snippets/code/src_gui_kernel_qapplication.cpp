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
int main(int argc, char **argv)
{
#ifdef Q_WS_X11
    bool useGUI = getenv("DISPLAY") != 0;
#else
    bool useGUI = true;
#endif
    QApplication app(argc, argv, useGUI);

    if (useGUI) {
       // start GUI version
       ...
    } else {
       // start non-GUI version
       ...
    }
    return app.exec();
}
//! [0]


//! [1]
QApplication::setStyle(new QWindowsStyle);
//! [1]


//! [2]
int main(int argc, char *argv[])
{
    QApplication::setColorSpec(QApplication::ManyColor);
    QApplication app(argc, argv);
    ...
    return app.exec();
}
//! [2]


//! [3]
QSize MyWidget::sizeHint() const
{
    return QSize(80, 25).expandedTo(QApplication::globalStrut());
}
//! [3]


//! [4]
void showAllHiddenTopLevelWidgets()
{
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->isHidden())
            widget->show();
    }
}
//! [4]


//! [5]
void updateAllWidgets()
{
    foreach (QWidget *widget, QApplication::allWidgets())
        widget->update();
}
//! [5]


//! [6]
int main(int argc, char *argv[])
{
    QApplication::setDesktopSettingsAware(false);
    QApplication app(argc, argv);
    ...
    return app.exec();
}
//! [6]


//! [7]
if ((startPos - currentPos).manhattanLength() >=
        QApplication::startDragDistance())
    startTheDrag();
//! [7]


//! [8]
void MyApplication::commitData(QSessionManager& manager)
{
    if (manager.allowsInteraction()) {
        int ret = QMessageBox::warning(
                    mainWindow,
                    tr("My Application"),
                    tr("Save changes to document?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        switch (ret) {
        case QMessageBox::Save:
            manager.release();
            if (!saveDocument())
                manager.cancel();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
        default:
            manager.cancel();
        }
    } else {
        // we did not get permission to interact, then
        // do something reasonable instead
    }
}
//! [8]


//! [9]
appname -session id
//! [9]


//! [10]
foreach (QString command, mySession.restartCommand())
    do_something(command);
//! [10]


//! [11]
foreach (QString command, mySession.discardCommand())
    do_something(command);
//! [11]


//! [12]
QWidget *widget = qApp->widgetAt(x, y);
if (widget)
    widget = widget->window();
//! [12]


//! [13]
QWidget *widget = qApp->widgetAt(point);
if (widget)
    widget = widget->window();
//! [13]
