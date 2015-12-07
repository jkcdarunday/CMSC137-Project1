#ifndef KSERVER_H
#define KSERVER_H

#include <QMutex>
#include <QQueue>
#include <QTimer>
#include <QUdpSocket>
#include <kheader.h>
#include <cstdlib>
#include <ctime>



class KServer : public QUdpSocket
{
public:
    KServer();
    bool listen(quint16 port);
    void sendData(const QByteArray &data = QByteArray());
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

    QHostAddress peer;
    quint16 peerPort;
};

#endif // KSERVER_H
