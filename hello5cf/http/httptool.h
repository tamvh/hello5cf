#ifndef HTTPTOOL_H
#define HTTPTOOL_H

#include <QVariant>
#include <QByteArray>

class HttpTool
{
public:
    HttpTool();

    static QVariant     byteArrayToVariant(const QByteArray &byteArray);

    static QString      getJsonString   (const QVariantMap& data, const QString& key);
    static qint32       getJsonInteger  (const QVariantMap& data, const QString& key);
    static bool         getJsonBoolean  (const QVariantMap& data, const QString& key);
    static QVariantList getJsonArray    (const QVariantMap& data, const QString& key);
    static QVariantMap  getJsonObject   (const QVariantMap& data, const QString& key);

    static QString      createMd5(const QString &s);

    static void writelog(QString key, QVariant v, QString blank="");
    static void writelog(QVariant v, QString blank="");
    static void writelog(QVariantMap m, QString blank="");
    static void writelog(QVariantList l, QString blank="");
};

#endif // HTTPTOOL_H
