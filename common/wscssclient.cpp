#include "wscssclient.h"
#include <QDebug>
#include <QThread>

WsCssClient::WsCssClient(int fromId, int toId, QObject *parent) :
    QObject(parent),
    m_url("")
  , isConnected(false)
  , m_id(fromId)
  , m_toId(toId)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WsCssClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WsCssClient::onClosed);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WsCssClient::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::pong, this, &WsCssClient::pong);

    brdcastClient = new UdpBrdcastClient(fromId);
    connect(brdcastClient, SIGNAL(foundServer(QString,int)), this, SLOT(onServer(QString,int)));

    connect(&m_ppTimer, SIGNAL(timeout()), this, SLOT(ping()));
}

WsCssClient::~WsCssClient()
{
    qDebug() << "~WsCssClient";

    if( m_webSocket.isValid() ) {
        m_webSocket.close();
    }
}

void WsCssClient::onServer(const QString &ips, const int &port)
{
    qDebug() << "Ws Client, found server center ip=" << ips << ", port=" << port;

    QStringList iplist = ips.split(",");

    if( iplist.count() > 0 )
    {
        m_url = QString("ws://%1:%2").arg(iplist[0]).arg(port);
        m_webSocket.open(QUrl(m_url));

        emit foundServer(ips, port);
    }
}

void WsCssClient::onConnected()
{
    qDebug() << "onConnected()";

    m_webSocket.sendTextMessage( initJson(m_id, m_toId) );
    isConnected = true;

    // gửi tất cả những msg đến CSS khi có kết nối
    while( (m_msgs.size() > 0) && isConnected )
    {
        QString msg = m_msgs.at(0);

        int sentcount = m_webSocket.sendTextMessage( msg );

        if( sentcount >= msg.length() ) {
            m_msgs.removeAt(0);
        }

        QThread::msleep(10);
    }

    m_ppTimer.start(WSS_PINGPONG_TIMER);
    m_ppCounter = 0;

    emit connected();
}

void WsCssClient::ping()
{
    if( isConnected ) {
        if( ++m_ppCounter > 3 ) {
            m_webSocket.close(QWebSocketProtocol::CloseCodeAbnormalDisconnection);
        }
        else {
            m_webSocket.ping();
        }
    }
}

void WsCssClient::pong(quint64, const QByteArray &)
{
    --m_ppCounter;
}

void WsCssClient::onClosed()
{
    qDebug() << "onClosed()";

    isConnected = false;
    m_ppTimer.stop();

    brdcastClient->startBroadcasting();

    emit disconnected();
}

void WsCssClient::onReconnect()
{
    qDebug() << "onReconnect()";

    m_webSocket.open( QUrl(m_url) );
}

void WsCssClient::onTextMessageReceived(QString message)
{
    qDebug() << "Received:" << message;

#ifdef KITCHENVIEW

    if( QString::compare(message, "CONNECTED") != 0 ) {
        emit orderNumber(jsonData(message));
    }

#else

    emit textMessageReceived(message);

#endif
}

QString WsCssClient::initJson(const int fromId, const int toId)
{
    QJsonObject jso;
    jso["action"] = packet_content_type::CLIENT_LOGIN;
    jso["appid"] = fromId;
    jso["to_appid"] = toId;

    QJsonDocument doc(jso);
    return doc.toJson(QJsonDocument::Compact);
}

QVariant WsCssClient::jsonData(QString number)
{
    QJsonObject jso;
    bool ck = false;
    jso["number"] = number;
    for (int i = 0; i < aJson.size(); ++i)
    {
        if(QJsonValue(jso) == (aJson.at(i)))
        {
            ck = true;
            break;
        }
    }
    if(!ck) {
        aJson.insert(0, jso);
    }

    return aJson;
}

void WsCssClient::sendMsg(const QString &msg)
{
    QJsonObject jso;
    jso["action"]   = packet_content_type::CHAT_TEXT_MSG;
    jso["appid"]    = m_id;
    jso["to_appid"] = m_toId;
    jso["data"]     = msg;

    QJsonDocument jdoc(jso);
    QByteArray data = jdoc.toJson(QJsonDocument::Compact);

    if( isConnected ) {
        m_webSocket.sendTextMessage( data );
    }
    else {
        qDebug() << "ws is not connected !";

        // store to msgs list to send later
        m_msgs << QString::fromUtf8(data);
    }
}

void WsCssClient::sendTextMessage(const QString &msg)
{
    if( isConnected ) {
        qDebug() << "WsCssClient::sendTextMessage, msg:" << msg;
        m_webSocket.sendTextMessage(msg);
    }
    else {
        qDebug() << "ws is not connected !";

        // store to msgs list to send later
        m_msgs << msg;
    }
}
