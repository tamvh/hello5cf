#include <QCryptographicHash>
#include <QStringList>
#include "httptool.h"

//#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonDocument>

QVariant HttpTool::byteArrayToVariant(const QByteArray &byteArray)
{
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray, &error);
    if (error.error != QJsonParseError::NoError)
    {
        QJsonDocument json = QJsonDocument::fromJson(QString(byteArray).toUtf8(), &error);
        if (error.error != QJsonParseError::NoError)
        {
            QString respone(byteArray);

            if (respone.contains('{'))
            {
                QVariantMap map;

                respone.remove('{');
                respone.remove('}');
                respone.remove('\n');
                QStringList list = respone.split(QRegExp("(\\;|\\,)"));
                for (int i=0; i<list.count(); i++)
                {
                    QStringList pair = list.at(i).split(':');
                    if (pair.count() == 2)
                    {
                        map.insert(pair[0].trimmed(), pair[1]);
                    }
                }

                if (map.size() > 0) {
                    return map;
                }

                return respone;
            }
            else
            {
                return respone.remove("\n");
            }
        }

        return json.toVariant();
    }

    return jsonDoc.toVariant();
}

//#else

//#include "QJsonValue.h"
//#include "QJsonDocument.h"
//#include "QJsonArray.h"
//#include "QJsonObject.h"
//#include "QJsonParseError.h"

//QVariant HttpTool::byteArrayToVariant(const QByteArray &byteArray)
//{
//    QJsonParseError e;
//    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray, &e);
//    if (!jsonDoc.isNull())
//    {
//        return jsonDoc.toVariant();
//    }
//    else
//    {
//        QJsonDocument json = QJsonDocument::fromJson(QString(byteArray).toUtf8(), &e);
//        if (!json.isNull())
//        {
//            return json.toVariant();
//        }
//        else
//        {
//            QString respone(byteArray);

//            if (respone.contains('{'))
//            {
//                QVariantMap map;

//                respone.remove('{');
//                respone.remove('}');
//                respone.remove('\n');
//                QStringList list = respone.split(QRegExp("(\\;|\\,)"));
//                for (int i=0; i<list.count(); i++)
//                {
//                    QStringList pair = list.at(i).split(':');
//                    if (pair.count() == 2)
//                    {
//                        map.insert(pair[0].trimmed(), pair[1]);
//                    }
//                }

//                if (map.size() > 0) {
//                    return map;
//                }

//                return respone;
//            }
//            else
//            {
//                return respone.remove("\n");
//            }
//        }
//    }

//    return QVariant(byteArray);
//}

//#endif

//HttpTool::HttpTool()
//{
//}

//QString HttpTool::getJsonString(const QVariantMap& data, const QString& key)
//{
//    QString result("");

//    if (data.contains(key)) {
//        QVariant value = data[key];
//        // Check the valid of value.
//        if (value.isValid()) {
//            // Check the type of value.
//            if (value.type() == QVariant::String) {
//                result = value.value<QString>();
//            } else {
//                // If type of value is not string, try to convert to string.
//                result = value.toString();
//            }
//        }
//    }

//    return (result);
//}

//qint32 HttpTool::getJsonInteger(const QVariantMap& data, const QString& key)
//{
//    qint32 result = -1;

//    if (data.contains(key)) {
//        QVariant value = data[key];
//        // Check the valid of value.
//        if (value.isValid()) {
//            // Check the type of value.
//            if (value.type() == QVariant::Int) {
//                result = value.value<qint32>();
//            } else {
//                // If type of value is not string, try to parse the string data.
//                result = value.toString().toInt();
//            }
//        }
//    }

//    return (result);
//}

//bool HttpTool::getJsonBoolean(const QVariantMap& data, const QString& key)
//{
//    bool result = false;

//    if (data.contains(key)) {
//        QVariant value = data[key];
//        // Check the valid of value.
//        if (value.isValid()) {
//            // Check the type of value.
//            if (value.type() == QVariant::Bool) {
//                result = value.value<bool>();
//            }
//        }
//    }

//    return (result);
//}

//QVariantList HttpTool::getJsonArray(const QVariantMap& data, const QString& key)
//{
//    QVariantList result = QVariantList();

//    if (data.contains(key)) {
//        QVariant value = data[key];
//        // Check the valid of value.
//        if (value.isValid()) {
//            // Check the type of value.
//            if (value.type() == QVariant::List) {
//                result = value.value<QVariantList>();
//            }
//        }
//    }

//    return (result);
//}

//QVariantMap HttpTool::getJsonObject(const QVariantMap& data, const QString& key)
//{
//    QVariantMap result = QVariantMap();

//    if (data.contains(key)) {
//        QVariant value = data[key];
//        // Check the valid of value.
//        if (value.isValid()) {
//            // Check the type of value.
//            if (value.type() == QVariant::Map) {
//                result = value.value<QVariantMap>();
//            }
//        }
//    }

//    return (result);
//}

QString HttpTool::createMd5(const QString &s)
{
    return QString( QCryptographicHash::hash(QByteArray(s.toStdString().data()),
                                            QCryptographicHash::Md5).toHex() );
}

//void HttpTool::writelog(QString key, QVariant v, QString blank)
//{
//    Logger::debug(QString("%1%2:<<\n").arg(blank).arg(key));
//    writelog(v, blank+"\t");
//}

//void HttpTool::writelog(QVariant v, QString blank)
//{
//    if (v.type() == QVariant::Map)
//    {
//        writelog(v.toMap(), blank+"\t");
//    }
//    else if (v.type() == QVariant::List)
//    {
//        writelog(v.toList(), blank+"\t");
//    }
//    else
//    {
//        Logger::debug(QString("%1%2").arg(blank).arg(v.toString()));
//    }
//}

//void HttpTool::writelog(QVariantMap m, QString blank)
//{
//    QStringList keys = m.keys();
//    foreach (QString key, keys)
//    {
//        switch (m.value(key).type())
//        {
//        case QVariant::Map:
//        case QVariant::List:
//            writelog(key, m.value(key), blank+"\t");
//            break;
//        default:
//            Logger::debug(QString("%1%2:\t%3").arg(blank).arg(key).arg(m.value(key).toString()));
//        }
//    }
//}

//void HttpTool::writelog(QVariantList l, QString blank)
//{
//    foreach (QVariant v, l)
//    {
//        switch (v.type())
//        {
//        case QVariant::Map:
//            writelog(v.toMap(), blank+"\t");
//            break;
//        case QVariant::List:
//            writelog(v.toList(), blank+"\t");
//            break;
//        default:
//            Logger::debug(QString("%1%2").arg(blank).arg(v.toString()));
//        }
//    }
//}
