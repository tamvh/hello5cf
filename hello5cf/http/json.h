#ifndef JSON_H
#define JSON_H

#include <QVariant>

class json
{
public:
    static QVariant byteArrayToVariant(const QByteArray &byteArray);

};

#endif // JSON_H
