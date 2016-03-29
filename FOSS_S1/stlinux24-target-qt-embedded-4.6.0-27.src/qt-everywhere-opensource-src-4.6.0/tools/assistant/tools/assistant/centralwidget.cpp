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

#include "centralwidget.h"
#include "helpviewer.h"
#include "searchwidget.h"
#include "mainwindow.h"
#include "preferencesdialog.h"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QTimer>

#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPrinter>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextCursor>
#include <QtGui/QPrintDialog>
#include <QtGui/QApplication>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QPrintPreviewDialog>
#include <QtGui/QPageSetupDialog>

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpSearchEngine>

QT_BEGIN_NAMESPACE

namespace {
    HelpViewer* helpViewerFromTabPosition(const QTabWidget *widget,
        const QPoint &point)
    {
        QTabBar *tabBar = qFindChild<QTabBar*>(widget);
        for (int i = 0; i < tabBar->count(); ++i) {
            if (tabBar->tabRect(i).contains(point))
                return qobject_cast<HelpViewer*>(widget->widget(i));
        }
        return 0;
    }
    CentralWidget *staticCentralWidget = 0;
}

FindWidget::FindWidget(QWidget *parent)
    : QWidget(parent)
    , appPalette(qApp->palette())
{
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    QString resourcePath = QLatin1String(":/trolltech/assistant/images/");

#ifndef Q_OS_MAC
    hboxLayout->setMargin(0);
    hboxLayout->setSpacing(6);
    resourcePath.append(QLatin1String("win"));
#else
    resourcePath.append(QLatin1String("mac"));
#endif

    toolClose = setupToolButton(QLatin1String(""),
        resourcePath + QLatin1String("/closetab.png"));
    hboxLayout->addWidget(toolClose);

    editFind = new QLineEdit(this);
    hboxLayout->addWidget(editFind);
    editFind->setMinimumSize(QSize(150, 0));
    connect(editFind, SIGNAL(textChanged(QString)), this, SLOT(updateButtons()));

    toolPrevious = setupToolButton(tr("Previous"),
        resourcePath + QLatin1String("/previous.png"));
    hboxLayout->addWidget(toolPrevious);

    toolNext = setupToolButton(tr("Next"),
        resourcePath + QLatin1String("/next.png"));
    hboxLayout->addWidget(toolNext);

    checkCase = new QCheckBox(tr("Case Sensitive"), this);
    hboxLayout->addWidget(checkCase);

    checkWholeWords = new QCheckBox(tr("Whole words"), this);
    hboxLayout->addWidget(checkWholeWords);
#if !defined(QT_NO_WEBKIT)
    checkWholeWords->hide();
#endif

    labelWrapped = new QLabel(this);
    labelWrapped->setScaledContents(true);
    labelWrapped->setTextFormat(Qt::RichText);
    labelWrapped->setMinimumSize(QSize(0, 20));
    labelWrapped->setMaximumSize(QSize(105, 20));
    labelWrapped->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    labelWrapped->setText(tr("<img src=\":/trolltech/assistant/images/wrap.png\""
        ">&nbsp;Search wrapped"));
    hboxLayout->addWidget(labelWrapped);

    QSpacerItem *spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding,
        QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);
    setMinimumWidth(minimumSizeHint().width());
    labelWrapped->hide();

    updateButtons();
}

FindWidget::~FindWidget()
{
}

void FindWidget::hideEvent(QHideEvent* event)
{
#if !defined(QT_NO_WEBKIT)
    // TODO: remove this once webkit supports setting the palette
    if (!event->spontaneous())
        qApp->setPalette(appPalette);
#else
    Q_UNUSED(event);
#endif
}

void FindWidget::showEvent(QShowEvent* event)
{
#if !defined(QT_NO_WEBKIT)
    // TODO: remove this once webkit supports setting the palette
    if (!event->spontaneous()) {
        QPalette p = appPalette;
        p.setColor(QPalette::Inactive, QPalette::Highlight,
            p.color(QPalette::Active, QPalette::Highlight));
        p.setColor(QPalette::Inactive, QPalette::HighlightedText,
            p.color(QPalette::Active, QPalette::HighlightedText));
        qApp->setPalette(p);
    }
#else
    Q_UNUSED(event);
#endif
}

void FindWidget::updateButtons()
{
    if (editFind->text().isEmpty()) {
        toolPrevious->setEnabled(false);
        toolNext->setEnabled(false);
    } else {
        toolPrevious->setEnabled(true);
        toolNext->setEnabled(true);
    }
}

QToolButton* FindWidget::setupToolButton(const QString &text, const QString &icon)
{
    QToolButton *toolButton = new QToolButton(this);

    toolButton->setText(text);
    toolButton->setAutoRaise(true);
    toolButton->setIcon(QIcon(icon));
    toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    return toolButton;
}


// --


CentralWidget::CentralWidget(QHelpEngine *engine, MainWindow *parent)
    : QWidget(parent)
    , lastTabPage(0)
    , collectionFile(engine->collectionFile())
    , findBar(0)
    , tabWidget(0)
    , findWidget(0)
    , helpEngine(engine)
    , printer(0)
    , usesDefaultCollection(parent->usesDefaultCollection())
    , m_searchWidget(0)
{
    globalActionList.clear();
    staticCentralWidget = this;
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    QString resourcePath = QLatin1String(":/trolltech/assistant/images/");

    vboxLayout->setMargin(0);
#ifndef Q_OS_MAC
    resourcePath.append(QLatin1String("win"));
#else
    resourcePath.append(QLatin1String("mac"));
#endif

    tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    connect(tabWidget, SIGNAL(currentChanged(int)), this,
        SLOT(currentPageChanged(int)));

    QToolButton *newTabButton = new QToolButton(this);
    newTabButton->setAutoRaise(true);
    newTabButton->setToolTip(tr("Add new page"));
    newTabButton->setIcon(QIcon(resourcePath + QLatin1String("/addtab.png")));

    tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    connect(newTabButton, SIGNAL(clicked()), this, SLOT(newTab()));

    QToolButton *closeTabButton = new QToolButton(this);
    closeTabButton->setEnabled(false);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setToolTip(tr("Close current page"));
    closeTabButton->setIcon(QIcon(resourcePath + QLatin1String("/closetab.png")));

    tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    connect(closeTabButton, SIGNAL(clicked()), this, SLOT(closeTab()));

    vboxLayout->addWidget(tabWidget);

    findBar = new QWidget(this);
    findWidget = new FindWidget(findBar);
    findBar->setMinimumHeight(findWidget->minimumSizeHint().height());
    findWidget->move(0, 0);
    vboxLayout->addWidget(findBar);
    findBar->hide();
    findWidget->editFind->installEventFilter(this);

    connect(findWidget->toolClose, SIGNAL(clicked()), findBar, SLOT(hide()));
    connect(findWidget->toolNext, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(findWidget->editFind, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(findWidget->editFind, SIGNAL(textChanged(QString)), this,
        SLOT(findCurrentText(QString)));
    connect(findWidget->toolPrevious, SIGNAL(clicked()), this, SLOT(findPrevious()));

    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);
    if (tabBar) {
        tabBar->installEventFilter(this);
        tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tabBar, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showTabBarContextMenu(QPoint)));
    }

#if defined(QT_NO_WEBKIT)
    QPalette p = palette();
    p.setColor(QPalette::Inactive, QPalette::Highlight,
        p.color(QPalette::Active, QPalette::Highlight));
    p.setColor(QPalette::Inactive, QPalette::HighlightedText,
        p.color(QPalette::Active, QPalette::HighlightedText));
    setPalette(p);
#endif
}

CentralWidget::~CentralWidget()
{
#ifndef QT_NO_PRINTER
    delete printer;
#endif

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    QString zoomCount;
    QString currentPages;
    QLatin1Char separator('|');
    bool searchAttached = m_searchWidget->isAttached();

    int i = searchAttached ? 1 : 0;
    for (; i < tabWidget->count(); ++i) {
        HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
        if (viewer && viewer->source().isValid()) {
            currentPages += viewer->source().toString() + separator;
            zoomCount += QString::number(viewer->zoom()) + separator;
        }
    }

    engine.setCustomValue(QLatin1String("LastTabPage"), lastTabPage);
    engine.setCustomValue(QLatin1String("LastShownPages"), currentPages);
    engine.setCustomValue(QLatin1String("SearchWasAttached"), searchAttached);
#if !defined(QT_NO_WEBKIT)
    engine.setCustomValue(QLatin1String("LastPagesZoomWebView"), zoomCount);
#else
    engine.setCustomValue(QLatin1String("LastPagesZoomTextBrowser"), zoomCount);
#endif
}

CentralWidget *CentralWidget::instance()
{
    return staticCentralWidget;
}

void CentralWidget::newTab()
{
    HelpViewer *viewer = currentHelpViewer();
#if !defined(QT_NO_WEBKIT)
    if (viewer && viewer->hasLoadFinished())
#else
    if (viewer)
#endif
        setSourceInNewTab(viewer->source());
}

void CentralWidget::zoomIn()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->zoomIn();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->zoomIn();
}

void CentralWidget::zoomOut()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->zoomOut();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->zoomOut();
}

void CentralWidget::findNext()
{
    find(findWidget->editFind->text(), true, false);
}

void CentralWidget::nextPage()
{
    int index = tabWidget->currentIndex() + 1;
    if (index >= tabWidget->count())
        index = 0;
    tabWidget->setCurrentIndex(index);
}

void CentralWidget::resetZoom()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->resetZoom();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->resetZoom();
}

void CentralWidget::previousPage()
{
    int index = tabWidget->currentIndex() -1;
    if (index < 0)
        index = tabWidget->count() -1;
    tabWidget->setCurrentIndex(index);
}

void CentralWidget::findPrevious()
{
    find(findWidget->editFind->text(), false, true);
}

void CentralWidget::closeTab()
{
    HelpViewer *viewer = currentHelpViewer();
    if (!viewer|| tabWidget->count() == 1)
        return;

    tabWidget->removeTab(tabWidget->indexOf(viewer));
    QTimer::singleShot(0, viewer, SLOT(deleteLater()));
}

void CentralWidget::setSource(const QUrl &url)
{
    HelpViewer *viewer = currentHelpViewer();
    HelpViewer *lastViewer =
        qobject_cast<HelpViewer*>(tabWidget->widget(lastTabPage));

    if (!viewer && !lastViewer) {
        viewer = new HelpViewer(helpEngine, this);
        viewer->installEventFilter(this);
        lastTabPage = tabWidget->addTab(viewer, QString());
        tabWidget->setCurrentIndex(lastTabPage);
        connectSignals();
    } else {
        viewer = lastViewer;
    }

    viewer->setSource(url);
    currentPageChanged(lastTabPage);
    viewer->setFocus(Qt::OtherFocusReason);
    tabWidget->setCurrentIndex(lastTabPage);
    tabWidget->setTabText(lastTabPage, quoteTabTitle(viewer->documentTitle()));
}

void CentralWidget::setupWidget()
{
    int option = helpEngine->customValue(QLatin1String("StartOption"),
        ShowLastPages).toInt();

    if (option != ShowLastPages) {
        QString homePage;
        if (option == ShowHomePage) {
            homePage = helpEngine->customValue(QLatin1String("defaultHomepage"),
                QLatin1String("help")).toString();
            homePage = helpEngine->customValue(QLatin1String("homepage"),
                homePage).toString();
        }
        if (option == ShowBlankPage)
            homePage = QLatin1String("about:blank");
        setSource(homePage);
    } else {
        setLastShownPages();
    }
}

void CentralWidget::setLastShownPages()
{
    const QLatin1String key("LastShownPages");
    QString value = helpEngine->customValue(key, QString()).toString();
    const QStringList lastShownPageList = value.split(QLatin1Char('|'),
        QString::SkipEmptyParts);

    const int pageCount = lastShownPageList.count();
    if (pageCount == 0) {
        if (usesDefaultCollection)
            setSource(QUrl(QLatin1String("help")));
        else
            setSource(QUrl(QLatin1String("about:blank")));
        return;
    }

#if !defined(QT_NO_WEBKIT)
    const QLatin1String zoom("LastPagesZoomWebView");
#else
    const QLatin1String zoom("LastPagesZoomTextBrowser");
#endif

    value = helpEngine->customValue(zoom, QString()).toString();
    QVector<QString> zoomVector = value.split(QLatin1Char('|'),
        QString::SkipEmptyParts).toVector();

    const int zoomCount = zoomVector.count();
    zoomVector.insert(zoomCount, pageCount - zoomCount, QLatin1String("0.0"));

    QVector<QString>::const_iterator zIt = zoomVector.constBegin();
    QStringList::const_iterator it = lastShownPageList.constBegin();
    for (; it != lastShownPageList.constEnd(); ++it, ++zIt)
        setSourceInNewTab((*it), (*zIt).toFloat());

    const QLatin1String lastTab("LastTabPage");
    int tab = helpEngine->customValue(lastTab, 1).toInt();

    const QLatin1String searchKey("SearchWasAttached");
    const bool searchIsAttached = m_searchWidget->isAttached();
    const bool searchWasAttached = helpEngine->customValue(searchKey).toBool();

    if (searchWasAttached && !searchIsAttached)
        tabWidget->setCurrentIndex(--tab);
    else if (!searchWasAttached && searchIsAttached)
        tabWidget->setCurrentIndex(++tab);
    else
        tabWidget->setCurrentIndex(tab);
}

bool CentralWidget::hasSelection() const
{
    const HelpViewer *viewer = currentHelpViewer();
    return viewer ? viewer->hasSelection() : false;
}

QUrl CentralWidget::currentSource() const
{
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->source();

    return QUrl();
}

QString CentralWidget::currentTitle() const
{
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->documentTitle();

    return QString();
}

void CentralWidget::copySelection()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->copy();
}

void CentralWidget::showTextSearch()
{
    findBar->show();
    findWidget->editFind->selectAll();
    findWidget->editFind->setFocus(Qt::ShortcutFocusReason);
}

void CentralWidget::initPrinter()
{
#ifndef QT_NO_PRINTER
    if (!printer)
        printer = new QPrinter(QPrinter::HighResolution);
#endif
}

void CentralWidget::print()
{
#ifndef QT_NO_PRINTER
    HelpViewer *viewer = currentHelpViewer();
    if (!viewer)
        return;

    initPrinter();

    QPrintDialog *dlg = new QPrintDialog(printer, this);
#if defined(QT_NO_WEBKIT)
    if (viewer->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
#endif
    dlg->addEnabledOption(QAbstractPrintDialog::PrintPageRange);
    dlg->addEnabledOption(QAbstractPrintDialog::PrintCollateCopies);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        viewer->print(printer);
    }
    delete dlg;
#endif
}

void CentralWidget::printPreview()
{
#ifndef QT_NO_PRINTER
    initPrinter();
    QPrintPreviewDialog preview(printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
        SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void CentralWidget::printPreview(QPrinter *p)
{
#ifndef QT_NO_PRINTER
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->print(p);
#endif
}

void CentralWidget::pageSetup()
{
#ifndef QT_NO_PRINTER
    initPrinter();
    QPageSetupDialog dlg(printer);
    dlg.exec();
#endif
}

bool CentralWidget::isHomeAvailable() const
{
    return currentHelpViewer() ? true : false;
}

void CentralWidget::home()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->home();
}

bool CentralWidget::isForwardAvailable() const
{
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->isForwardAvailable();

    return false;
}

void CentralWidget::forward()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->forward();
}

bool CentralWidget::isBackwardAvailable() const
{
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->isBackwardAvailable();

    return false;
}

void CentralWidget::backward()
{
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->backward();
}


QList<QAction*> CentralWidget::globalActions() const
{
    return globalActionList;
}

void CentralWidget::setGlobalActions(const QList<QAction*> &actions)
{
    globalActionList = actions;
}

void CentralWidget::setSourceInNewTab(const QUrl &url, qreal zoom)
{
    HelpViewer *viewer;

#if defined(QT_NO_WEBKIT)
    viewer = currentHelpViewer();
    if (viewer && viewer->launchedWithExternalApp(url))
        return;
#endif

    viewer = new HelpViewer(helpEngine, this);
    viewer->installEventFilter(this);
    viewer->setSource(url);
    viewer->setFocus(Qt::OtherFocusReason);
    tabWidget->setCurrentIndex(tabWidget->addTab(viewer,
        quoteTabTitle(viewer->documentTitle())));

    QFont font;
    getBrowserFontFor(viewer, &font);

#if defined(QT_NO_WEBKIT)
    font.setPointSize((int)(font.pointSize() + zoom));
    setBrowserFontFor(viewer, font);
    viewer->setZoom((int)zoom);
#else
    setBrowserFontFor(viewer, font);
    viewer->setTextSizeMultiplier(zoom == 0.0 ? 1.0 : zoom);
#endif

    connectSignals();
}

HelpViewer *CentralWidget::newEmptyTab()
{
    HelpViewer *viewer = new HelpViewer(helpEngine, this);
    viewer->installEventFilter(this);
    viewer->setFocus(Qt::OtherFocusReason);
#if defined(QT_NO_WEBKIT)
    viewer->setDocumentTitle(tr("unknown"));
#endif
    tabWidget->setCurrentIndex(tabWidget->addTab(viewer, tr("unknown")));

    connectSignals();
    return viewer;
}

void CentralWidget::findCurrentText(const QString &text)
{
    find(text, false, false);
}

void CentralWidget::connectSignals()
{
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer) {
        connect(viewer, SIGNAL(copyAvailable(bool)), this,
            SIGNAL(copyAvailable(bool)));
        connect(viewer, SIGNAL(forwardAvailable(bool)), this,
            SIGNAL(forwardAvailable(bool)));
        connect(viewer, SIGNAL(backwardAvailable(bool)), this,
            SIGNAL(backwardAvailable(bool)));
        connect(viewer, SIGNAL(sourceChanged(QUrl)), this,
            SIGNAL(sourceChanged(QUrl)));
        connect(viewer, SIGNAL(highlighted(QString)), this,
            SIGNAL(highlighted(QString)));
        connect(viewer, SIGNAL(sourceChanged(QUrl)), this,
            SLOT(setTabTitle(QUrl)));
    }
}

HelpViewer *CentralWidget::currentHelpViewer() const
{
    return qobject_cast<HelpViewer*>(tabWidget->currentWidget());
}

void CentralWidget::activateTab(bool onlyHelpViewer)
{
    if (currentHelpViewer()) {
        currentHelpViewer()->setFocus();
    } else {
        int idx = 0;
        if (onlyHelpViewer)
            idx = lastTabPage;
        tabWidget->setCurrentIndex(idx);
        tabWidget->currentWidget()->setFocus();
    }
}

void CentralWidget::setTabTitle(const QUrl &url)
{
    Q_UNUSED(url)
#if !defined(QT_NO_WEBKIT)
    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);
    for (int tab = 0; tab < tabBar->count(); ++tab) {
        HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(tab));
        if (viewer) {
            tabWidget->setTabText(tab,
                quoteTabTitle(viewer->documentTitle().trimmed()));
        }
    }
#else
    HelpViewer *viewer = currentHelpViewer();
    if (viewer) {
        tabWidget->setTabText(lastTabPage,
            quoteTabTitle(viewer->documentTitle().trimmed()));
    }
#endif
}

void CentralWidget::currentPageChanged(int index)
{
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        lastTabPage = index;

    QWidget *widget = tabWidget->cornerWidget(Qt::TopRightCorner);
    widget->setEnabled(viewer && enableTabCloseAction());

    widget = tabWidget->cornerWidget(Qt::TopLeftCorner);
    widget->setEnabled(viewer ? true : false);

    emit currentViewerChanged();
}

void CentralWidget::showTabBarContextMenu(const QPoint &point)
{
    HelpViewer *viewer = helpViewerFromTabPosition(tabWidget, point);
    if (!viewer)
        return;

    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);

    QMenu menu(QLatin1String(""), tabBar);
    QAction *newPage = menu.addAction(tr("Add New Page"));

    bool enableAction = enableTabCloseAction();
    QAction *closePage = menu.addAction(tr("Close This Page"));
    closePage->setEnabled(enableAction);

    QAction *closePages = menu.addAction(tr("Close Other Pages"));
    closePages->setEnabled(enableAction);

    menu.addSeparator();

    QAction *newBookmark = menu.addAction(tr("Add Bookmark for this Page..."));
    const QString &url = viewer->source().toString();
    if (url.isEmpty() || url == QLatin1String("about:blank"))
        newBookmark->setEnabled(false);

    QAction *pickedAction = menu.exec(tabBar->mapToGlobal(point));
    if (pickedAction == newPage)
        setSourceInNewTab(viewer->source());

    if (pickedAction == closePage) {
        tabWidget->removeTab(tabWidget->indexOf(viewer));
        QTimer::singleShot(0, viewer, SLOT(deleteLater()));
    }

    if (pickedAction == closePages) {
        int currentPage = tabWidget->indexOf(viewer);
        for (int i = tabBar->count() -1; i >= 0; --i) {
            viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
            if (i != currentPage && viewer) {
                tabWidget->removeTab(i);
                QTimer::singleShot(0, viewer, SLOT(deleteLater()));

                if (i < currentPage)
                    --currentPage;
            }
        }
    }

    if (pickedAction == newBookmark)
        emit addNewBookmark(viewer->documentTitle(), viewer->source().toString());
}

bool CentralWidget::eventFilter(QObject *object, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        switch (ke->key()) {
            default: {
                return QWidget::eventFilter(object, e);
            }   break;

            case Qt::Key_Escape: {
                if (findWidget->editFind == object) {
                    findBar->hide();
                    if (HelpViewer *viewer = currentHelpViewer())
                        viewer->setFocus();
                }
            }   break;

            case Qt::Key_Backspace: {
                HelpViewer *viewer = currentHelpViewer();
                if (viewer == object) {
#if defined(QT_NO_WEBKIT)
                    if (viewer->isBackwardAvailable()) {
#else
                    if (viewer->isBackwardAvailable() && !viewer->hasFocus()) {
#endif
                        viewer->backward();
                        return true;
                    }
                }
            }   break;
        }
    }

    if (qobject_cast<QTabBar*>(object)) {
        const bool dblClick = e->type() == QEvent::MouseButtonDblClick;
        if ((e->type() == QEvent::MouseButtonRelease) || dblClick) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
            HelpViewer *viewer = helpViewerFromTabPosition(tabWidget,
                mouseEvent->pos());
            if (viewer) {
                if ((mouseEvent->button() == Qt::MidButton) || dblClick) {
                    if (availableHelpViewer() > 1) {
                        tabWidget->removeTab(tabWidget->indexOf(viewer));
                        QTimer::singleShot(0, viewer, SLOT(deleteLater()));
                        currentPageChanged(tabWidget->currentIndex());
                        return true;
                    }
                }
            }
        }
    }

    return QWidget::eventFilter(object, e);
}

void CentralWidget::keyPressEvent(QKeyEvent *e)
{
    const QString &text = e->text();
    if (text.startsWith(QLatin1Char('/'))) {
        if (!findBar->isVisible()) {
            findBar->show();
            findWidget->editFind->clear();
        } else {
            findWidget->editFind->selectAll();
        }
        findWidget->editFind->setFocus();
        return;
    }
    QWidget::keyPressEvent(e);
}

void CentralWidget::find(const QString &ttf, bool forward, bool backward)
{
    QPalette p = findWidget->editFind->palette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);

    if (!ttf.isEmpty()) {
        HelpViewer *viewer = currentHelpViewer();

        bool found = false;
#if !defined(QT_NO_WEBKIT)
        if (viewer) {
            QWebPage::FindFlags options;
            if (backward)
                options |= QWebPage::FindBackward;

            if (findWidget->checkCase->isChecked())
                options |= QWebPage::FindCaseSensitively;

            found = viewer->findText(ttf, options);
            findWidget->labelWrapped->hide();

            if (!found) {
                options |= QWebPage::FindWrapsAroundDocument;
                found = viewer->findText(ttf, options);
                if (found)
                    findWidget->labelWrapped->show();
            }
        } else if (tabWidget->currentWidget() == m_searchWidget) {
            QTextBrowser *browser = qFindChild<QTextBrowser*>(m_searchWidget);
            found = findInTextBrowser(browser, ttf, forward, backward);
        }
#else
        QTextBrowser *browser = qobject_cast<QTextBrowser*>(viewer);
        if (tabWidget->currentWidget() == m_searchWidget)
            browser = qFindChild<QTextBrowser*>(m_searchWidget);
        found = findInTextBrowser(browser, ttf, forward, backward);
#endif

        if (!found)
            p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
    }

    if (!findWidget->isVisible())
        findWidget->show();
    findWidget->editFind->setPalette(p);
}

bool CentralWidget::findInTextBrowser(QTextBrowser* browser, const QString &ttf,
    bool forward, bool backward)
{
    if (!browser)
        return false;

    QTextDocument *doc = browser->document();
    QTextCursor cursor = browser->textCursor();

    if (!doc || cursor.isNull())
        return false;

    QTextDocument::FindFlags options;

    if (cursor.hasSelection()) {
        cursor.setPosition(forward ? cursor.position() : cursor.anchor(),
            QTextCursor::MoveAnchor);
    }

    if (backward)
        options |= QTextDocument::FindBackward;

    if (findWidget->checkCase->isChecked())
        options |= QTextDocument::FindCaseSensitively;

    if (findWidget->checkWholeWords->isChecked())
        options |= QTextDocument::FindWholeWords;

    findWidget->labelWrapped->hide();

    bool found = true;
    QTextCursor newCursor = doc->find(ttf, cursor, options);
    if (newCursor.isNull()) {
        QTextCursor ac(doc);
        ac.movePosition(options & QTextDocument::FindBackward
            ? QTextCursor::End : QTextCursor::Start);
        newCursor = doc->find(ttf, ac, options);
        if (newCursor.isNull()) {
            found = false;
            newCursor = cursor;
        } else {
            findWidget->labelWrapped->show();
        }
    }
    browser->setTextCursor(newCursor);
    return found;
}

void CentralWidget::updateBrowserFont()
{
    QFont font;
    bool searchAttached = searchWidgetAttached();
    if (searchAttached) {
        getBrowserFontFor(m_searchWidget, &font);
        setBrowserFontFor(m_searchWidget, font);
    }

    int i = searchAttached ? 1 : 0;
    getBrowserFontFor(tabWidget->widget(i), &font);
    for (i; i < tabWidget->count(); ++i)
        setBrowserFontFor(tabWidget->widget(i), font);
}

void CentralWidget::createSearchWidget(QHelpSearchEngine *searchEngine)
{
    if (m_searchWidget)
        return;

    m_searchWidget = new SearchWidget(searchEngine, this);
    connect(m_searchWidget, SIGNAL(requestShowLink(QUrl)), this,
        SLOT(setSourceFromSearch(QUrl)));
    connect(m_searchWidget, SIGNAL(requestShowLinkInNewTab(QUrl)), this,
        SLOT(setSourceFromSearchInNewTab(QUrl)));

    QFont font;
    getBrowserFontFor(m_searchWidget, &font);
    setBrowserFontFor(m_searchWidget, font);
}

void CentralWidget::activateSearchWidget(bool updateLastTabPage)
{
    if (!m_searchWidget)
        createSearchWidget(helpEngine->searchEngine());

    if (!m_searchWidget->isAttached()) {
        tabWidget->insertTab(0, m_searchWidget, tr("Search"));
        m_searchWidget->setAttached(true);

        if (updateLastTabPage)
            lastTabPage++;
    }

    tabWidget->setCurrentWidget(m_searchWidget);
    m_searchWidget->setFocus();
}

void CentralWidget::removeSearchWidget()
{
    if (searchWidgetAttached()) {
        tabWidget->removeTab(0);
        m_searchWidget->setAttached(false);
    }
}

int CentralWidget::availableHelpViewer() const
{
    int count = tabWidget->count();
    if (searchWidgetAttached())
        count--;
    return count;
}

bool CentralWidget::enableTabCloseAction() const
{
    int minTabCount = 1;
    if (searchWidgetAttached())
        minTabCount = 2;

    return (tabWidget->count() > minTabCount);
}

QString CentralWidget::quoteTabTitle(const QString &title) const
{
    QString s = title;
    return s.replace(QLatin1Char('&'), QLatin1String("&&"));
}

void
CentralWidget::setSourceFromSearch(const QUrl &url)
{
    setSource(url);
    highlightSearchTerms();
}

void
CentralWidget::setSourceFromSearchInNewTab(const QUrl &url)
{
    setSourceInNewTab(url);
    highlightSearchTerms();
}

void
CentralWidget::highlightSearchTerms()
{
#if defined(QT_NO_WEBKIT)
    HelpViewer *viewer = currentHelpViewer();
    if (!viewer)
        return;

    QHelpSearchEngine *searchEngine = helpEngine->searchEngine();
    QList<QHelpSearchQuery> queryList = searchEngine->query();

    QStringList terms;
    foreach (QHelpSearchQuery query, queryList) {
        switch (query.fieldName) {
            default: break;
            case QHelpSearchQuery::ALL: {
            case QHelpSearchQuery::PHRASE:
            case QHelpSearchQuery::DEFAULT:
            case QHelpSearchQuery::ATLEAST:
                foreach (QString term, query.wordList)
                    terms.append(term.remove(QLatin1String("\"")));
            }
        }
    }

    viewer->viewport()->setUpdatesEnabled(false);

    QTextCharFormat marker;
    marker.setForeground(Qt::red);

    QTextCursor firstHit;

    QTextCursor c = viewer->textCursor();
    c.beginEditBlock();
    foreach (const QString& term, terms) {
        c.movePosition(QTextCursor::Start);
        viewer->setTextCursor(c);

        while (viewer->find(term, QTextDocument::FindWholeWords)) {
            QTextCursor hit = viewer->textCursor();
            if (firstHit.isNull() || hit.position() < firstHit.position())
                firstHit = hit;

            hit.mergeCharFormat(marker);
        }
    }

    if (firstHit.isNull()) {
        firstHit = viewer->textCursor();
        firstHit.movePosition(QTextCursor::Start);
    }
    firstHit.clearSelection();
    c.endEditBlock();
    viewer->setTextCursor(firstHit);

    viewer->viewport()->setUpdatesEnabled(true);
#endif
}

void CentralWidget::closeTabAt(int index)
{
    HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(index));
    tabWidget->removeTab(index);
    QTimer::singleShot(0, viewer, SLOT(deleteLater()));
}

QMap<int, QString> CentralWidget::currentSourceFileList() const
{
    QMap<int, QString> sourceList;
    for (int i = 1; i < tabWidget->count(); ++i) {
        HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
        if (viewer && viewer->source().isValid())
            sourceList.insert(i, viewer->source().host());
    }
    return sourceList;
}

void CentralWidget::getBrowserFontFor(QWidget *viewer, QFont *font)
{
    const QLatin1String key("useBrowserFont");
    if (!helpEngine->customValue(key, false).toBool()) {
        *font = qApp->font();   // case for QTextBrowser and SearchWidget
#if !defined(QT_NO_WEBKIT)
        QWebView *view = qobject_cast<QWebView*> (viewer);
        if (view) {
            QWebSettings *settings = QWebSettings::globalSettings();
            *font = QFont(settings->fontFamily(QWebSettings::StandardFont),
                settings->fontSize(QWebSettings::DefaultFontSize));
        }
#endif
    } else {
        *font = qVariantValue<QFont>(helpEngine->customValue(
            QLatin1String("browserFont")));
    }
}

void CentralWidget::setBrowserFontFor(QWidget *widget, const QFont &font)
{
#if !defined(QT_NO_WEBKIT)
    QWebView *view = qobject_cast<QWebView*> (widget);
    if (view) {
        QWebSettings *settings = view->settings();
        settings->setFontFamily(QWebSettings::StandardFont, font.family());
        settings->setFontSize(QWebSettings::DefaultFontSize, font.pointSize());
    } else if (widget && widget->font() != font) {
        widget->setFont(font);
    }
#else
    if (widget && widget->font() != font)
        widget->setFont(font);
#endif
}

QT_END_NAMESPACE
