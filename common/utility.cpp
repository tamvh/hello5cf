#include "utility.h"
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QEventLoop>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QProcess>

Utility* Utility::_instance = Q_NULLPTR;
Utility::Utility(QObject *parent)
    : QObject(parent)
{
    _networkMgr = new QNetworkAccessManager();
}

Utility::~Utility() {
    if (_networkMgr)
            delete _networkMgr;
}

QNetworkReply* Utility::syncPost(const QString &url, const QByteArray &data)
{
    QSslConfiguration conf;
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    // Setup url
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setSslConfiguration(conf);

    QEventLoop eventLoop;
    QNetworkReply* reply = _networkMgr->post(request, data);
    qDebug() << "Setup connection with reply";
    connect(reply,SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    qDebug() << "Request reply finish";

    return reply;
}

QNetworkReply *Utility::syncGet(const QString &url)
{
    QSslConfiguration conf;
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);

    // Setup url
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setSslConfiguration(conf);

    QEventLoop eventLoop;
    QNetworkReply* reply = _networkMgr->get(request);
    connect(reply,SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    return reply;
}

int Utility::parseServerResponse(const QString &data, QJsonValue &dt)
{
    QJsonParseError perror;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(), &perror);
    if (QJsonParseError::NoError != perror.error) {
        qWarning() << "Have error while parse json response " << perror.errorString();
        return -1;
    }

    QJsonObject res = doc.object();
    if (res.isEmpty()) {
        qWarning() << "Response object empty ";
        return -1;
    }

    // Check error code
    int err = res["err"].toInt();
    if (err != 0) {
        qWarning() << "Response error from server " << err;
        return -1;
    }

    dt = res["dt"];

    return 0;
}

