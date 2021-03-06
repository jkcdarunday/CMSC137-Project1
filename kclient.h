#ifndef KCLIENT_H
#define KCLIENT_H

#include <QMutex>
#include <QQueue>
#include <QTimer>
#include <QUdpSocket>
#include <kheader.h>
#include <cstdlib>
#include <ctime>
#include <iostream>



class KClient : public QUdpSocket
{
public:
    KClient();
    void connectTo(const QHostAddress &address, const quint16 &port);
    void sendData(const QByteArray &data = QByteArray());
    void disconnect();
public slots:
    void readPendingDatagrams();
    void resendData();
private:
    int state;
    KHeader *header;
    quint32 currentSeqNum;
    quint32 currentAckNum;
    quint32 timeout;

    QMutex isSending;

    quint32 baseSeqNum;
    QList<QByteArray> window;
    QQueue<QByteArray> toSend;
    QTimer *timer;
    bool needToAck;

    quint16 lastReceived;

    QHostAddress peer;
    quint16 peerPort;
};

#endif // KCLIENT_H
