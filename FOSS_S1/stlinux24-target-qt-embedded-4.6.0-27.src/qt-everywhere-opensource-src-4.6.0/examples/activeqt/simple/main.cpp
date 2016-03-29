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

#include <QAxBindable>
#include <QAxFactory>
#include <QApplication>
#include <QLayout>
#include <QSlider>
#include <QLCDNumber>
#include <QLineEdit>
#include <QMessageBox>

//! [0]
class QSimpleAX : public QWidget, public QAxBindable
{
    Q_OBJECT
    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( int value READ value WRITE setValue )
public:
    QSimpleAX(QWidget *parent = 0)
    : QWidget(parent)
    {
	QVBoxLayout *vbox = new QVBoxLayout( this );

        slider = new QSlider( Qt::Horizontal, this );
	LCD = new QLCDNumber( 3, this );
	edit = new QLineEdit( this );

	connect( slider, SIGNAL( valueChanged( int ) ), this, SLOT( setValue(int) ) );
	connect( edit, SIGNAL(textChanged(const QString&)), this, SLOT(setText(const QString&)) );

	vbox->addWidget( slider );
	vbox->addWidget( LCD );
	vbox->addWidget( edit );
    }

    QString text() const 
    { 
	return edit->text(); 
    }
    int value() const
    {
	return slider->value();
    }

signals:
    void someSignal();
    void valueChanged(int);
    void textChanged(const QString&);

public slots:
    void setText( const QString &string )
    {
	if ( !requestPropertyChange( "text" ) )
	    return;

	edit->blockSignals( true );
	edit->setText( string );
	edit->blockSignals( false );
	emit someSignal();
	emit textChanged( string );

	propertyChanged( "text" );
    }
    void about()
    {
	QMessageBox::information( this, "About QSimpleAX", "This is a Qt widget, and this slot has been\n"
							  "called through ActiveX/OLE automation!" );
    }
    void setValue( int i )
    {
	if ( !requestPropertyChange( "value" ) )
	    return;
	slider->blockSignals( true );
	slider->setValue( i );
	slider->blockSignals( false );
	LCD->display( i );
	emit valueChanged( i );

	propertyChanged( "value" );
    }

private:
    QSlider *slider;
    QLCDNumber *LCD;
    QLineEdit *edit;
};

//! [0]
#include "main.moc"

//! [1]
QAXFACTORY_DEFAULT(QSimpleAX,
	   "{DF16845C-92CD-4AAB-A982-EB9840E74669}",
	   "{616F620B-91C5-4410-A74E-6B81C76FFFE0}",
	   "{E1816BBA-BF5D-4A31-9855-D6BA432055FF}",
	   "{EC08F8FC-2754-47AB-8EFE-56A54057F34E}",	   
	   "{A095BA0C-224F-4933-A458-2DD7F6B85D8F}")
//! [1]
