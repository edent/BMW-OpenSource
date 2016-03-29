/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpsearchquerywidget.h"

#include <QtCore/QDebug>

#include <QtCore/QAbstractListModel>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QtGlobal>

#include <QtGui/QCompleter>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QFocusEvent>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

class QHelpSearchQueryWidgetPrivate : public QObject
{
    Q_OBJECT

private:
    struct QueryHistory {
        explicit QueryHistory() : curQuery(-1) {}
        QList<QList<QHelpSearchQuery> > queries;
        int curQuery;
    };

    class CompleterModel : public QAbstractListModel
    {
    public:
        explicit CompleterModel(QObject *parent)
          : QAbstractListModel(parent) {}

        int rowCount(const QModelIndex &parent = QModelIndex()) const
        {
            return parent.isValid() ? 0 : termList.size();
        }

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        {
            if (!index.isValid() || index.row() >= termList.count()||
                (role != Qt::EditRole && role != Qt::DisplayRole))
                return QVariant();
            return termList.at(index.row());
        }

        void addTerm(const QString &term)
        {
            if (!termList.contains(term)) {
                termList.append(term);
                reset();
            }
        }

    private:
        QStringList termList;
    };

    QHelpSearchQueryWidgetPrivate()
        : QObject(), simpleSearch(true),
          searchCompleter(new CompleterModel(this), this)
    {
        searchButton = 0;
        advancedSearchWidget = 0;
        showHideAdvancedSearchButton = 0;
        defaultQuery = 0;
        exactQuery = 0;
        similarQuery = 0;
        withoutQuery = 0;
        allQuery = 0;
        atLeastQuery = 0;
    }

    ~QHelpSearchQueryWidgetPrivate()
    {
        // nothing todo
    }

    QString escapeString(const QString &text)
    {
        QString retValue = text;
        const QString escape(QLatin1String("\\"));
        QStringList escapableCharsList;
        escapableCharsList << QLatin1String("\\") << QLatin1String("+")
            << QLatin1String("-") << QLatin1String("!") << QLatin1String("(")
            << QLatin1String(")") << QLatin1String(":") << QLatin1String("^")
            << QLatin1String("[") << QLatin1String("]") << QLatin1String("{")
            << QLatin1String("}") << QLatin1String("~");

        // make sure we won't end up with an empty string
        foreach (const QString escapeChar, escapableCharsList) {
            if (retValue.contains(escapeChar))
                retValue.replace(escapeChar, QLatin1String(""));
        }
        if (retValue.trimmed().isEmpty())
            return retValue;

        retValue = text; // now realy escape the string...
        foreach (const QString escapeChar, escapableCharsList) {
            if (retValue.contains(escapeChar))
                retValue.replace(escapeChar, escape + escapeChar);
        }
        return retValue;
    }

    QStringList buildTermList(const QString query)
    {
        bool s = false;
        QString phrase;
        QStringList wordList;
        QString searchTerm = query;

        for (int i = 0; i < searchTerm.length(); ++i) {
            if (searchTerm[i] == QLatin1Char('\"') && !s) {
                s = true;
                phrase = searchTerm[i];
                continue;
            }
            if (searchTerm[i] != QLatin1Char('\"') && s)
                phrase += searchTerm[i];
            if (searchTerm[i] == QLatin1Char('\"') && s) {
                s = false;
                phrase += searchTerm[i];
                wordList.append(phrase);
                searchTerm.remove(phrase);
            }
        }
        if (s)
            searchTerm.replace(phrase, phrase.mid(1));

        const QRegExp exp(QLatin1String("\\s+"));
        wordList += searchTerm.split(exp, QString::SkipEmptyParts);
        return wordList;
    }

    void saveQuery(const QList<QHelpSearchQuery> &query, QueryHistory &queryHist)
    {
        // We only add the query to the list if it is different from the last one.
        bool insert = false;
        if (queryHist.queries.empty())
            insert = true;
        else {
            const QList<QHelpSearchQuery> &lastQuery = queryHist.queries.last();
            if (lastQuery.size() != query.size()) {
                insert = true;
            } else {
                for (int i = 0; i < query.size(); ++i) {
                    if (query.at(i).fieldName != lastQuery.at(i).fieldName
                        || query.at(i).wordList != lastQuery.at(i).wordList) {
                        insert = true;
                        break;
                    }
                }
            }
        }
        if (insert) {
            queryHist.queries.append(query);
            foreach (const QHelpSearchQuery &queryPart, query) {
                static_cast<CompleterModel *>(searchCompleter.model())->
                        addTerm(queryPart.wordList.join(" "));
            }
        }
    }

    void nextOrPrevQuery(int maxOrMinIndex, int addend,
                         QToolButton *thisButton, QToolButton *otherButton)
    {
        QueryHistory *queryHist;
        QList<QLineEdit *> lineEdits;
        if (simpleSearch) {
            queryHist = &simpleQueries;
            lineEdits << defaultQuery;
        } else {
            queryHist = &complexQueries;
            lineEdits << allQuery << atLeastQuery << similarQuery
                    << withoutQuery << exactQuery;
        }
        foreach (QLineEdit *lineEdit, lineEdits)
            lineEdit->clear();

        // Otherwise, the respective button would be disabled.
        Q_ASSERT(queryHist->curQuery != maxOrMinIndex);

        queryHist->curQuery += addend;
        const QList<QHelpSearchQuery> &query =
                queryHist->queries.at(queryHist->curQuery);
        foreach (const QHelpSearchQuery &queryPart, query) {
            QLineEdit *lineEdit = 0;
            switch (queryPart.fieldName) {
            case QHelpSearchQuery::DEFAULT:
                lineEdit = defaultQuery;
                break;
            case QHelpSearchQuery::ALL:
                lineEdit = allQuery;
                break;
            case QHelpSearchQuery::ATLEAST:
                lineEdit = atLeastQuery;
                break;
            case QHelpSearchQuery::FUZZY:
                lineEdit = similarQuery;
                break;
            case QHelpSearchQuery::WITHOUT:
                lineEdit = withoutQuery;
                break;
            case QHelpSearchQuery::PHRASE:
                lineEdit = exactQuery;
                break;
            default:
                Q_ASSERT(0);
            }
            lineEdit->setText(queryPart.wordList.join(" "));
        }

        if (queryHist->curQuery == maxOrMinIndex)
            thisButton->setEnabled(false);
        otherButton->setEnabled(true);
    }

    void enableOrDisableToolButtons()
    {
        const QueryHistory &queryHist =
                simpleSearch ? simpleQueries : complexQueries;
        prevQueryButton->setEnabled(queryHist.curQuery > 0);
        nextQueryButton->setEnabled(queryHist.curQuery <
                                    queryHist.queries.size() - 1);
    }

private slots:
    void showHideAdvancedSearch()
    {
        if (simpleSearch) {
            advancedSearchWidget->show();
            showHideAdvancedSearchButton->setText((QLatin1String("-")));
        } else {
            advancedSearchWidget->hide();
            showHideAdvancedSearchButton->setText((QLatin1String("+")));
        }

        simpleSearch = !simpleSearch;
        defaultQuery->setEnabled(simpleSearch);
        enableOrDisableToolButtons();
    }

    void searchRequested()
    {
        QList<QHelpSearchQuery> queryList;
#if !defined(QT_CLUCENE_SUPPORT)
        queryList.append(QHelpSearchQuery(QHelpSearchQuery::DEFAULT,
                                          QStringList(defaultQuery->text())));

#else
        if (defaultQuery->isEnabled()) {
            queryList.append(QHelpSearchQuery(QHelpSearchQuery::DEFAULT,
                                              buildTermList(escapeString(defaultQuery->text()))));
        } else {
            const QRegExp exp(QLatin1String("\\s+"));
            QStringList lst = similarQuery->text().split(exp, QString::SkipEmptyParts);
            if (!lst.isEmpty()) {
                QStringList fuzzy;
                foreach (const QString term, lst)
                    fuzzy += buildTermList(escapeString(term));
                queryList.append(QHelpSearchQuery(QHelpSearchQuery::FUZZY, fuzzy));
            }

            lst = withoutQuery->text().split(exp, QString::SkipEmptyParts);
            if (!lst.isEmpty()) {
                QStringList without;
                foreach (const QString term, lst)
                    without.append(escapeString(term));
                queryList.append(QHelpSearchQuery(QHelpSearchQuery::WITHOUT, without));
            }

            if (!exactQuery->text().isEmpty()) {
                QString phrase = exactQuery->text().remove(QLatin1Char('\"'));
                phrase = escapeString(phrase.simplified());
                queryList.append(QHelpSearchQuery(QHelpSearchQuery::PHRASE, QStringList(phrase)));
            }

            lst = allQuery->text().split(exp, QString::SkipEmptyParts);
            if (!lst.isEmpty()) {
                QStringList all;
                foreach (const QString term, lst)
                    all.append(escapeString(term));
                queryList.append(QHelpSearchQuery(QHelpSearchQuery::ALL, all));
            }

            lst = atLeastQuery->text().split(exp, QString::SkipEmptyParts);
            if (!lst.isEmpty()) {
                QStringList atLeast;
                foreach (const QString term, lst)
                    atLeast += buildTermList(escapeString(term));
                queryList.append(QHelpSearchQuery(QHelpSearchQuery::ATLEAST, atLeast));
            }
        }
#endif
        QueryHistory &queryHist = simpleSearch ? simpleQueries : complexQueries;
        saveQuery(queryList, queryHist);
        queryHist.curQuery = queryHist.queries.size() - 1;
        if (queryHist.curQuery > 0)
            prevQueryButton->setEnabled(true);
        nextQueryButton->setEnabled(false);
    }

    void nextQuery()
    {
        nextOrPrevQuery((simpleSearch ? simpleQueries : complexQueries).queries.size() - 1,
                        1, nextQueryButton, prevQueryButton);
    }

    void prevQuery()
    {
        nextOrPrevQuery(0, -1, prevQueryButton, nextQueryButton);
    }

private:
    friend class QHelpSearchQueryWidget;

    bool simpleSearch;
    QPushButton *searchButton;
    QWidget* advancedSearchWidget;
    QToolButton *showHideAdvancedSearchButton;
    QLineEdit *defaultQuery;
    QLineEdit *exactQuery;
    QLineEdit *similarQuery;
    QLineEdit *withoutQuery;
    QLineEdit *allQuery;
    QLineEdit *atLeastQuery;
    QToolButton *nextQueryButton;
    QToolButton *prevQueryButton;
    QueryHistory simpleQueries;
    QueryHistory complexQueries;
    QCompleter searchCompleter;
};

#include "qhelpsearchquerywidget.moc"


/*!
    \class QHelpSearchQueryWidget
    \since 4.4
    \inmodule QtHelp
    \brief The QHelpSearchQueryWidget class provides a simple line edit or
    an advanced widget to enable the user to input a search term in a
    standardized input mask.
*/

/*!
    \fn void QHelpSearchQueryWidget::search()

    This signal is emitted when a the user has the search button invoked.
    After reciving the signal you can ask the QHelpSearchQueryWidget for the build list
    of QHelpSearchQuery's that you may pass to the QHelpSearchEngine's search() function.
*/

/*!
    Constructs a new search query widget with the given \a parent.
*/
QHelpSearchQueryWidget::QHelpSearchQueryWidget(QWidget *parent)
    : QWidget(parent)
{
    d = new QHelpSearchQueryWidgetPrivate();

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);

    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    QLabel *label = new QLabel(tr("Search for:"), this);
    d->defaultQuery = new QLineEdit(this);
    d->defaultQuery->setCompleter(&d->searchCompleter);
    d->prevQueryButton = new QToolButton(this);
    d->prevQueryButton->setArrowType(Qt::LeftArrow);
    d->prevQueryButton->setToolTip(tr("Previous search"));
    d->prevQueryButton->setEnabled(false);
    d->nextQueryButton = new QToolButton(this);
    d->nextQueryButton->setArrowType(Qt::RightArrow);
    d->nextQueryButton->setToolTip(tr("Next search"));
    d->nextQueryButton->setEnabled(false);
    d->searchButton = new QPushButton(tr("Search"), this);
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(d->defaultQuery);
    hBoxLayout->addWidget(d->prevQueryButton);
    hBoxLayout->addWidget(d->nextQueryButton);
    hBoxLayout->addWidget(d->searchButton);

    vLayout->addLayout(hBoxLayout);

    connect(d->prevQueryButton, SIGNAL(clicked()), d, SLOT(prevQuery()));
    connect(d->nextQueryButton, SIGNAL(clicked()), d, SLOT(nextQuery()));
    connect(d->searchButton, SIGNAL(clicked()), this, SIGNAL(search()));
    connect(d->defaultQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));

#if defined(QT_CLUCENE_SUPPORT)
    hBoxLayout = new QHBoxLayout();
    d->showHideAdvancedSearchButton = new QToolButton(this);
    d->showHideAdvancedSearchButton->setText(QLatin1String("+"));
    d->showHideAdvancedSearchButton->setMinimumSize(25, 20);

    label = new QLabel(tr("Advanced search"), this);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);

    QFrame* hLine = new QFrame(this);
    hLine->setFrameStyle(QFrame::HLine);
    hBoxLayout->addWidget(d->showHideAdvancedSearchButton);
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(hLine);

    vLayout->addLayout(hBoxLayout);

    // setup advanced search layout
    d->advancedSearchWidget = new QWidget(this);
    QGridLayout *gLayout = new QGridLayout(d->advancedSearchWidget);
    gLayout->setMargin(0);

    label = new QLabel(tr("words <B>similar</B> to:"), this);
    gLayout->addWidget(label, 0, 0);
    d->similarQuery = new QLineEdit(this);
    d->similarQuery->setCompleter(&d->searchCompleter);
    gLayout->addWidget(d->similarQuery, 0, 1);

    label = new QLabel(tr("<B>without</B> the words:"), this);
    gLayout->addWidget(label, 1, 0);
    d->withoutQuery = new QLineEdit(this);
    d->withoutQuery->setCompleter(&d->searchCompleter);
    gLayout->addWidget(d->withoutQuery, 1, 1);

    label = new QLabel(tr("with <B>exact phrase</B>:"), this);
    gLayout->addWidget(label, 2, 0);
    d->exactQuery = new QLineEdit(this);
    d->exactQuery->setCompleter(&d->searchCompleter);
    gLayout->addWidget(d->exactQuery, 2, 1);

    label = new QLabel(tr("with <B>all</B> of the words:"), this);
    gLayout->addWidget(label, 3, 0);
    d->allQuery = new QLineEdit(this);
    d->allQuery->setCompleter(&d->searchCompleter);
    gLayout->addWidget(d->allQuery, 3, 1);

    label = new QLabel(tr("with <B>at least one</B> of the words:"), this);
    gLayout->addWidget(label, 4, 0);
    d->atLeastQuery = new QLineEdit(this);
    d->atLeastQuery->setCompleter(&d->searchCompleter);
    gLayout->addWidget(d->atLeastQuery, 4, 1);

    vLayout->addWidget(d->advancedSearchWidget);
    d->advancedSearchWidget->hide();

    connect(d->exactQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->similarQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->withoutQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->allQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->atLeastQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->showHideAdvancedSearchButton, SIGNAL(clicked()),
        d, SLOT(showHideAdvancedSearch()));
#endif
    connect(this, SIGNAL(search()), d, SLOT(searchRequested()));
}

/*!
    Destroys the search query widget.
*/
QHelpSearchQueryWidget::~QHelpSearchQueryWidget()
{
    delete d;
}

/*!
    Returns a list of querys to use in combination with the search engines
    search(QList<QHelpSearchQuery> &query) function.
*/
QList<QHelpSearchQuery> QHelpSearchQueryWidget::query() const
{
    const QHelpSearchQueryWidgetPrivate::QueryHistory &queryHist =
        d->simpleSearch ? d->simpleQueries : d->complexQueries;
    return queryHist.queries.isEmpty() ?
        QList<QHelpSearchQuery>() : queryHist.queries.last();
}

/*! \reimp
*/
void QHelpSearchQueryWidget::focusInEvent(QFocusEvent *focusEvent)
{
    if (focusEvent->reason() != Qt::MouseFocusReason) {
        d->defaultQuery->selectAll();
        d->defaultQuery->setFocus();
    }
}

QT_END_NAMESPACE
