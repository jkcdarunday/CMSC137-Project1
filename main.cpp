#include "kclient.h"
#include "kheader.h"
#include "kserver.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(" A Data Flow Control Layer Over UDP that Mimics the Transfer Control Protocol (TCP)");
    parser.addHelpOption();
    parser.addPositionalArgument("port", QCoreApplication::translate("main", "Port number"));

    // Option for listen
    QCommandLineOption listenOption("l", QCoreApplication::translate("main", "Specify that program shall listen rather than connect"));
    parser.addOption(listenOption);

    // Drop percent
    QCommandLineOption dropRateOption(QStringList() << "d" << "drop",QCoreApplication::translate("main", "Packet drop rate. 0=0%, 1=25%, 2=50%, 3=75%"), "rate");
    parser.addOption(dropRateOption);

    parser.process(a);

    quint16 port = 9999;

    if(parser.positionalArguments().length()==1){
        bool ok;
        quint16 pn = parser.positionalArguments().at(0).toUShort(&ok);
        if(ok) port=pn;
    }

    QThread *e = new QThread();
    if(parser.isSet("l")){
        KServer *s = new KServer();

        s->moveToThread(e);
        if(parser.isSet("drop")){
            QString dr = parser.value(dropRateOption);
            bool ok;
            quint16 drr = dr.toUShort(&ok);
            if(ok&& drr<4){
                qDebug() << "Drop rate set to" << drr;
                s->setDrop(drr);
            }
        }

        s->listen(port);
    } else {
        KClient *c = new KClient();
        c->connectTo(QHostAddress::LocalHost, port);

       c->moveToThread(e);

        std::string msg;
        std::cout << "Enter message:";
        std::getline(std::cin, msg);

        c->sendData(QString::fromStdString(msg).toUtf8());
    }

    return a.exec();
}
