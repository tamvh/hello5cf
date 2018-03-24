#ifndef UTILITY_H
#define UTILITY_H
#include <QObject>
#include <QString>
#include <QNetworkReply>

class QNetworkAccessManager;
class Utility : QObject
{
    Q_OBJECT
public:
    static Utility* getInstance(){
      if (Q_NULLPTR == _instance) {
          _instance = new Utility();
      }
      return _instance;
    }

    explicit Utility(QObject *parent = 0);
    ~Utility();

    QNetworkReply* syncPost(const QString& url, const QByteArray& data);
    QNetworkReply* syncGet(const QString& url);
    static int parseServerResponse(const QString& data, QJsonValue& dt);
public slots:

signals:

private:
    QNetworkAccessManager* _networkMgr;
    static Utility* _instance;
};

#endif // UTILITY_H
