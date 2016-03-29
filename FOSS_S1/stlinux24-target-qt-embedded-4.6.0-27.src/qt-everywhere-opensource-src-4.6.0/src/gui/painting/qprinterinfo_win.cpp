/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qprinterinfo.h"

#include <qstringlist.h>

#include <qt_windows.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PRINTER

extern QPrinter::PaperSize mapDevmodePaperSize(int s);

class QPrinterInfoPrivate
{
Q_DECLARE_PUBLIC(QPrinterInfo)
public:
    ~QPrinterInfoPrivate();
    QPrinterInfoPrivate();
    QPrinterInfoPrivate(const QString& name);

private:
    QString                     m_name;
    bool                        m_default;
    bool                        m_isNull;

    QPrinterInfo*               q_ptr;
};

static QPrinterInfoPrivate nullQPrinterInfoPrivate;

class QPrinterInfoPrivateDeleter
{
public:
    static inline void cleanup(QPrinterInfoPrivate *d)
    {
        if (d != &nullQPrinterInfoPrivate)
            delete d;
    }
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QList<QPrinterInfo> QPrinterInfo::availablePrinters()
{
    QList<QPrinterInfo> printers;
    LPBYTE buffer;
    DWORD needed = 0;
    DWORD returned = 0;

    if ( !EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, 0, 0, &needed, &returned))
    {
        buffer = new BYTE[needed];
        if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS , NULL,
                           4, buffer, needed, &needed, &returned))
        {
            delete [] buffer;
            return printers;
        }
        PPRINTER_INFO_4 infoList = reinterpret_cast<PPRINTER_INFO_4>(buffer);
        QPrinterInfo defPrn = defaultPrinter();
        for (uint i = 0; i < returned; ++i) {
            printers.append(QPrinterInfo(QString::fromWCharArray(infoList[i].pPrinterName)));
            if (printers.at(i).printerName() == defPrn.printerName())
                printers[i].d_ptr->m_default = true;
        }
        delete [] buffer;
    }

    return printers;
}

QPrinterInfo QPrinterInfo::defaultPrinter()
{
    QString noPrinters(QLatin1String("qt_no_printers"));
    wchar_t buffer[256];
    GetProfileString(L"windows", L"device", (wchar_t*)noPrinters.utf16(), buffer, 256);
    QString output = QString::fromWCharArray(buffer);

    // Filter out the name of the printer, which should be everything
    // before a comma.
    bool noConfiguredPrinters = (output == noPrinters);
    QStringList info = output.split(QLatin1Char(','));
    QString printerName = noConfiguredPrinters ? QString() : info.at(0);

    QPrinterInfo prn(printerName);
    prn.d_ptr->m_default = true;
    if (noConfiguredPrinters)
        prn.d_ptr->m_isNull = true;
    return prn;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QPrinterInfo::QPrinterInfo()
    : d_ptr(&nullQPrinterInfoPrivate)
{
}

QPrinterInfo::QPrinterInfo(const QString& name)
    : d_ptr(new QPrinterInfoPrivate(name))
{
    d_ptr->q_ptr = this;
}

QPrinterInfo::QPrinterInfo(const QPrinterInfo& src)
    : d_ptr(&nullQPrinterInfoPrivate)
{
    *this = src;
}

QPrinterInfo::QPrinterInfo(const QPrinter& prn)
    : d_ptr(&nullQPrinterInfoPrivate)
{
    QList<QPrinterInfo> list = availablePrinters();
    for (int c = 0; c < list.size(); ++c) {
        if (prn.printerName() == list[c].printerName()) {
            *this = list[c];
            return;
        }
    }

    *this = QPrinterInfo();
}

QPrinterInfo::~QPrinterInfo()
{
}

QPrinterInfo& QPrinterInfo::operator=(const QPrinterInfo& src)
{
    Q_ASSERT(d_ptr);
    d_ptr.reset(new QPrinterInfoPrivate(*src.d_ptr));
    d_ptr->q_ptr = this;
    return *this;
}

QString QPrinterInfo::printerName() const
{
    const Q_D(QPrinterInfo);
    return d->m_name;
}

bool QPrinterInfo::isNull() const
{
    const Q_D(QPrinterInfo);
    return d->m_isNull;
}

bool QPrinterInfo::isDefault() const
{
    const Q_D(QPrinterInfo);
    return d->m_default;
}

QList<QPrinter::PaperSize> QPrinterInfo::supportedPaperSizes() const
{
    const Q_D(QPrinterInfo);
    QList<QPrinter::PaperSize> paperList;

    DWORD size = DeviceCapabilities(reinterpret_cast<const wchar_t*>(d->m_name.utf16()),
                                    NULL, DC_PAPERS, NULL, NULL);
    if ((int)size == -1)
        return paperList;

    wchar_t *papers = new wchar_t[size];
    size = DeviceCapabilities(reinterpret_cast<const wchar_t*>(d->m_name.utf16()),
                              NULL, DC_PAPERS, papers, NULL);

    for (int c = 0; c < (int)size; ++c) {
        paperList.append(mapDevmodePaperSize(papers[c]));
    }

    delete [] papers;

    return paperList;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QPrinterInfoPrivate::QPrinterInfoPrivate() :
    m_default(false),
    m_isNull(true),
    q_ptr(NULL)
{
}

QPrinterInfoPrivate::QPrinterInfoPrivate(const QString& name) :
    m_name(name),
    m_default(false),
    m_isNull(false),
    q_ptr(NULL)
{
}

QPrinterInfoPrivate::~QPrinterInfoPrivate()
{
}

#endif // QT_NO_PRINTER

QT_END_NAMESPACE
