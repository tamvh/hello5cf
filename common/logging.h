#ifndef LOGGING_H
#define LOGGING_H
#include <QString>
#include <QObject>
#include <http/httpbase.h>

#define URL_LOGGING "https://gbcsyslog.zing.vn/logging"
class Logging : public QObject
{
    Q_OBJECT
public:
    static Logging *getInstance();
    explicit Logging(QObject *parent = 0);
    ~Logging();

    void push_log(const QString &program,
                  const QString &pid,
                  int pri,
                  int level_number,
                  int facility_num,
                  const QString &objid,
                  const QString &objtype,
                  const QString &objname,
                  const QString &cm,
                  const QString &message);
signals:

private slots:

private:
    static Logging *m_instance;
    HttpBase*       m_http_logging;
};

#endif // LOGGING_H
