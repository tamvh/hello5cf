#include "wsclient.h"
#include <QtCore/QDebug>
#include <QTimerEvent>
#include "commonfunction.h"

#define PING_TIMER 3000

WSClient::WSClient(const QUrl &wsUrl, const QUrl &httpUrl, const QString mid, const QString devid, bool debug, QObject *parent):
    QThread(parent),
    m_url(wsUrl),
    m_httpUrl(httpUrl),
    m_merchantCode(mid),
    m_deviceId(devid),
    m_debug(debug),
    m_getSession(false),
    timerPing(0)
{
    qDebug() << "wsurl:" << wsUrl;
    qDebug() << "httpurl:" << httpUrl;
    ws_time_disconnect = "";
    connect(&m_webSocket, &QWebSocket::connected, this, &WSClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WSClient::onClosed);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WSClient::textMessageReceived);
    connect(&m_webSocket, &QWebSocket::pong, this, &WSClient::onPong);
    connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(this, &WSClient::reconnect, this, &WSClient::getSession);
}

WSClient::~WSClient()
{
    qDebug() << "close websocket client!";
}

void WSClient::start()
{
    timerPing = startTimer(PING_TIMER);
    m_start = true;
    getSession();
    QThread::start();
}

void WSClient::stop()
{
    m_start = false;
    this->terminate();
}

void WSClient::onPong(quint64 elapsedTime, const QByteArray &payload)
{
    qDebug() << "Receive pong:" << QString(payload);
    pingFlag = 0;
}

void WSClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerPing) {
        qDebug() << "Timer ping event";
        if (pingFlag > 2) {
            if(pingFlag == 3) {
                 emit onCannotRecievePong();
            }
            onClosed();
            return;
        }
        if (m_webSocket.isValid()) {
            qDebug() << "Send ping";
            m_webSocket.ping(QString("ping").toLocal8Bit());
            pingFlag++;
        } else {
            qDebug() << "Websocket not valid";
        }
    }
}

void WSClient::sendTextMessage(QString message)
{
     qint64 sent = m_webSocket.sendTextMessage(message);
     qDebug() << "Sent: " << sent;
}

void WSClient::run()
{
    while (m_start)
    {
        if (m_getSession)
        {
            QThread::sleep(6);
        }
        else
        {
            QThread::sleep(3);
            if (!m_webSocket.isValid())
            {
                qDebug() << "Reconnect";
                emit this->reconnect();
            }
        }
    }
}

void WSClient::onConnected()
{
    qDebug() << "WebSocket connected " << QTime::currentTime().toString("HH:mm:ss.zzz");
    emit connected();
    pingFlag = 0;
}

void WSClient::onTextMessageReceived(QString message)
{
    if (m_debug) {
        qDebug() << "Message received:" << message;
    }
    emit textMessageReceived(message);
}

void WSClient::onClosed()
{
    if (m_debug) {
        qDebug() << "WebSocket disconnected " << QTime::currentTime().toString("HH:mm:ss.zzz");
    }
    m_webSocket.close();
    emit closed();
}

void WSClient::onReConnect()
{
    m_webSocket.open(m_url);
}

void WSClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "WSError: " << error;
}

void WSClient::getSession()
{
    if (!m_getSession) {
        m_getSession = true;
        m_httpSession = new HttpBase(m_httpUrl);
        QObject::connect(m_httpSession, SIGNAL(done(QVariant)), this, SLOT(onSessionDone(QVariant)));
        QObject::connect(m_httpSession, SIGNAL(error(int,QString)), this, SLOT(onSessionError(int,QString)));

        m_httpSession->addParameter("cm", "get_ws_session", true);
        m_httpSession->addParameter("dt", CommonFunction::formatWsSession(m_merchantCode, m_deviceId));
        m_httpSession->process(POST);
    }
}

void WSClient::onSessionDone(const QVariant &data)
{
    m_getSession = false;
    m_httpSession->deleteLater();

    QString sid;
    if( CommonFunction::parseWssid(data.toString(), &sid) == 0 )
    {
        openConnection(sid);
    }
}

void WSClient::onSessionError(const int &errorCode, const QString &errorMessage)
{
    m_getSession = false;
    m_httpSession->deleteLater();
}

void WSClient::openConnection(const QString& sessionID)
{
    QString request = CommonFunction::formatWSConnectString(m_url.toString(),
                                                            m_merchantCode,
                                                            m_deviceId,
                                                            sessionID);
    qDebug() << "Websocket request: " << request;
    m_webSocket.open(QUrl(request));
}
