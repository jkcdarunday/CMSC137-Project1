#include "kclient.h"
#include "kheader.h"
#include "kserver.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    KServer *s = new KServer();
    s->listen(6666);

    KClient *c = new KClient();
    c->connectTo(QHostAddress::LocalHost, 6666);

//    c->sendData(QString("Hello World 2012-10053 Allahu Akbar 2012-10053").toUtf8());

    s->sendData(QString("1342132413241234132412341234234xxxx").toUtf8());

//    KHeader *s = new KHeader();
//    s->setSyn(true);
//    s->setAck(false);
//    QByteArray d = s->getByteArray();

//    KHeader *y = new KHeader(d);

//    d=y->getByteArray();

//    qDebug() << d.size();

//    QString resHex = "";
//    for (int i = 0; i < d.size(); i++)
//      resHex.append( QString::number((quint8)d.at(i), 16).rightJustified(2, '0') + " " );

//    qDebug() << resHex;

    return a.exec();
}
