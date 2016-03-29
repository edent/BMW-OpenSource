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
#ifndef QSYM_IAP_UTIL_H
#define QSYM_IAP_UTIL_H

// Symbian
#include <utf.h>
#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>
#include <in_iface.h>
#include <commdbconnpref.h>
#include <e32cmn.h>

// OpenC
#include <sys/socket.h>
#include <net/if.h>

//Qt
#include <QSettings>
#include <QStringList>
//#include <QTextCodec>

_LIT(KIapNameSetting, "IAP\\Name");             // text - mandatory
_LIT(KIapDialogPref, "IAP\\DialogPref");        // TUnit32 - optional
_LIT(KIapService, "IAP\\IAPService");           // TUnit32 - mandatory
_LIT(KIapServiceType, "IAP\\IAPServiceType");   // text - mandatory
_LIT(KIapBearer, "IAP\\IAPBearer");             // TUint32 - optional
_LIT(KIapBearerType, "IAP\\IAPBearerType");     // text - optional
_LIT(KIapNetwork, "IAP\\IAPNetwork");           // TUint32 - optional

const QLatin1String qtOrganizationTag("Trolltech");
const QLatin1String qtNetworkModuleTag("QtNetwork");
const QLatin1String iapGroupTag("IAP");
const QLatin1String iapNamesArrayTag("Names");
const QLatin1String iapNameItemTag("Name");

static QTextCodec *utf16LETextCodec = 0;

void clearIapNamesSettings(QSettings &settings) {
    settings.beginGroup(qtNetworkModuleTag);
        settings.beginGroup(iapGroupTag);
           settings.remove(iapNamesArrayTag);
        settings.endGroup();
    settings.endGroup();
}

void writeIapNamesSettings(QSettings &settings, const QStringList& iapNames) {
    clearIapNamesSettings(settings);
    settings.beginGroup(qtNetworkModuleTag);
        settings.beginGroup(iapGroupTag);
            settings.beginWriteArray(iapNamesArrayTag);
            for (int index = 0; index < iapNames.size(); ++index) {
                settings.setArrayIndex(index);
                settings.setValue(iapNameItemTag, iapNames.at(index));
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
}

void readIapNamesSettings(QSettings &settings, QStringList& iapNames) {
    settings.beginGroup(qtNetworkModuleTag);
        settings.beginGroup(iapGroupTag);
            int last = settings.beginReadArray(iapNamesArrayTag);
            for (int index = 0; index < last; ++index) {
                settings.setArrayIndex(index);
                iapNames.append(settings.value(iapNameItemTag).toString());
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
}

static QString qt_TNameToQString(TName data) {
    if(utf16LETextCodec == 0)
        utf16LETextCodec = QTextCodec::codecForName("UTF-16LE");

    QByteArray tmpByteArray = QByteArray::fromRawData((char*)(data.PtrZ()), data.Length() * 2);
    return utf16LETextCodec->toUnicode(tmpByteArray);
}

static QString qt_InterfaceInfoL()
{
    QString output;

    TBuf8<512> buffer;
    TBuf<128> t;
    TAutoClose<RSocketServ> ss;
    User::LeaveIfError(ss.iObj.Connect());
    ss.PushL();

    TAutoClose<RSocket> sock;
    User::LeaveIfError(sock.iObj.Open(ss.iObj, _L("udp")));
    sock.PushL();

    User::LeaveIfError(sock.iObj.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));

    TProtocolDesc in;
    User::LeaveIfError(sock.iObj.Info(in));
    printf("EPOC32 IP Configuration TCPIP Version %d.%d.%d\n", in.iVersion.iMajor, in.iVersion.iMinor, in.iVersion.iBuild);

    TPckgBuf<TSoInetInterfaceInfo> info, next;

    TInt res=sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
    if(res!=KErrNone)
        User::Leave(res);
    TInt count = 0;
    while(res==KErrNone) {
        res=sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, next);

        if(info().iName != _L("") && info().iName != _L("loop6") && info().iName != _L("loop4")) {
            printf("Interface %d\n", count++);

            printf("Name \"%s\"\n", qt_TNameToQString(info().iName).toLatin1().data());
            printf("NIF tag \"%s\"\n", qt_TNameToQString(info().iTag).toLatin1().data());

            printf("State ");
            switch (info().iState)
            {
                case EIfPending:
                    printf("pending\n");
                    break;
                case EIfUp:
                    printf("up\n");
                    break;
                case EIfBusy:
                    printf("busy\n");
                    break;
                default:
                    printf("down\n");
                    break;
            }

            printf("Mtu %d\n", info().iMtu);
            printf("Speed Metric %d\n", info().iSpeedMetric);

            printf("Features:");
            info().iFeatures & KIfIsLoopback         ? printf(" loopback") : printf("");
            info().iFeatures & KIfIsDialup           ? printf(" dialup") : printf("");
            info().iFeatures & KIfIsPointToPoint     ? printf(" pointtopoint") : printf("");
            info().iFeatures & KIfCanBroadcast       ? printf(" canbroadcast") : printf("");
            info().iFeatures & KIfCanMulticast       ? printf(" canmulticast") : printf("");
            info().iFeatures & KIfCanSetMTU          ? printf(" cansetmtu") : printf("");
            info().iFeatures & KIfHasHardwareAddr    ? printf(" hardwareaddr") : printf("");
            info().iFeatures & KIfCanSetHardwareAddr ? printf(" cansethardwareaddr") : printf("");
            printf("\n");

            TName address;
            info().iAddress.Output(address);
            printf("Addr: %s\n", qt_TNameToQString(address).toLatin1().data());

            if(info().iAddress.IsLinkLocal()) {
                printf("  -link local\n");
            } else if(info().iAddress.IsSiteLocal()) {
                printf("  -site local\n");
            } else {
                printf("  -global\n");
            }

            info().iNetMask.Output(address);
            printf("Netmask %s\n", qt_TNameToQString(address).toLatin1().data());

            info().iBrdAddr.Output(address);
            printf("Broadcast address %s\n", qt_TNameToQString(address).toLatin1().data());

            info().iDefGate.Output(address);
            printf("Gatew: %s\n", qt_TNameToQString(address).toLatin1().data());

            info().iNameSer1.Output(address);
            printf("DNS 1: %s\n", qt_TNameToQString(address).toLatin1().data());

            info().iNameSer2.Output(address);
            printf("DNS 2: %s\n", qt_TNameToQString(address).toLatin1().data());

            if (info().iHwAddr.Family() != KAFUnspec) {
                printf("Hardware address ");
                TUint j;
                for(j = sizeof(SSockAddr) ; j < sizeof(SSockAddr) + 6 ; ++j) {
                    if(j < (TUint)info().iHwAddr.Length()) {
                        printf("%02X", info().iHwAddr[j]);
                    } else {
                        printf("??");
                    }
                    if(j < sizeof(SSockAddr) + 5)
                        printf("-");
                    else
                        printf("\n");
                    }
                }
            }
        if(res == KErrNone) {
            info = next;
            printf("\n");
        } else {
            printf("\n");
        }
    }

    sock.Pop();
    ss.Pop();

    return output;
}

static QString qt_RouteInfoL() {
    QString output;
    TAutoClose<RSocketServ> ss;
    User::LeaveIfError(ss.iObj.Connect());
    ss.PushL();

    TAutoClose<RSocket> sock;
    User::LeaveIfError(sock.iObj.Open(ss.iObj, _L("udp")));
    sock.PushL();

    TSoInetRouteInfo routeInfo;
    TPckg<TSoInetRouteInfo> routeInfoPkg(routeInfo);

    TName destAddr;
    TName netMask;
    TName gateway;
    TName ifAddr;

    // Begins enumeration of routes by setting this option
    User::LeaveIfError(sock.iObj.SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl));

    // The TSoInetRouteInfo contains information for a new route each time GetOpt returns KErrNone
    for(TInt i = 0; sock.iObj.GetOpt(KSoInetNextRoute, KSolInetRtCtrl, routeInfoPkg) == KErrNone ; i++)
    {
      // Extract the destination and netmask
      routeInfo.iDstAddr.Output(destAddr);
      routeInfo.iNetMask.Output(netMask);
      routeInfo.iGateway.Output(gateway);
      routeInfo.iIfAddr.Output(ifAddr);
/*
      if(destAddr.Length() <= 2)
          continue;

      if(netMask.Find(_L("255.255.255.255")) != KErrNotFound
              || netMask.Find(_L("0.0.0.0")) != KErrNotFound
              || netMask.Find(_L("ffff:ffff:ffff:ffff")) != KErrNotFound)
          continue;
*/
      printf("Route Info #[%i]\n", i);
      printf("DstAddr %s\n", qt_TNameToQString(destAddr).toLatin1().data());
      printf("NetMask %s\n", qt_TNameToQString(netMask).toLatin1().data());
      printf("Gateway %s\n", qt_TNameToQString(gateway).toLatin1().data());
      printf("IfAddr %s\n", qt_TNameToQString(ifAddr).toLatin1().data());
      printf("\n");
    }

    sock.Pop();
    ss.Pop();

    return output;
}

QString qt_TDesC2QStringL(const TDesC& aDescriptor)
{
#ifdef QT_NO_UNICODE
    return QString::fromLocal8Bit(aDescriptor.Ptr(), aDescriptor.Length());
#else
    return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
#endif
}

static bool qt_SetDefaultIapName(const QString &iapName, int &error) {
    struct ifreq ifReq;
    // clear structure
    memset(&ifReq, 0, sizeof(struct ifreq));
    // set IAP name value
    // make sure it is in UTF8
    strcpy(ifReq.ifr_name, iapName.toUtf8().data());

    if(setdefaultif(&ifReq) == 0) {
        // OK
        error = 0;
        return true;
    } else {
        error = errno;
        return false;
    }

}
static bool qt_SetDefaultSnapId(const int snapId, int &error) {
    struct ifreq ifReq;
    // clear structure
    memset(&ifReq, 0, sizeof(struct ifreq));
    // set SNAP ID value
    ifReq.ifr_ifru.snap_id = snapId;

    if(setdefaultif(&ifReq) == 0) {
        // OK
        error = 0;
        return true;
    } else {
        error = errno;
        return false;
    }

}

static void qt_SaveIapName(QSettings& settings, QStringList& iapNames, QString& iapNameValue) {
    if(iapNames.contains(iapNameValue) && iapNames.first() == iapNameValue) {
        // no need to update
    } else {
        if(iapNameValue != QString("Easy WLAN")) {
            // new selection alway on top
            iapNames.removeAll(iapNameValue);
            iapNames.prepend(iapNameValue);
            writeIapNamesSettings(settings, iapNames);
        } else {
            // Unbeliveable ... if IAP dodn't exist before
            // no matter what you choose from IAP selection list
            // you will get "Easy WLAN" as IAP name value

            // somehow commsdb is not in sync
        }
    }
}

static QString qt_OfferIapDialog() {
    TBuf8<256> iapName;

    RSocketServ socketServ;
    CleanupClosePushL(socketServ);

    RConnection connection;
    CleanupClosePushL(connection);

    socketServ.Connect();
    connection.Open(socketServ);
    connection.Start();

    connection.GetDesSetting(TPtrC(KIapNameSetting), iapName);

    //connection.Stop();

    iapName.ZeroTerminate();
    QString strIapName((char*)iapName.Ptr());

    int error = 0;
    if(!qt_SetDefaultIapName(strIapName, error)) {
        //printf("failed setdefaultif @ %i with %s and errno = %d \n", __LINE__, strIapName.toUtf8().data(), error);
        strIapName = QString("");
    }

    CleanupStack::PopAndDestroy(&connection);
    CleanupStack::PopAndDestroy(&socketServ);

    return strIapName;
}

static QString qt_CheckForActiveConnection() {
    TUint count;

    RSocketServ serv;
    CleanupClosePushL(serv);

    RConnection conn;
    CleanupClosePushL(conn);

    serv.Connect();
    conn.Open(serv);

    TConnectionInfoBuf connInfo;

    TBuf8<256> iapName;
    TBuf8<256> iapServiceType;

    QString strIapName;

    if (conn.EnumerateConnections(count) == KErrNone) {
        if(count > 0) {
            for (TUint i = 1; i <= count; i++) {
                if (conn.GetConnectionInfo(i, connInfo) == KErrNone) {
                    RConnection tempConn;
                    CleanupClosePushL(tempConn);
                    tempConn.Open(serv);
                    if (tempConn.Attach(connInfo, RConnection::EAttachTypeNormal) == KErrNone) {
                       tempConn.GetDesSetting(TPtrC(KIapNameSetting), iapName);
                       tempConn.GetDesSetting(TPtrC(KIapServiceType), iapServiceType);
                       //tempConn.Stop();
                       iapName.ZeroTerminate();
		               iapServiceType.ZeroTerminate();

//                        if(iapServiceType.Find(_L8("LANService")) != KErrNotFound) {
//                            activeLanConnectionFound = ETrue;
//                            break;
//                        }
			            strIapName = QString((char*)iapName.Ptr());
                        int error = 0;
                        if(!qt_SetDefaultIapName(strIapName, error)) {
                            //printf("failed setdefaultif @ %i with %s and errno = %d \n", __LINE__, strIapName.toUtf8().data(), error);
                            strIapName = QString("");
                        }

                        CleanupStack::PopAndDestroy(&tempConn);
                        break;
                    }
                }
            }
        }
    }

    //conn.Stop();

    CleanupStack::PopAndDestroy(&conn);
    CleanupStack::PopAndDestroy(&serv);

    return strIapName;
}

static QString qt_CheckSettingsForConnection(QStringList& iapNames) {
    QString strIapName;
    for(int index = 0; index < iapNames.size(); ++index) {
        strIapName = iapNames.at(index);
        int error = 0;
        if(!qt_SetDefaultIapName(strIapName, error)) {
            //printf("failed setdefaultif @ %i with %s and errno = %d \n", __LINE__, strIapName.toUtf8().data(), error);
            strIapName = QString("");
        } else {
            return strIapName;
        }
    }
    return strIapName;
}

static void qt_SetDefaultIapL()
{
    // settings @ /c/data/.config/Trolltech.com
    QSettings settings(QSettings::UserScope, qtOrganizationTag);
    // populate iap name list
    QStringList iapNames;
    readIapNamesSettings(settings, iapNames);

    QString iapNameValue;

    iapNameValue = qt_CheckForActiveConnection();

    if(!iapNameValue.isEmpty()) {
        qt_SaveIapName(settings, iapNames, iapNameValue);
        return;
    }

    iapNameValue = qt_CheckSettingsForConnection(iapNames);

    if(!iapNameValue.isEmpty()) {
        qt_SaveIapName(settings, iapNames, iapNameValue);
        return;
    }

    /*
     * no active LAN connections yet
     * no IAP in settings
     * offer IAP dialog to user
     */
    iapNameValue = qt_OfferIapDialog();
    qt_SaveIapName(settings, iapNames, iapNameValue);
    return;

}

static int qt_SetDefaultIap()
{
#ifndef __WINS__
    TRAPD(err1, qt_SetDefaultIapL());
//    TRAPD(err2, qt_InterfaceInfoL());
//    TRAPD(err3, qt_RouteInfoL());
    return err1;
#else
    return 0; // IAP dialog not required for emulator
#endif
}

#endif // QSYM_IAP_UTIL_H
