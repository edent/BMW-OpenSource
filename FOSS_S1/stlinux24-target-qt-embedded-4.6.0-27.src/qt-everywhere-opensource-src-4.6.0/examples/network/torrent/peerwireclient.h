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

#ifndef PEERWIRECLIENT_H
#define PEERWIRECLIENT_H

#include <QBitArray>
#include <QList>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QHostAddress;
class QTimerEvent;
QT_END_NAMESPACE
class TorrentPeer;

struct TorrentBlock
{
    inline TorrentBlock(int p, int o, int l)
        : pieceIndex(p), offset(o), length(l)
    {
    }
    inline bool operator==(const TorrentBlock &other) const
    {
        return pieceIndex == other.pieceIndex
                && offset == other.offset
                && length == other.length;
    }
    
    int pieceIndex;
    int offset;
    int length;
};

class PeerWireClient : public QTcpSocket
{
    Q_OBJECT

public:
    enum PeerWireStateFlag {
        ChokingPeer = 0x1,
        InterestedInPeer = 0x2,
        ChokedByPeer = 0x4,
        PeerIsInterested = 0x8
    };
    Q_DECLARE_FLAGS(PeerWireState, PeerWireStateFlag)

    PeerWireClient(const QByteArray &peerId, QObject *parent = 0);
    void initialize(const QByteArray &infoHash, int pieceCount);

    void setPeer(TorrentPeer *peer);
    TorrentPeer *peer() const;

    // State
    inline PeerWireState peerWireState() const { return pwState; }
    QBitArray availablePieces() const;
    QList<TorrentBlock> incomingBlocks() const;

    // Protocol
    void chokePeer();
    void unchokePeer();
    void sendInterested();
    void sendKeepAlive();
    void sendNotInterested();
    void sendPieceNotification(int piece);
    void sendPieceList(const QBitArray &bitField);
    void requestBlock(int piece, int offset, int length);
    void cancelRequest(int piece, int offset, int length);
    void sendBlock(int piece, int offset, const QByteArray &data);

    // Rate control
    qint64 writeToSocket(qint64 bytes);
    qint64 readFromSocket(qint64 bytes);
    qint64 downloadSpeed() const;
    qint64 uploadSpeed() const;

    bool canTransferMore() const;
    qint64 bytesAvailable() const { return incomingBuffer.size() + QTcpSocket::bytesAvailable(); }
    qint64 socketBytesAvailable() const { return socket.bytesAvailable(); }
    qint64 socketBytesToWrite() const { return socket.bytesToWrite(); }

    void setReadBufferSize(int size);

signals:
    void infoHashReceived(const QByteArray &infoHash);
    void readyToTransfer();

    void choked();
    void unchoked();
    void interested();
    void notInterested();

    void piecesAvailable(const QBitArray &pieces);
    void blockRequested(int pieceIndex, int begin, int length);
    void blockReceived(int pieceIndex, int begin, const QByteArray &data);

    void bytesReceived(qint64 size);

protected slots:
    void connectToHostImplementation(const QString &hostName,
                                     quint16 port, OpenMode openMode = ReadWrite);
    void diconnectFromHostImplementation();

protected:
    void timerEvent(QTimerEvent *event);

    qint64 readData(char *data, qint64 maxlen);
    qint64 readLineData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private slots:
    void sendHandShake();
    void processIncomingData();
    void socketStateChanged(QAbstractSocket::SocketState state);

private:
    // Data waiting to be read/written
    QByteArray incomingBuffer;
    QByteArray outgoingBuffer;

    struct BlockInfo {
        int pieceIndex;
        int offset;
        int length;
        QByteArray block;
    };
    QList<BlockInfo> pendingBlocks;
    int pendingBlockSizes;
    QList<TorrentBlock> incoming;

    enum PacketType {
        ChokePacket = 0,
        UnchokePacket = 1,
        InterestedPacket = 2,
        NotInterestedPacket = 3,
        HavePacket = 4,
        BitFieldPacket = 5,
        RequestPacket = 6,
        PiecePacket = 7,
        CancelPacket = 8
    };

    // State
    PeerWireState pwState;
    bool receivedHandShake;
    bool gotPeerId;
    bool sentHandShake;
    int nextPacketLength;

    // Upload/download speed records
    qint64 uploadSpeedData[8];
    qint64 downloadSpeedData[8];
    int transferSpeedTimer;

    // Timeout handling
    int timeoutTimer;
    int pendingRequestTimer;
    bool invalidateTimeout;
    int keepAliveTimer;

    // Checksum, peer ID and set of available pieces
    QByteArray infoHash;
    QByteArray peerIdString;
    QBitArray peerPieces;
    TorrentPeer *torrentPeer;

    QTcpSocket socket;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PeerWireClient::PeerWireState)

#endif
