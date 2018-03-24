#include "udpbrdcastclient.h"
#include <QTimer>
#include <QtNetwork>
#include <QJsonObject>
#include <QJsonDocument>

UdpBrdcastClient::UdpBrdcastClient(int appid, QObject *parent)
    : QObject(parent)
    , appId(appid)
{
    localIp = getLocalIp(false);

    int retry = 0;
    while( localIp.isEmpty() && (retry<3) )
    {
        QThread::sleep(1);
        localIp = getLocalIp(false);
        ++retry;
    }

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    brdcastTimer = new QTimer();
    connect(brdcastTimer, SIGNAL(timeout()), this, SLOT(ontimeBroadcast()));

    startBroadcasting();
}

void UdpBrdcastClient::startBroadcasting()
{
    qDebug() << "UdpBrdcastClient::startBroadcasting ... ";

    // wake up service incase the Slaver service is sleeping
    if( !brdcastTimer->isActive() )
    {
        wakeupService();
        brdcastTimer->start(2000);
    }
}

void UdpBrdcastClient::ontimeBroadcast()
{
    static QByteArray datagram = QString("Vpos client IP:%1 (type=%2) detecting center service ... ").arg(localIp).arg(appId).toUtf8();
    udpSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, UDP_BRDCAST_PORT);
}

void UdpBrdcastClient::readyRead()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        QString ip;
        int port;
        if( parseIp(datagram, ip, port) )
        {
            brdcastTimer->stop();

            qDebug() << "Udp Client, found server ip:" << ip << ", port:" << port;

            emit foundServer(ip, port);
        }
    }
}

QString UdpBrdcastClient::getLocalIp(bool allInterface)
{
    QString ip = "";
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        {
            if( ip.isEmpty() )
            {
                ip = address.toString();

                if( allInterface == false ) {
                    break;
                }
            }
            else {
                ip += "," + address.toString();
            }
        }
    }

    return ip;
}

bool UdpBrdcastClient::parseIp(const QByteArray &datagram, QString &ip, int &port)
{
    QJsonDocument jsd = QJsonDocument::fromJson(datagram);
    if( jsd.isObject() )
    {
        QJsonObject jso = jsd.object();
        if( jso.contains("ip")
            && jso.contains("port")
            && jso.contains("srvname")
            && jso["srvname"].toString().compare("Vpos") == 0 )
        {
            ip = jso["ip"].toString();
            port = jso["port"].toInt();

            return true;
        }
    }

    return false;
}

void UdpBrdcastClient::wakeupService()
{
    qDebug() << "wake up service ...";

    QJsonObject jso;
    jso["srvname"]  = "Vpos";
    jso["ip"]       = localIp;
    jso["port"]     = (int)0;
    jso["msg"]      = "wakeup";

    QJsonDocument jsd(jso);

    QByteArray datagram = jsd.toJson(QJsonDocument::Compact);
    udpSocket->writeDatagram(datagram, datagram.size(), QHostAddress::Broadcast, UDP_BRDCAST_PORT);
}
