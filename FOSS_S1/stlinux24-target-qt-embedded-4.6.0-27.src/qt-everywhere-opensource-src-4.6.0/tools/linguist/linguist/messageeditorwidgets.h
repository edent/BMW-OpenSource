/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef MESSAGEEDITORWIDGETS_H
#define MESSAGEEDITORWIDGETS_H

#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QMap>
#include <QTextEdit>
#include <QUrl>
#include <QWidget>

QT_BEGIN_NAMESPACE

class QAbstractButton;
class QAction;
class QContextMenuEvent;
class QKeyEvent;
class QMenu;
class QSizeF;
class QString;
class QVariant;

class MessageHighlighter;

/*
  Automatically adapt height to document contents
 */
class ExpandingTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    ExpandingTextEdit(QWidget *parent = 0);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private slots:
    void updateHeight(const QSizeF &documentSize);
    void reallyEnsureCursorVisible();

private:
    int m_minimumHeight;
};

/*
  Format markup & control characters
*/
class FormatTextEdit : public ExpandingTextEdit
{
    Q_OBJECT
public:
    FormatTextEdit(QWidget *parent = 0);
    void setEditable(bool editable);

public slots:
    void setPlainText(const QString & text, bool userAction);

private:
    MessageHighlighter *m_highlighter;
};

/*
  Displays text field & associated label
*/
class FormWidget : public QWidget
{
    Q_OBJECT
public:
    FormWidget(const QString &label, bool isEditable, QWidget *parent = 0);
    void setLabel(const QString &label) { m_label->setText(label); }
    void setTranslation(const QString &text, bool userAction = false);
    void clearTranslation() { setTranslation(QString(), false); }
    QString getTranslation() { return m_editor->toPlainText(); }
    void setEditingEnabled(bool enable);
    void setHideWhenEmpty(bool optional) { m_hideWhenEmpty = optional; }
    FormatTextEdit *getEditor() { return m_editor; }

signals:
    void textChanged(QTextEdit *);
    void selectionChanged(QTextEdit *);
    void cursorPositionChanged();

private slots:
    void slotSelectionChanged();
    void slotTextChanged();

private:
    QLabel *m_label;
    FormatTextEdit *m_editor;
    bool m_hideWhenEmpty;
};

/*
  Displays text fields & associated label
*/
class FormMultiWidget : public QWidget
{
    Q_OBJECT
public:
    FormMultiWidget(const QString &label, QWidget *parent = 0);
    void setLabel(const QString &label) { m_label->setText(label); }
    void setTranslation(const QString &text, bool userAction = false);
    void clearTranslation() { setTranslation(QString(), false); }
    QString getTranslation() const;
    void setEditingEnabled(bool enable);
    void setMultiEnabled(bool enable);
    void setHideWhenEmpty(bool optional) { m_hideWhenEmpty = optional; }
    const QList<FormatTextEdit *> &getEditors() const { return m_editors; }

signals:
    void editorCreated(QTextEdit *);
    void textChanged(QTextEdit *);
    void selectionChanged(QTextEdit *);
    void cursorPositionChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void slotTextChanged();
    void slotSelectionChanged();
    void minusButtonClicked();
    void plusButtonClicked();

private:
    void addEditor(int idx);
    void updateLayout();
    QAbstractButton *makeButton(const QIcon &icon, const char *slot);
    void insertEditor(int idx);
    void deleteEditor(int idx);

    QLabel *m_label;
    QList<FormatTextEdit *> m_editors;
    QList<QWidget *> m_plusButtons;
    QList<QAbstractButton *> m_minusButtons;
    bool m_hideWhenEmpty;
    bool m_multiEnabled;
    QIcon m_plusIcon, m_minusIcon;
};

QT_END_NAMESPACE

#endif // MESSAGEEDITORWIDGETS_H
