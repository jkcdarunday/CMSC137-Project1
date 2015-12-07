#ifndef KHEADER_H
#define KHEADER_H

#include <bitset>
#include <QByteArray>
#include <QtEndian>

class KHeader
{
public:
    struct KHeaderData{
        quint32 seqNum;

        quint32 ackNum;

        quint8 offset;
        quint8 flags;
        quint16 window;
    };

    KHeaderData header;

    //    QByteArray data;

    KHeader();
    KHeader(QByteArray &data);
    QByteArray getByteArray();

    void readFrom(QByteArray &data);

    quint32 seqNum();
    quint32 ackNum();
    quint8 offset();
    quint16 window();

    void setSeqNum(quint32 value);
    void setAckNum(quint32 value);
    void setOffset(quint8 value);
    void setWindow(quint16 value);

    void incrementSeqNum(quint32 value = 1);
    void incrementAckNum(quint32 value = 1);
    void swapNums();

    void setSyn(bool value);
    void setAck(bool value);
    void setFin(bool value);

    bool syn();
    bool fin();
    bool ack();
};


#endif // KHEADER_H
