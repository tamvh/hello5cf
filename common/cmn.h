#ifndef CMN_H
#define CMN_H
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

class Cmn
{
public:
    Cmn();
    static QString formatLogging(const QString &program,
                                 const QString &pid,
                                 int pri,
                                 int level_number,
                                 int facility_num,
                                 const QString &objid,
                                 const QString &objtype,
                                 const QString &objname,
                                 const QString &cm,
                                 const QString &message);
};

#endif // CMN_H
