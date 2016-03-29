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

#include <QtGui>

typedef QDialog WordCountDialog;
typedef QDialog FindDialog;

#define this 0
#define setWordCount(x) isVisible()

QString tr(const char *text)
{
    return QApplication::translate(text, text);
}

class EditorWindow : public QMainWindow
{
public:
    void find();
    void countWords();

private:
    FindDialog *findDialog;
};

//! [0]
void EditorWindow::find()
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext()), this, SLOT(findNext()));
    }

    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}
//! [0]

//! [1]
void EditorWindow::countWords()
{
    WordCountDialog dialog(this);
    dialog.setWordCount(document().wordCount());
    dialog.exec();
}
//! [1]

inline bool boo()
{
    QMessageBox::information(this, "Application name",
                             "Unable to find the user preferences file.\n"
                             "The factory default will be used instead.");

    QString filename;
    if (QFile::exists(filename) &&
        QMessageBox::question(
            this,
            tr("Overwrite File? -- Application Name"),
            tr("A file called %1 already exists."
               "Do you want to overwrite it?")
            .arg(filename),
            tr("&Yes"), tr("&No"),
            QString(), 0, 1))
        return false;

    switch(QMessageBox::warning(this, "Application name",
                                "Could not connect to the <mumble> server.\n"
                                "This program can't function correctly "
                                "without the server.\n\n",
                                "Retry",
                                "Quit", 0, 0, 1)) {
    case 0: // The user clicked the Retry again button or pressed Enter
        // try again
        break;
    case 1: // The user clicked the Quit or pressed Escape
        // exit
        break;
    }

    switch(QMessageBox::information(this, "Application name here",
                                    "The document contains unsaved changes\n"
                                    "Do you want to save the changes before exiting?",
                                    "&Save", "&Discard", "Cancel",
                                    0,      // Enter == button 0
                                    2)) { // Escape == button 2
    case 0: // Save clicked or Alt+S pressed or Enter pressed.
        // save
        break;
    case 1: // Discard clicked or Alt+D pressed
        // don't save but exit
        break;
    case 2: // Cancel clicked or Escape pressed
        // don't exit
        break;
    }

    switch(QMessageBox::warning(this, "Application name here",
                                "Could not save the user preferences,\n"
                                "because the disk is full. You can delete\n"
                                "some files and press Retry, or you can\n"
                                "abort the Save Preferences operation.",
                                QMessageBox::Retry | QMessageBox::Default,
                                QMessageBox::Abort | QMessageBox::Escape)) {
    case QMessageBox::Retry: // Retry clicked or Enter pressed
        // try again
        break;
    case QMessageBox::Abort: // Abort clicked or Escape pressed
        // abort
        break;
    }

    QString errorDetails;
    QMessageBox::critical(0, "Application name here",
                          QString("An internal error occurred. Please ") +
                          "call technical support at 1234-56789 and report\n"+
                          "these numbers:\n\n" + errorDetails +
                          "\n\nApplication will now exit.");

    QMessageBox::about(this, "About <Application>",
                       "<Application> is a <one-paragraph blurb>\n\n"
                       "Copyright 1991-2003 Such-and-such. "
                       "<License words here.>\n\n"
                       "For technical support, call 1234-56789 or see\n"
                       "http://www.such-and-such.com/Application/\n");

    {
        // saving the file
        QMessageBox mb("Application name here",
                       "Saving the file will overwrite the original file on the disk.\n"
                       "Do you really want to save?",
                       QMessageBox::Information,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No,
                       QMessageBox::Cancel | QMessageBox::Escape);
        mb.setButtonText(QMessageBox::Yes, "Save");
        mb.setButtonText(QMessageBox::No, "Discard");
        switch(mb.exec()) {
        case QMessageBox::Yes:
            // save and exit
            break;
        case QMessageBox::No:
            // exit without saving
            break;
        case QMessageBox::Cancel:
            // don't save and don't exit
            break;
        }
    }

    {
        // hardware failure
//! [2]
        QMessageBox mb("Application Name",
                       "Hardware failure.\n\nDisk error detected\nDo you want to stop?",
                       QMessageBox::Question,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No | QMessageBox::Escape,
                       QMessageBox::NoButton);
        if (mb.exec() == QMessageBox::No) {
            // try again
//! [2]
        }
    }
}

inline void moo()
{
    int numFiles;
//! [3]
    QProgressDialog progress("Copying files...", "Abort Copy", 0, numFiles, this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < numFiles; i++) {
        progress.setValue(i);

        if (progress.wasCanceled())
            break;
        //... copy one file
    }
    progress.setValue(numFiles);
//! [3]
}

class Operation : public QObject
{
public:
    Operation(QObject *parent);
    void perform();
    void cancel();

private:
    int steps;
    QProgressDialog *pd;
    QTimer *t;
};

//! [4]
// Operation constructor
Operation::Operation(QObject *parent)
    : QObject(parent), steps(0)
{
    pd = new QProgressDialog("Operation in progress.", "Cancel", 0, 100);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(perform()));
    t->start(0);
}
//! [4] //! [5]

void Operation::perform()
{
    pd->setValue(steps);
    //... perform one percent of the operation
    steps++;
    if (steps > pd->maximum())
        t->stop();
}
//! [5] //! [6]

void Operation::cancel()
{
    t->stop();
    //... cleanup
}
//! [6]

int main()
{
}
