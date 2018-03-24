#include "httpparams.h"

QMyQuery::QMyQuery():
    m_data(QList<QPair<QString, QString> >())
{
}

QMyQuery::QMyQuery(const QMyQuery &obj):
    m_data(QList<QPair<QString, QString> >())
{
    this->m_data = obj.m_data;
}

QMyQuery::~QMyQuery()
{
}

void QMyQuery::addQueryItem(const QString &key, const QString &value)
{
    m_data.append(QPair<QString, QString>(key, value));
}

QList<QPair<QString, QString> > QMyQuery::get()
{
    return m_data;
}


HttpParams::HttpParams() :
    QObject(), m_params(QList<QPair<QString, QVariant> >())
{
}

HttpParams::HttpParams(const QString &key, const QString &value) :
    QObject(), m_params(QList<QPair<QString, QVariant> >())
{
    this->m_params.append(QPair<QString, QVariant>(key, value));
}

HttpParams::HttpParams(const HttpParams &obj) :
    QObject(), m_params(QList<QPair<QString, QVariant> >())
{
    this->m_params = obj.m_params;
}

HttpParams::~HttpParams()
{
    this->m_params.clear();
}

HttpParams HttpParams::operator=(const HttpParams &obj)
{
    this->m_params = obj.m_params;
    return (*this);
}

void HttpParams::sort()
{
    if (!this->m_params.isEmpty())
    {
        // Save the source
        QList<QPair<QString, QVariant> > source = this->m_params;

        // Get the list of key.
        QList<QString> keys;
        for (int i = 0; i < source.size(); ++i)
        {
            const QPair<QString, QVariant> item = source.at(i);
            keys.append(item.first);
        }

        // Sort the list of keys.
        qSort(keys);

        // Clear the original list.
        this->m_params.clear();

        // Re-add data.
        for (int i = 0; i < keys.size(); ++i)
        {
            const QString key = keys.at(i);
            qint32 index = -1;
            for (int j = 0; j < source.size(); ++j)
            {
                const QPair<QString, QVariant> item = source.at(j);

                if (key == item.first) {
                    this->m_params.append(item);
                    index = j;
                    break;
                }
            }

            if (index >= 0) {
                source.removeAt(index);
            }
        }
    }
}

void HttpParams::clear()
{
    this->m_params.clear();
}

void HttpParams::add(const QString &key, const QVariant &value, bool reset)
{
    if (reset) {
        this->m_params.clear();
    }
    this->m_params.append(QPair<QString, QVariant>(key, value));
}

QUrlQuery HttpParams::get() const
{
    QUrlQuery query;
    for (int i=0; i<this->m_params.size(); i++) {
        const QPair<QString, QVariant> item = this->m_params.at(i);
        query.addQueryItem(item.first, item.second.toString());
    }

    return query;
}

QString HttpParams::toString() const
{
    QString value("");
    for (int i=0; i<this->m_params.size(); i++)
    {
        const QPair<QString, QVariant> item = this->m_params.at(i);
        if (value.isEmpty())
            value += (item.first + "=" + item.second.toString());
        else
            value += "&" + (item.first + "=" + item.second.toString());

    }

    return value;
}

QByteArray HttpParams::post()
{
    return get().toString().toUtf8();
}
