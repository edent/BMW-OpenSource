/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
#include "addressview.h"
#include "msoutl.h"
#include <QtGui>

class AddressBookModel : public QAbstractListModel
{
public:
    AddressBookModel(AddressView *parent);
    ~AddressBookModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    void changeItem(const QModelIndex &index, const QString &firstName, const QString &lastName, const QString &address, const QString &email);
    void addItem(const QString &firstName, const QString &lastName, const QString &address, const QString &email);
    void update();

private:
    Outlook::Application outlook;
    Outlook::Items * contactItems;

    mutable QHash<QModelIndex, QStringList> cache;
};
//! [0] //! [1]

AddressBookModel::AddressBookModel(AddressView *parent)
: QAbstractListModel(parent)
{
    if (!outlook.isNull()) {
        Outlook::NameSpace session(outlook.Session());
        session.Logon();
        Outlook::MAPIFolder *folder = session.GetDefaultFolder(Outlook::olFolderContacts);
        contactItems = new Outlook::Items(folder->Items());
	connect(contactItems, SIGNAL(ItemAdd(IDispatch*)), parent, SLOT(updateOutlook()));
	connect(contactItems, SIGNAL(ItemChange(IDispatch*)), parent, SLOT(updateOutlook()));
	connect(contactItems, SIGNAL(ItemRemove()), parent, SLOT(updateOutlook()));    

        delete folder;
    }
}

//! [1] //! [2]
AddressBookModel::~AddressBookModel()
{
    delete contactItems;

    if (!outlook.isNull())
        Outlook::NameSpace(outlook.Session()).Logoff();
}

//! [2] //! [3]
int AddressBookModel::rowCount(const QModelIndex &) const
{
    return contactItems ? contactItems->Count() : 0;
}

int AddressBookModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

//! [3] //! [4]
QVariant AddressBookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case 0:
        return tr("First Name");
    case 1:
        return tr("Last Name");
    case 2:
        return tr("Address");
    case 3:
        return tr("Email");
    default:
        break;
    }

    return QVariant();
}

//! [4] //! [5]
QVariant AddressBookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    QStringList data;
    if (cache.contains(index)) {
        data = cache.value(index);
    } else {
        Outlook::ContactItem contact(contactItems->Item(index.row() + 1));
        data << contact.FirstName() << contact.LastName() << contact.HomeAddress() << contact.Email1Address();
        cache.insert(index, data);
    }

    if (index.column() < data.count())
        return data.at(index.column());

    return QVariant();
}

//! [5] //! [6]
void AddressBookModel::changeItem(const QModelIndex &index, const QString &firstName, const QString &lastName, const QString &address, const QString &email)
{
    Outlook::ContactItem item(contactItems->Item(index.row() + 1));

    item.SetFirstName(firstName);
    item.SetLastName(lastName);
    item.SetHomeAddress(address);
    item.SetEmail1Address(email);

    item.Save();

    cache.take(index);
}

//! [6] //! [7]
void AddressBookModel::addItem(const QString &firstName, const QString &lastName, const QString &address, const QString &email)
{
    Outlook::ContactItem item(outlook.CreateItem(Outlook::olContactItem));
    if (!item.isNull()) {
        item.SetFirstName(firstName);
        item.SetLastName(lastName);
        item.SetHomeAddress(address);
        item.SetEmail1Address(email);

        item.Save();
    }
}

//! [7] //! [8]
void AddressBookModel::update()
{
    cache.clear();

    emit reset();
}


//! [8] //! [9]
AddressView::AddressView(QWidget *parent)
: QWidget(parent)
{
    QGridLayout *mainGrid = new QGridLayout(this);

    QLabel *liFirstName = new QLabel("First &Name", this);
    liFirstName->resize(liFirstName->sizeHint());
    mainGrid->addWidget(liFirstName, 0, 0);

    QLabel *liLastName = new QLabel("&Last Name", this);
    liLastName->resize(liLastName->sizeHint());
    mainGrid->addWidget(liLastName, 0, 1);

    QLabel *liAddress = new QLabel("Add&ress", this);
    liAddress->resize(liAddress->sizeHint());
    mainGrid->addWidget(liAddress, 0, 2);

    QLabel *liEMail = new QLabel("&E-Mail", this);
    liEMail->resize(liEMail->sizeHint());
    mainGrid->addWidget(liEMail, 0, 3);

    add = new QPushButton("A&dd", this);
    add->resize(add->sizeHint());
    mainGrid->addWidget(add, 0, 4);
    connect(add, SIGNAL(clicked()), this, SLOT(addEntry()));

    iFirstName = new QLineEdit(this);
    iFirstName->resize(iFirstName->sizeHint());
    mainGrid->addWidget(iFirstName, 1, 0);
    liFirstName->setBuddy(iFirstName);

    iLastName = new QLineEdit(this);
    iLastName->resize(iLastName->sizeHint());
    mainGrid->addWidget(iLastName, 1, 1);
    liLastName->setBuddy(iLastName);

    iAddress = new QLineEdit(this);
    iAddress->resize(iAddress->sizeHint());
    mainGrid->addWidget(iAddress, 1, 2);
    liAddress->setBuddy(iAddress);

    iEMail = new QLineEdit(this);
    iEMail->resize(iEMail->sizeHint());
    mainGrid->addWidget(iEMail, 1, 3);
    liEMail->setBuddy(iEMail);

    change = new QPushButton("&Change", this);
    change->resize(change->sizeHint());
    mainGrid->addWidget(change, 1, 4);
    connect(change, SIGNAL(clicked()), this, SLOT(changeEntry()));

    treeView = new QTreeView(this);
    treeView->setSelectionMode(QTreeView::SingleSelection);
    treeView->setRootIsDecorated(false);

    model = new AddressBookModel(this);
    treeView->setModel(model);

    connect(treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(itemSelected(QModelIndex)));

    mainGrid->addWidget(treeView, 2, 0, 1, 5);
}

void AddressView::updateOutlook()
{
    model->update();
}

void AddressView::addEntry()
{
    if (!iFirstName->text().isEmpty() || !iLastName->text().isEmpty() ||
         !iAddress->text().isEmpty() || !iEMail->text().isEmpty()) {
        model->addItem(iFirstName->text(), iFirstName->text(), iAddress->text(), iEMail->text());
    }

    iFirstName->setText("");
    iLastName->setText("");
    iAddress->setText("");
    iEMail->setText("");
}

void AddressView::changeEntry()
{
    QModelIndex current = treeView->currentIndex();

    if (current.isValid())
        model->changeItem(current, iFirstName->text(), iLastName->text(), iAddress->text(), iEMail->text());
}

//! [9] //! [10]
void AddressView::itemSelected(const QModelIndex &index)
{
    if (!index.isValid())
	return;

    QAbstractItemModel *model = treeView->model();
    iFirstName->setText(model->data(model->index(index.row(), 0)).toString());
    iLastName->setText(model->data(model->index(index.row(), 1)).toString());
    iAddress->setText(model->data(model->index(index.row(), 2)).toString());
    iEMail->setText(model->data(model->index(index.row(), 3)).toString());
}
//! [10]
