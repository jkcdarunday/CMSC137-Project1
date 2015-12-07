#include "kclient.h"

/**
 * @brief KClient::KClient
 * States:
 * 0 - Not Connected
 * 1 - SYN Sent / Connecting
 * 2 - Connected
 */

KClient::KClient()
{

    connect(this, &KClient::readyRead,
            this, &KClient::readPendingDatagrams);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &KClient::resendData);
    this->currentSeqNum = 90;
    this->timeout = 4000;
    this->state=0;
    header = new KHeader();
    srand(time(NULL));
}


void KClient::connectTo(const QHostAddress &address, const quint16 &port)
{
    header->setSeqNum(currentSeqNum);
    header->setSyn(true);
    header->setAck(false);
    this->connectToHost(address, port);
    this->state=0;
    qDebug() << "Client: Connecting to " << address << ":" << port;
    if(this->waitForConnected(timeout)){
        qDebug() << "Client: Connected";
        this->write(header->getByteArray());
        state++;
    }

}

void KClient::sendData(const QByteArray &data)
{
    if(data.size()>0){
        qDebug() << "Adding data..";
        for(int i=0;i<data.size();i+=4)
            toSend.push_back(data.mid(i,4));
    }

    if(state == 2){
        if(currentSeqNum-baseSeqNum < toSend.size()){
            QByteArray frame = toSend[currentSeqNum-baseSeqNum];
            this->write(header->getByteArray() + frame);
            this->timer->start(this->timeout);
        } else {
//            qDebug("client: Sending done.");
        }
    }
}

void KClient::readPendingDatagrams()
{
    bool seqNumIncreased = false; // whether the window moves or not
    while (this->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        this->readDatagram(datagram.data(), datagram.size(),
                           &sender, &senderPort);

        header->readFrom(datagram);

        QByteArray d = datagram;
        QString resHex = "Client : ";
        for (int i = 0; i < d.size(); i++)
            resHex.append( QString::number((quint8)d.at(i), 16).rightJustified(2, '0') + " " );
        //        qDebug() << resHex;

        if(state==1 && header->syn() && header->ack()){
            qDebug() << "Client: SYN+ACK Received";

            header->setSyn(false);
            header->incrementSeqNum();

            header->swapNums();

            currentSeqNum = header->seqNum();
            baseSeqNum = header->seqNum();
            this->writeDatagram(header->getByteArray(), sender, senderPort);

            state++;

            sendData();
        } else if(state==2 && !header->syn() && header->ack()){
            QByteArray data =  datagram.mid(12);
            header->incrementSeqNum();

            header->swapNums();

            if(header->seqNum() > currentSeqNum){
                currentSeqNum = header->seqNum();
                seqNumIncreased = true;
                this->timer->stop();
            }


            if(data.length()>0){
                qDebug() << "client: Got Data" << data;
                int rnum = rand()%20;
                if(rnum < 18)
                    this->writeDatagram(header->getByteArray(), sender, senderPort);
            }
        }

        if(state==2 && seqNumIncreased){
            // Window moves
            // Send data with new Window
            sendData();
        }
    }
}

void KClient::resendData()
{
    qDebug("Client: Sending previous data failed. Resending..");
    this->sendData();
}
