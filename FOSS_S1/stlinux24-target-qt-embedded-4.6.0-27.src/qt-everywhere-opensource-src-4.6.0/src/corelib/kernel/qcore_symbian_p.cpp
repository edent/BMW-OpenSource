/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <exception>
#include <e32base.h>
#include <e32uid.h>
#include "qcore_symbian_p.h"
#include <string>

QT_BEGIN_NAMESPACE

/*
    Helper function for calling into Symbian classes that expect a TDes&.
    This function converts a QString to a TDes by allocating memory that
    must be deleted by the caller.
*/

Q_CORE_EXPORT HBufC* qt_QString2HBufC(const QString& aString)
{
    HBufC *buffer;
#ifdef QT_NO_UNICODE
    TPtrC8 ptr(reinterpret_cast<const TUint8*>(aString.toLocal8Bit().constData()));
#else
    TPtrC16 ptr(qt_QString2TPtrC(aString));
#endif
    buffer = q_check_ptr(HBufC::New(ptr.Length()));
    buffer->Des().Copy(ptr);
    return buffer;
}

Q_CORE_EXPORT QString qt_TDesC2QString(const TDesC& aDescriptor)
{
#ifdef QT_NO_UNICODE
    return QString::fromLocal8Bit(aDescriptor.Ptr(), aDescriptor.Length());
#else
    return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
#endif
}

QHBufC::QHBufC()
    : m_hBufC(0)
{
}

QHBufC::QHBufC(const QHBufC &src)
	: m_hBufC(q_check_ptr(src.m_hBufC->Alloc()))
{
}

/*!
  \internal
  Constructs a QHBufC from an HBufC. Note that the QHBufC instance takes
  ownership of the HBufC.
*/
QHBufC::QHBufC(HBufC *src)
    : m_hBufC(src)
{
}

QHBufC::QHBufC(const QString &src)
{
    m_hBufC = qt_QString2HBufC(src);
}

QHBufC::~QHBufC()
{
    if (m_hBufC)
        delete m_hBufC;
}

class QS60PluginResolver
{
public:
    QS60PluginResolver()
        : initTried(false) {}

    ~QS60PluginResolver() {
        lib.Close();
    }

    TLibraryFunction resolve(int ordinal) {
        if (!initTried) {
            init();
            initTried = true;
        }

        if (lib.Handle())
            return lib.Lookup(ordinal);
        else
            return reinterpret_cast<TLibraryFunction>(NULL);
    }

private:
    void init()
    {
#ifdef Q_WS_S60
        _LIT(KLibName_3_1, "qts60plugin_3_1.dll");
        _LIT(KLibName_3_2, "qts60plugin_3_2.dll");
        _LIT(KLibName_5_0, "qts60plugin_5_0.dll");
        TPtrC libName;
        TInt uidValue;
        switch (QSysInfo::s60Version()) {
        case QSysInfo::SV_S60_3_1:
            libName.Set(KLibName_3_1);
            uidValue = 0x2001E620;
            break;
        case QSysInfo::SV_S60_3_2:
            libName.Set(KLibName_3_2);
            uidValue = 0x2001E621;
            break;
        case QSysInfo::SV_S60_5_0: // Fall through to default
        default:
            // Default to 5.0 version, as any unknown platform is likely to be newer than that
            libName.Set(KLibName_5_0);
            uidValue = 0x2001E622;
            break;
        }

        TUidType libUid(KDynamicLibraryUid, KSharedLibraryUid, TUid::Uid(uidValue));
        lib.Load(libName, libUid);
#endif
    }

    RLibrary lib;
    bool initTried;
};

Q_GLOBAL_STATIC(QS60PluginResolver, qt_s60_plugin_resolver);

/*!
  \internal
  Resolves a platform version specific function from S60 plugin.
  If plugin is missing or resolving fails for another reason, NULL is returned.
*/
Q_CORE_EXPORT TLibraryFunction qt_resolveS60PluginFunc(int ordinal)
{
    return qt_s60_plugin_resolver()->resolve(ordinal);
}

class QS60RFsSession
{
public:
    QS60RFsSession() {
        qt_symbian_throwIfError(iFs.Connect());
        qt_symbian_throwIfError(iFs.ShareProtected());
    }

    ~QS60RFsSession() {
        iFs.Close();
    }

    RFs& GetRFs() {
        return iFs;
    }

private:

    RFs iFs;
};

Q_GLOBAL_STATIC(QS60RFsSession, qt_s60_RFsSession);

Q_CORE_EXPORT RFs& qt_s60GetRFs()
{
    return qt_s60_RFsSession()->GetRFs();
}

QT_END_NAMESPACE
