#ifndef HTTPPARAMS_H
#define HTTPPARAMS_H

#include <QObject>
#include <QVariant>
#include <QList>
#include <QPair>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <QUrlQuery>
#endif

class QMyQuery : public QObject
{
    Q_OBJECT
public:
    QMyQuery();
    QMyQuery(const QMyQuery &);
    virtual ~QMyQuery();

    void addQueryItem(const QString &key, const QString &value);
    QList<QPair<QString, QString> > get();

protected:
    QList<QPair<QString, QString> > m_data;

};

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
typedef QMyQuery QUrlQuery;
#endif

class HttpParams : public QObject
{
    Q_OBJECT
protected:
    QList<QPair<QString, QVariant> > m_params;

public:
    HttpParams();
    HttpParams(const QString &key, const QString &value);
    HttpParams(const HttpParams &);
    virtual ~HttpParams();

    HttpParams operator=(const HttpParams&);

    void sort();
    void clear();

    void add(const QString &key, const QVariant &value, bool reset = false);
    QUrlQuery get() const;
    QString toString() const;
    QByteArray post();
};

#endif // HTTPPARAMS_H
