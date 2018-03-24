#include "json.h"
#include <QStringList>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonDocument>

QVariant json::byteArrayToVariant(const QByteArray &byteArray)
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

#else

#include "QJsonValue.h"
#include "QJsonDocument.h"
#include "QJsonArray.h"
#include "QJsonObject.h"
#include "QJsonParseError.h"

QVariant json::byteArrayToVariant(const QByteArray &byteArray)
{
    QJsonParseError e;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray, &e);
    if (!jsonDoc.isNull())
    {
        return jsonDoc.toVariant();
    }
    else
    {
        QJsonDocument json = QJsonDocument::fromJson(QString(byteArray).toUtf8(), &e);
        if (!json.isNull())
        {
            return json.toVariant();
        }
        else
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
    }

    return QVariant(byteArray);
}

#endif
