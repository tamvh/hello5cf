#include "cmn.h"

Cmn::Cmn()
{

}


QString Cmn::formatLogging(const QString &program,
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
    QJsonDocument jsonDoc(jsonObj);
    return QString::fromUtf8(jsonDoc.toJson().data());
}
