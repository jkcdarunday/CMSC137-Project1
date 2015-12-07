#include "kserver.h"

/**
 * @brief KServer::KServer
 * States:
 * 0 - Not Connected
 * 1 - SYN Received / Connecting
 * 2 - Connected
 */

KServer::KServer()
{

    connect(this, &QUdpSocket::readyRead,
            this, &KServer::readPendingDatagrams);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &KServer::resendData);
    this->currentSeqNum = 10;
    this->timeout = 4000;
    this->state=0;
    this->header = new KHeader();
    srand(time(NULL));
}

bool KServer::listen(quint16 port)
{
    qDebug("Server[%d]: Listening on port %d.", this->state, port);
    return this->bind(QHostAddress::LocalHost, port);
}


void KServer::sendData(const QByteArray &data)
{
    if(data.size()>0){
        qDebug() << "Adding data..";
        for(int i=0;i<data.size();i+=4)
            toSend.push_back(data.mid(i,4));
    }

    if(state == 2){
        qDebug() << "S:" << currentSeqNum << baseSeqNum << currentSeqNum-baseSeqNum ;
        if(currentSeqNum-baseSeqNum < toSend.size()){
            qDebug("S->C: ACK+DAT %d :: S:%d A:%d", needToAck?1:0, header->seqNum(),header->ackNum());
            QByteArray frame = toSend[currentSeqNum-baseSeqNum];
            this->writeDatagram(header->getByteArray() + frame,peer,peerPort);
            this->timer->start(this->timeout);
        } else if(needToAck){
            qDebug("S->C: ACK :: %d", header->seqNum());
            this->writeDatagram(header->getByteArray(),peer,peerPort);
        }
    }
}

void KServer::readPendingDatagrams()
{
    bool seqNumIncreased = false; // whether the window moves or not
    while (this->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        this->readDatagram(datagram.data(), datagram.size(),
                           &sender, &senderPort);

        QByteArray d = datagram;
        QString resHex = "Server: ";
        for (int i = 0; i < d.size(); i++)
            resHex.append( QString::number((quint8)d.at(i), 16).rightJustified(2, '0') + " " );
        qDebug() << resHex;

        header->readFrom(datagram);

        if(state==0 && header->syn() && !header->ack()){
            qDebug("Server[%d]: SYN Recieved.", state);
            header->swapNums();
            header->incrementAckNum();
            header->setSeqNum(currentSeqNum);
            currentSeqNum = header->seqNum();
            currentAckNum = header->ackNum();

            header->setAck(true);
            this->writeDatagram(header->getByteArray(), sender, senderPort);
            state++;
        } else if(state==1 && !header->syn() && header->ack()){
            header->swapNums();
            if(header->seqNum() != currentSeqNum+1 || header->ackNum() != currentAckNum){
                qDebug("Server[%d]: Improper ACK Recieved.", state);
                state = 0;
            } else {
                qDebug("Server[%d]: Proper ACK Recieved.", state);

                peer=sender;
                peerPort=senderPort;

                state++;
                currentSeqNum = header->seqNum();
                baseSeqNum = header->seqNum();

                this->connectToHost(sender, senderPort);
                if(!this->hasPendingDatagrams())
                    sendData();
            }
        } else if(state==2 && !header->syn() && header->ack()){
            QByteArray data =  datagram.mid(12);
            header->incrementSeqNum();
            header->swapNums();

            if(data.size() > 0){
                qDebug() << "\e[1;31mS: ###### Received " << data << "\033[0m";
                needToAck = true;
            }

            if(header->seqNum() > currentSeqNum){
                qDebug("S:21");
                currentSeqNum = header->seqNum();
                this->timer->stop();
            }

            sendData();
            needToAck = false;
        }
    }
}

void KServer::resendData()
{
    qDebug("Server: Sending previous data failed. Resending..");
    this->sendData();
}
