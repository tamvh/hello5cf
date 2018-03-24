#include "logging.h"
#include "cmn.h"
#include "utility.h"
#include <QMutex>
#include <QDebug>

Logging* Logging::m_instance = NULL;
Logging::Logging(QObject *parent) : QObject(parent)
{
}

Logging::~Logging()
{
}

Logging *Logging::getInstance()
{
    static QMutex mutex;

    if (m_instance == NULL)
    {
        mutex.lock();

        if (m_instance == NULL) {
            m_instance = new Logging();
        }

        mutex.unlock();
    }

    return (m_instance);
}

void Logging::push_log(const QString &program,
              const QString &pid,
              int pri,
              int level_number,
              int facility_num,
              const QString &objid,
              const QString &objtype,
              const QString &objname,
              const QString &cm,
              const QString &message) {
    QJsonObject jsonObj;
    jsonObj["program"] = program;
    jsonObj["pid"] = pid;
    jsonObj["pri"] = pri;
    jsonObj["level_number"] = level_number;
    jsonObj["facility_num"] = facility_num;
    jsonObj["objid"] = objid;
    jsonObj["objtype"] = objtype;
    jsonObj["objname"] = objname;
    jsonObj["cm"] = cm;
    jsonObj["message"] = message;
    QNetworkReply *reply = Utility::getInstance()->syncPost(URL_LOGGING, QJsonDocument(jsonObj).toJson());

    if (Q_NULLPTR == reply) {
        qWarning() << "push log null reply";
    }

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Request push log success";
    }

    delete reply;
}




