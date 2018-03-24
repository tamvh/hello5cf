#ifndef UDPBRDCASTCLIENT_H
#define UDPBRDCASTCLIENT_H

#include <QObject>
class QTimer;
class QUdpSocket;

#define UDP_BRDCAST_PORT    45459

class UdpBrdcastClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpBrdcastClient(int appid, QObject *parent = 0);

signals:
    void foundServer(const QString &ip, const int &port);

public slots:
    void    startBroadcasting();
    void    ontimeBroadcast();
    void    readyRead();

private:
    int         appId;
    QUdpSocket  *udpSocket;
    QTimer      *brdcastTimer;
    QString     localIp;

    QString     getLocalIp(bool allInterface=true);
    bool        parseIp(const QByteArray &datagram, QString &ip, int &port);
    void        wakeupService();
};

#endif // UDPBRDCASTCLIENT_H
