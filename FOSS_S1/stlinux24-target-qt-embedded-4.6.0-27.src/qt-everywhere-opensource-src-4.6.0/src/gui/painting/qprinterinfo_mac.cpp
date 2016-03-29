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

#include "private/qt_mac_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PRINTER

class QPrinterInfoPrivate
{
Q_DECLARE_PUBLIC(QPrinterInfo)
public:
    ~QPrinterInfoPrivate();
    QPrinterInfoPrivate();
    QPrinterInfoPrivate(const QString& name);

private:
    QPrinterInfo*                 q_ptr;

    QString                     m_name;
    bool                        m_default;
    bool                        m_isNull;
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

extern QPrinter::PaperSize qSizeFTopaperSize(const QSizeF& size);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QList<QPrinterInfo> QPrinterInfo::availablePrinters()
{
    QList<QPrinterInfo> printers;

    OSStatus status = noErr;
    QCFType<CFArrayRef> printerList;
    status = PMServerCreatePrinterList(kPMServerLocal, &printerList);
    if (status == noErr) {
        CFIndex count = CFArrayGetCount(printerList);
        for (CFIndex i=0; i<count; ++i) {
            PMPrinter printer = static_cast<PMPrinter>(const_cast<void *>(CFArrayGetValueAtIndex(printerList, i)));
            QString name = QCFString::toQString(PMPrinterGetName(printer));
            printers.append(QPrinterInfo(name));
            if (PMPrinterIsDefault(printer)) {
                printers[i].d_ptr->m_default = true;
            }
        }
    }

    return printers;
}

QPrinterInfo QPrinterInfo::defaultPrinter(){
    QList<QPrinterInfo> printers = availablePrinters();
    for (int c = 0; c < printers.size(); ++c) {
        if (printers[c].isDefault()) {
            return printers[c];
        }
    }
    return QPrinterInfo();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

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
}

QPrinterInfo::~QPrinterInfo()
{
}

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

    PMPrinter cfPrn = PMPrinterCreateFromPrinterID(QCFString::toCFStringRef(d->m_name));

    if (!cfPrn) return QList<QPrinter::PaperSize>();

    CFArrayRef array;
    OSStatus status = PMPrinterGetPaperList(cfPrn, &array);

    if (status != 0) {
        PMRelease(cfPrn);
        return QList<QPrinter::PaperSize>();
    }

    QList<QPrinter::PaperSize> paperList;
    int count = CFArrayGetCount(array);
    for (int c = 0; c < count; c++) {
        PMPaper paper = static_cast<PMPaper>(
                const_cast<void*>(
                CFArrayGetValueAtIndex(array, c)));
        double width, height;
        status = PMPaperGetWidth(paper, &width);
        status |= PMPaperGetHeight(paper, &height);
        if (status != 0) continue;

        QSizeF size(width * 0.3527, height * 0.3527);
        paperList.append(qSizeFTopaperSize(size));
    }

    PMRelease(cfPrn);

    return paperList;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QPrinterInfoPrivate::QPrinterInfoPrivate() :
    q_ptr(NULL),
    m_default(false),
    m_isNull(true)
{
}

QPrinterInfoPrivate::QPrinterInfoPrivate(const QString& name) :
    q_ptr(NULL),
    m_name(name),
    m_default(false),
    m_isNull(false)
{
}

QPrinterInfoPrivate::~QPrinterInfoPrivate()
{
}

#endif // QT_NO_PRINTER

QT_END_NAMESPACE
