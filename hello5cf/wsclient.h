#ifndef WSCLIENT_H
#define WSCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QThread>
#include <http/httpbase.h>

#define URL_SYS_LOG "https://gbcsyslog.zing.vn/alert/api/reqalert/?"
class WSClient : public QThread
{
    Q_OBJECT
public:
    explicit WSClient(const QUrl &wsUrl, const QUrl &httpUrl, const QString mid, const QString devid, bool debug=false, QObject *parent = Q_NULLPTR);
    ~WSClient();

    void sendTextMessage(QString message);
    void start();
    void stop();

    void onPong(quint64 elapsedTime, const QByteArray &payload);

    void timerEvent(QTimerEvent *event);
private:
      void run();
      void getSession();
      void openConnection(const QString& sessionID);

signals:
    void connected();
    void textMessageReceived(QString message);
    void closed();
    void reconnect();
    void onCannotRecievePong();

public slots:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onClosed();
    void onReConnect();
    void onSessionDone(const QVariant &data);
    void onSessionError(const int &errorCode, const QString &errorMessage);

public slots:
    void onError(QAbstractSocket::SocketError error);

private:
    QWebSocket  m_webSocket;
    QUrl        m_url;
    QUrl        m_httpUrl;
    QString     m_merchantCode;
    QString     m_deviceId;

    bool        m_debug;
    bool        m_start;
    bool        m_getSession;

    int         timerPing;
    int         pingFlag;
    HttpBase*   m_httpSession;
    QString     ws_time_disconnect;
};

#endif // WSCLIENT_H
