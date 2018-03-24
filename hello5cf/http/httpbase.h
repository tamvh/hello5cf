#ifndef HTTPBASE_H
#define HTTPBASE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVariant>
#include <QList>
#include <QPair>
#include "httpparams.h"

typedef enum
{
    HTTP,
    HTTPS
} HTTP_TYPE;

typedef enum
{
    GET,
    POST,
    PUT
} HTTP_PROCESS;

typedef enum
{
    CONTENT_TYPE_NONE = 0,
    CONTENT_TYPE_FORM = 1,
    CONTENT_TYPE_JSON = 2,
    CONTENT_TYPE_TEXT_XML = 3,
} CONTENT_TYPE;

class HttpBase : public QObject
{
    Q_OBJECT
protected:
    QNetworkAccessManager   m_nam;
    QUrl                    m_url;
    HttpParams              m_param;
    QString                 m_agentname;
    QString                 m_username;
    QString                 m_password;
    int                     m_contenttype;
    QHash<QString, QString> m_header;

private:
    QNetworkReply           *m_reply;

public:
    HttpBase(const QUrl &url=QString(""), QObject *parent = NULL);
    ~HttpBase();

    void            setContentType(CONTENT_TYPE contentType);
    void            setUrl(const QUrl &url);
    void            setAgentname(const QString &agentname);
    void            setParameter(const HttpParams &params);
    void            addParameter(const QString &key, const QVariant &value, bool reset = false);
    void            setUsernamePassword(const QString &username, const QString &password);
    void            addHeader(const QString& key, const QString& value);

    void            cancel();

protected:
    QNetworkReply*  get();
    QNetworkReply*  post(const QByteArray& postData = QByteArray());
    QNetworkReply*  put(QIODevice *iostream);
    QNetworkRequest createRequest(CONTENT_TYPE contentType = CONTENT_TYPE_NONE, const HTTP_PROCESS processType = GET);

signals:
    void    done(const QVariant& outputObject);
    void    error(const int &errorCode, const QString &errorMessage);

public slots:
    QNetworkReply* process(const HTTP_PROCESS processType = GET, QIODevice *iostream = NULL, const QByteArray &postData = QByteArray());
    void    finished();

};

#endif // HTTPBASE_H
