/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork of the Qt Toolkit.
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

//#define QNETWORKINTERFACE_DEBUG

#include "qnetworkinterface.h"
#include "qnetworkinterface_p.h"
#include "../corelib/kernel/qcore_symbian_p.h"

#ifndef QT_NO_NETWORKINTERFACE

#include <in_sock.h>
#include <in_iface.h>
#include <es_sock.h>

QT_BEGIN_NAMESPACE


static QNetworkInterface::InterfaceFlags convertFlags(const TSoInetInterfaceInfo& aInfo)
{
    QNetworkInterface::InterfaceFlags flags = 0;
    flags |= (aInfo.iState == EIfUp) ? QNetworkInterface::IsUp : QNetworkInterface::InterfaceFlag(0);
    // We do not have separate flag for running in Symbian OS
    flags |= (aInfo.iState == EIfUp) ? QNetworkInterface::IsRunning : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures & KIfCanBroadcast) ? QNetworkInterface::CanBroadcast : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures & KIfIsLoopback) ? QNetworkInterface::IsLoopBack : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures & KIfIsPointToPoint) ? QNetworkInterface::IsPointToPoint : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures & KIfCanMulticast) ? QNetworkInterface::CanMulticast : QNetworkInterface::InterfaceFlag(0);
    return flags;
}

static QList<QNetworkInterfacePrivate *> interfaceListing()
{
    TInt err(KErrNone);
    QList<QNetworkInterfacePrivate *> interfaces;

    // Connect to Native socket server
    RSocketServ socketServ;
    err = socketServ.Connect();
    if (err)
        return interfaces;

    // Open dummy socket for interface queries
    RSocket socket;
    err = socket.Open(socketServ, _L("udp"));
    if (err) {
        socketServ.Close();
        return interfaces;
    }

    // Ask socket to start enumerating interfaces
    err =  socket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
    if (err) {
        socket.Close();
        socketServ.Close();
        return interfaces;
    }

    int ifindex = 0;
    TPckgBuf<TSoInetInterfaceInfo> infoPckg;
    TSoInetInterfaceInfo &info = infoPckg();
    while (socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, infoPckg) == KErrNone) {
        // Do not include IPv6 addresses because netmask and broadcast address cannot be determined correctly
        if (info.iName != KNullDesC && info.iAddress.IsV4Mapped()) {
            TName address;
            QNetworkAddressEntry entry;
            QNetworkInterfacePrivate *iface = 0;

            iface = new QNetworkInterfacePrivate;
            iface->index = ifindex++;
            interfaces << iface;
            iface->name = qt_TDesC2QString(info.iName);
            iface->flags = convertFlags(info);

            if (/*info.iFeatures&KIfHasHardwareAddr &&*/ info.iHwAddr.Family() != KAFUnspec) {
                for (TInt i = sizeof(SSockAddr); i < sizeof(SSockAddr) + info.iHwAddr.GetUserLen(); i++) {
                    address.AppendNumFixedWidth(info.iHwAddr[i], EHex, 2);
                    if ((i + 1) < sizeof(SSockAddr) + info.iHwAddr.GetUserLen())
                        address.Append(_L(":"));
                }
                address.UpperCase();
                iface->hardwareAddress = qt_TDesC2QString(address);
            }

            // Get the address of the interface
            info.iAddress.Output(address);
            entry.setIp(QHostAddress(qt_TDesC2QString(address)));

            // Get the interface netmask
            // For some reason netmask is always 0.0.0.0
            // info.iNetMask.Output(address);
            // entry.setNetmask( QHostAddress( qt_TDesC2QString( address ) ) );

            // Workaround: Let Symbian determine netmask based on IP address class
            // TODO: Works only for IPv4 - Task: 259128 Implement IPv6 support
            TInetAddr netmask;
            netmask.NetMask(info.iAddress);
            netmask.Output(address);
            entry.setNetmask(QHostAddress(qt_TDesC2QString(address)));

            // Get the interface broadcast address
            if (iface->flags & QNetworkInterface::CanBroadcast) {
                // For some reason broadcast address is always 0.0.0.0
                // info.iBrdAddr.Output(address);
                // entry.setBroadcast( QHostAddress( qt_TDesC2QString( address ) ) );

                // Workaround: Let Symbian determine broadcast address based on IP address
                // TODO: Works only for IPv4 - Task: 259128 Implement IPv6 support
                TInetAddr broadcast;
                broadcast.NetBroadcast(info.iAddress);
                broadcast.Output(address);
                entry.setBroadcast(QHostAddress(qt_TDesC2QString(address)));
            }

            // Add new entry to interface address entries
            iface->addressEntries << entry;

#if defined(QNETWORKINTERFACE_DEBUG)
            printf("\n       Found network interface %s, interface flags:\n\
                IsUp = %d, IsRunning = %d, CanBroadcast = %d,\n\
                IsLoopBack = %d, IsPointToPoint = %d, CanMulticast = %d, \n\
                ip = %s, netmask = %s, broadcast = %s,\n\
                hwaddress = %s",
                   iface->name.toLatin1().constData(),
                   iface->flags & QNetworkInterface::IsUp, iface->flags & QNetworkInterface::IsRunning, iface->flags & QNetworkInterface::CanBroadcast,
                   iface->flags & QNetworkInterface::IsLoopBack, iface->flags & QNetworkInterface::IsPointToPoint, iface->flags & QNetworkInterface::CanMulticast,
                   entry.ip().toString().toLatin1().constData(), entry.netmask().toString().toLatin1().constData(), entry.broadcast().toString().toLatin1().constData(),
                   iface->hardwareAddress.toLatin1().constData());
#endif
        }
    }

    // we will try to use routing info to detect more precisely
    // netmask and then ::postProcess() should calculate
    // broadcast addresses

    // use dummy socket to start enumerating routes
    err =  socket.SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl);
    if (err) {
        socket.Close();
        socketServ.Close();
        // return what we have
        // up to this moment
        return interfaces;
    }

    TSoInetRouteInfo routeInfo;
    TPckg<TSoInetRouteInfo> routeInfoPkg(routeInfo);
    while (socket.GetOpt(KSoInetNextRoute, KSolInetRtCtrl, routeInfoPkg) == KErrNone) {
        TName address;

        // get interface address
        routeInfo.iIfAddr.Output(address);
        QHostAddress ifAddr(qt_TDesC2QString(address));
        if (ifAddr.isNull())
            continue;

        routeInfo.iDstAddr.Output(address);
        QHostAddress destination(qt_TDesC2QString(address));
        if (destination.isNull() || destination != ifAddr)
            continue;

        // search interfaces
        for (int ifindex = 0; ifindex < interfaces.size(); ++ifindex) {
            QNetworkInterfacePrivate *iface = interfaces.at(ifindex);
            for (int eindex = 0; eindex < iface->addressEntries.size(); ++eindex) {
                QNetworkAddressEntry entry = iface->addressEntries.at(eindex);
                if (entry.ip() != ifAddr) {
                    continue;
                } else if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                    // skip if not IPv4 address (e.g. IPv6)
                    // as results not reliable on Symbian
                    continue;
                } else {
                    routeInfo.iNetMask.Output(address);
                    QHostAddress netmask(qt_TDesC2QString(address));
                    entry.setNetmask(netmask);
                    // NULL boradcast address for
                    // ::postProcess to have effect
                    entry.setBroadcast(QHostAddress());
                    iface->addressEntries.replace(eindex, entry);
                }
            }
        }
    }

    socket.Close();
    socketServ.Close();

    return interfaces;
}

QList<QNetworkInterfacePrivate *> QNetworkInterfaceManager::scan()
{
    return interfaceListing();
}

QT_END_NAMESPACE

#endif // QT_NO_NETWORKINTERFACE
