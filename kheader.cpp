#include "kheader.h"
#include <QDebug>

KHeader::KHeader()
{
    header.seqNum = qToBigEndian(1);
    header.ackNum = qToBigEndian(2);

    header.offset = 3;
    header.flags = 0;

    header.window = qToBigEndian((quint16)5);
}

KHeader::KHeader(QByteArray &data)
{
    readFrom(data);
}

void KHeader::readFrom(QByteArray &data)
{
    QByteArray headerData = data.left(12);
    if(data.size()>=4*3){
        this->header = *((KHeaderData*)(headerData.data()));
    }
}

bool KHeader::syn(){
    return ((header.flags >> 1) & 1) == 1;
}

bool KHeader::fin(){
    return ((header.flags) & 1) == 1;
}

bool KHeader::ack(){
    return ((header.flags >> 4) & 1) == 1;
}

QByteArray KHeader::getByteArray(){
    return QByteArray::fromRawData((const char*)&header, sizeof(struct KHeaderData));
}

quint32 KHeader::seqNum()
{
    return qFromBigEndian(header.seqNum);
}

quint32 KHeader::ackNum()
{
    return qFromBigEndian(header.ackNum);
}

quint8 KHeader::offset()
{
    return header.offset;
}

quint16 KHeader::window()
{
    return qFromBigEndian(header.window);
}

void KHeader::setSeqNum(quint32 value)
{
    this->header.seqNum = qToBigEndian(value);
}

void KHeader::setAckNum(quint32 value)
{
    this->header.ackNum = qToBigEndian(value);
}

void KHeader::setOffset(quint8 value)
{
    this->header.offset = qToBigEndian(value);
}

void KHeader::setWindow(quint16 value)
{
    this->header.window = qToBigEndian(value);
}

void KHeader::incrementSeqNum(quint32 value)
{
    this->header.seqNum = qToBigEndian(qFromBigEndian(this->header.seqNum) + value);
}

void KHeader::incrementAckNum(quint32 value)
{
    this->header.ackNum = qToBigEndian(qFromBigEndian(this->header.ackNum) + value);
}

void KHeader::swapNums()
{
    quint32 tmp = this->header.seqNum;
    this->header.seqNum = this->header.ackNum;
    this->header.ackNum = tmp;
}

void KHeader::setSyn(bool value)
{
    if(value)
        this->header.flags |= (1<<1);
    else
        this->header.flags &= (~(1<<1));
}

void KHeader::setAck(bool value)
{
    if(value)
        this->header.flags |= (1<<4);
    else
        this->header.flags &= (~(1<<4));
}

void KHeader::setFin(bool value)
{
    if(value)
        this->header.flags |= (1);
    else
        this->header.flags &= (~(1));
}
