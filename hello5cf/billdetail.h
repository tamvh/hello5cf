#ifndef CBILLDETAIL_H
#define CBILLDETAIL_H

#include <QObject>
#include <QList>
#include <QString>
#include <QJsonArray>

typedef struct tagBillInfo {
    QString id;             // sql ID, = QDatetime
    QJsonArray foods;       // food item detail
    QString amount;         // total bill (money)
    QString ivceCode;       // invoice code from server
    QString sesId;          // session id
    QString token;          // token=<devid+amount+session, KEY>

    void Create(const QString& money, const QJsonArray& foodItems);
    quint32 getAmount();

    QString toPrinter(const QString &barcode,
                      const QString &receipt,
                      const QString &zptransid,
                      const QString &datetime,
                      const QString &discount,
                      const QString &payment,
                      const QJsonArray &items,
                      const QJsonObject &bill_note);
} BillInfo;

class CBillDetail
{
public:
    CBillDetail();
    bool checkInvceCode(const QString& invceCode);
    bool getAt(int idx, BillInfo& bi);
    bool getBill(const QString& invoiceCode, BillInfo& bi);
    bool getById(const QString& id, BillInfo& bi, const QString& sesId);
    bool getBySessId(const QString& sesId, BillInfo& bi, const QString &invceCode);

    bool getById(const QString& id, const QString &invceCode, BillInfo& bi);
    bool removeById(const QString &id);
    bool removeBill(const QString& invoiceCode);
    void setToken(const QString& id, const QString& token);
    void addBill(const BillInfo& billInfo);
    void cleanupBill();
    QString getBillInvoiceCode(int idx=0, const QString &invceDefault=QString(""));
private:
    QList<BillInfo> BillList;
};

#endif // CBILLDETAIL_H
