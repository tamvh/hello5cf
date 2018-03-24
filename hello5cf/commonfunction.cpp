#include "commonfunction.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDataStream>
#include <QByteArray>
#include <QDebug>
#include <QtEndian>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

CommonFunction::CommonFunction()
{

}

QString CommonFunction::gVPOS1()
{
    QString s;
    s.reserve(32);

    // 7VShsAFE3S4pS3lijpCkIxCDpzi7ljdS

    s.append('7');
    s.append('V');
    s.append('S');
    s.append('h');
    s.append('s');
    s.append('A');
    s.append('F');
    s.append('E');

    s.append('3');
    s.append('S');
    s.append('4');
    s.append('p');
    s.append('S');
    s.append('3');
    s.append('l');
    s.append('i');

    s.append('j');
    s.append('p');
    s.append('C');
    s.append('k');
    s.append('I');
    s.append('x');
    s.append('C');
    s.append('D');

    s.append('p');
    s.append('z');
    s.append('i');
    s.append('7');
    s.append('l');
    s.append('j');
    s.append('d');
    s.append('S');

    return s;
}

QString CommonFunction::formatLogin(const QString &usrname, const QString &pwd)
{
    QJsonObject jso;
    jso["u"] = usrname;
    jso["p"] = pwd;

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

QString CommonFunction::formatPayDone(const QString &invce, const QString &devid, int type, const QString &appid)
{
    // cấu trúc msg gửi lên server báo đã nhận được payment thành công,
    // và đã in hóa đơn thành công
    //{
    //    "type":1000,
    //    "dt":{
    //        "invoice_code":"1607010410120041",
    //        "devid": "123456789"
    //        "appuser":"vpos"
    //    }
    //}

    QJsonObject dt;
    dt["invoice_code"] = invce;
    dt["devid"] = devid;
    dt["appuser"] = appid;

    QJsonObject msg;
    msg["type"] = type;
    msg["dt"] = dt;

    QJsonDocument jsdoc(msg);

    return QString::fromUtf8(jsdoc.toJson().data());
}

// reject/cancel invoice
QString CommonFunction::formatInvoiceCancel(const QString& invoiceCode, const QString &appUser)
{
    QString invce = invoiceCode;

    QJsonObject jsonObj;
    jsonObj["merchant_code"] = appUser;
    jsonObj["invoice_code"] = invce.replace("#", "");

    QJsonDocument jsonDoc(jsonObj);

    return QString::fromUtf8(jsonDoc.toJson().data());
}

QString CommonFunction::formatRequestFoods(const QString &appUser)
{
    QJsonObject jsonObj;
    jsonObj["merchant_code"] = appUser;
    jsonObj["item_status"] = 1; // get enable food items only

    QJsonDocument jsonDoc(jsonObj);

    return QString::fromUtf8(jsonDoc.toJson().data());
}


QString CommonFunction::formatAlertEmail(
        const QString &device_type,
        const QString &list_emailto,
        const QString &list_emailcc,
        const QString &subject,
        const QString &branch,
        const QString &body) {
    QJsonObject jsonObj;
    QList<QString> q_listto = list_emailto.split(";");
    QList<QString> q_listcc = list_emailcc.split(";");
    QJsonArray arr_listcc;
    QJsonArray arr_listto;
    for(int i = 0; i<q_listto.size(); i++) {
        arr_listto.append(q_listto.at(i));
    }
    for(int i = 0; i<q_listcc.size(); i++) {
        arr_listcc.append(q_listcc.at(i));
    }
    jsonObj["type"] = "email";
    jsonObj["subject"] = subject;
    jsonObj["listcc"] = arr_listcc;
    jsonObj["listto"] = arr_listto;
    jsonObj["contentplaintext"] = "";
    jsonObj["contenthtml"] = CommonFunction::formatHtml(device_type, branch, body);
    QJsonDocument jsonDoc(jsonObj);
    return QString::fromUtf8(jsonDoc.toJson().data());
}

QString CommonFunction::formatRequestGetStaffInfo(const QString& appUser,const QString& cardCode)
{
    QJsonObject jsonObj;
    jsonObj["merchant_code"] = appUser;
    jsonObj["card_code"] = cardCode;

    QJsonDocument jsonDoc(jsonObj);

    return QString::fromUtf8(jsonDoc.toJson().data());
}

QString CommonFunction::formatHtml(const QString &device_type, const QString &branch, const QString &body) {
    QString content;
    content = "<!DOCTYPE html>";
    content += "<html>";
    content += "<body>";
    content += "    <p>Hi all,</p>";
    content += "    <p>Hệ thống phát sinh sự kiện, vui lòng xem nội dung.</p>";
    content += "    <div>- Loại thiết bị: " + device_type + "</div>";
    content += "    <div>- Chi nhánh: " + branch + "</div>";
    content += "    <div>- Nội dung: " + body + "</div> </br>";
    content += "    <div>Thanks all</div>";
    content += "    <div>GBC Team.</div>";
    content += "</body>";
    content += "</html>";
    return content;
}

int CommonFunction::parseErrcode(const QString &respone, QString &msg, QString *merchantId)
{
    QJsonDocument   jsd = QJsonDocument::fromJson(respone.toUtf8());
    QJsonObject     jso = jsd.object();

    if (jso.contains("msg")) {
        msg = jso["msg"].toString();
    }

    if (merchantId && jso.contains("dt") == true)
    {
        QJsonObject dt = jso["dt"].toObject();
        if (dt.contains("merchant_code")) {
            *merchantId = dt["merchant_code"].toString();
        }
    }

    if (jso.contains("err")) {
        return jso["err"].toInt();
    }

    return -1;
}

int CommonFunction::parsePmsid(const QString &respone, QString *msg, QString *sid, QString *biid)
{
    int err = -1;

    QJsonDocument   jsd = QJsonDocument::fromJson(respone.toUtf8());
    QJsonObject     jso = jsd.object();

    if (jso.contains("err")) {
        err = jso["err"].toInt();
    }

    if (msg && jso.contains("msg")) {
        *msg = jso["msg"].toString();
    }

    if (jso.contains("dt") == true)
    {
        QJsonObject jdt = jso["dt"].toObject();

        if (sid && jdt.contains("invoice_session") == true) {
            *sid = jdt["invoice_session"].toString();
        }

        if (biid && jdt.contains("food_order") == true) {
            *biid = jdt["food_order"].toString();
        }
    }

    return err;
}

int CommonFunction::parseWssid(const QString &respone, QString *sid)
{
    int err = -1;

    QJsonDocument   jsd = QJsonDocument::fromJson(respone.toUtf8());
    QJsonObject     jso = jsd.object();

    if (jso.contains("err")) {
        err = jso["err"].toInt();
    }

    if( (err == 0) && (jso.contains("dt") == true) )
    {
        QJsonObject jdt = jso["dt"].toObject();

        if (sid && jdt.contains("ws_session") == true) {
            *sid = jdt["ws_session"].toString();
        }
    }
    else {
        err = -1;
    }

    return err;
}

QString CommonFunction::formatRequestRecharge(const QString& appUser,const QString& cardCode, const QString& amount, const QString &machineName, const QString& sid)
{
    QJsonObject jsonObj;
    jsonObj["merchant_code"] = appUser;
    jsonObj["card_code"] = cardCode;
    jsonObj["amount"] = amount;
    jsonObj["machine_name"] = machineName;
    jsonObj["token"] = "token";
    QJsonDocument jsonDoc(jsonObj);
    return QString::fromUtf8(jsonDoc.toJson().data());
}

QString CommonFunction::formatMoney(int moneyValue)
{
    long long absVal = abs(moneyValue);
    QString money = QString::number(absVal);
    int loop = money.length() / 3;
    if (loop > 0)
    {
        if (money.length() % 3 == 0)
        {
            loop = loop - 1;
        }

        int index = money.length();
        for (int i = 0; i < loop ; i++)
        {
            index = index - 3;
            money.insert(index,  QString(","));
        }
    }

    if (moneyValue < 0)
        money.insert(0,  QString("-"));

    return money;
}

QString CommonFunction::formatMoney(const QString& money)
{
    QString m = money;
    m = m.replace(QString(","), QString::null);
    return formatMoney(m.toInt());
}

QString CommonFunction::formatRequestCreateInvoice(const QString &merchantCode,
                                                const QString &machineName,
                                                long amount,
                                                const QString& deviceId,
                                                const QString& biId,
                                                const QJsonArray &billItems,
                                                const QString &labTitle,
                                                   int paymentMethod)
{
    QJsonObject jsonObj;
    jsonObj["merchant_code"] = merchantCode;
    jsonObj["foodsorder_id"] = biId;
    jsonObj["machine_name"] = machineName;
    jsonObj["description"] = QString("%1").arg(machineName);
    jsonObj["appuser"] = merchantCode;
    jsonObj["amount"] = QString::number(amount);
    jsonObj["devid"] = deviceId;
    jsonObj["items"] = billItems;
    jsonObj["payment_method"] = QString::number(paymentMethod);

    QJsonDocument jsonDoc(jsonObj);
    QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

    return QString::fromUtf8(data.data());
}

QString CommonFunction::formatSession(const QString& merchantCode, const QString& devId, const QString& biId)
{
    QJsonObject jso;
    jso["merchant_code"] = merchantCode;
    jso["food_order"] = biId;
    jso["devid"] = devId;

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

QString CommonFunction::formatWsSession(const QString& merchantCode, const QString& devId)
{
    QJsonObject jso;
    jso["merchant_code"] = merchantCode;
    jso["devid"] = devId;

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

QString CommonFunction::formatCheckin(const QString &cardId)
{
    QJsonObject jso;
    jso["card_code"] = cardId;

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

QString CommonFunction::formatLogin(const QString &id, const QString &u, const QString &p)
{
    QJsonObject jso;
    jso["merchant_code"] = id;
    jso["u"] = u;
    jso["p"] = QString( QCryptographicHash::hash(QByteArray(p.toStdString().data()), QCryptographicHash::Md5).toHex() );

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

QString CommonFunction::formatBaocao(const QString &id, int bcType, const QString &datefrom, const QString &dateto)
{
    QJsonObject jso;
    jso["merchant_code"] = id;
    jso["current_page"] = 1;
    jso["total_trans_per_page"] = 100;
    jso["payment_date_time_from"] = datefrom;
    jso["payment_date_time_to"] = dateto;
    if (bcType != 0) {
        jso["report_z"] = bcType;
    }

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

QString CommonFunction::formatBroadcast(const QString &msg)
{
    QJsonObject jso;
    jso["msg"]  = "broadcast";
    jso["data"] = msg;

    QJsonDocument jsd(jso);
    return QString::fromUtf8( jsd.toJson(QJsonDocument::Compact) );
}

int CommonFunction::parseResponseZalopay(const QString& response, QString &msg,
                                         QString& qrcode, QString* token, quint16* appId,
                                         QString* ivceCode, QString *ivceSess, QString* ivceDate,
                                         QString* oripay, int* discount, QString* discpay)
{
    int err = -1;

    QJsonDocument   jsd = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject     jso = jsd.object();

    if (jso.contains("err")) {
        err = jso["err"].toInt();
    }

    if (jso.contains("msg")) {
        msg = jso["msg"].toString();
    }

    if (jso.contains("dt") == true)
    {
        QJsonObject jsonDt = jso["dt"].toObject();

        QJsonDocument jsdQr(jsonDt);
        qrcode = QString::fromUtf8(jsdQr.toJson().data());

        if (token && jsonDt.contains("zptranstoken") == true) {
            *token = jsonDt["zptranstoken"].toString();
        }

        if (appId && jsonDt.contains("appid") == true) {
            *appId = jsonDt["appid"].toInt();
        }

        if (ivceCode && jsonDt.contains("invoice_code") == true) {
            *ivceCode = jsonDt["invoice_code"].toString();
        }

        if (ivceSess && jsonDt.contains("invoice_session") == true) {
            *ivceSess = jsonDt["invoice_session"].toString();
        }

        if (ivceDate && jsonDt.contains("invoice_datetime") == true) {
            *ivceDate = jsonDt["invoice_datetime"].toString();
        }

        if (oripay && jsonDt.contains("amount") == true) {
            *oripay = formatMoney(jsonDt["amount"].toInt());
        }

        if (discount && jsonDt.contains("discount") == true) {
            *discount = jsonDt["discount"].toInt();
        }

        if (discpay && jsonDt.contains("payment") == true) {
            *discpay = formatMoney(jsonDt["payment"].toInt());
        }
    }

    return err;
}

QString CommonFunction::generateQRCode(const QString &token, int appId)
{
    QJsonObject jso;
    jso["zptranstoken"] = token;
    jso["appid"] = appId;

    QJsonDocument jsd(jso);

    return QString::fromUtf8(jsd.toJson().data());
}

int CommonFunction::parseResponsePayCash(const QString &respone, QString *msg,
                                     QString *ivceCode, QString *ivceTime,
                                     QString *barcode, QString *balance)
{
    int err = -1;
    static int invcode = 0;

    QJsonDocument   jsd = QJsonDocument::fromJson(respone.toUtf8());
    QJsonObject     jso = jsd.object();

    if (jso.contains("err")) {
        err = jso["err"].toInt();
    }

    if (msg && jso.contains("msg")) {
        *msg = jso["msg"].toString();
    }

    if (jso.contains("dt") == true)
    {
        QJsonObject jsonDt = jso["dt"].toObject();

        if (ivceCode && jsonDt.contains("invoice_code") == true) {
            *ivceCode = jsonDt["invoice_code"].toString();
        }
        else {
            *ivceCode = QString("%1").arg(invcode++, 10, 10, QChar('0'));
        }

        if (ivceTime && jsonDt.contains("invoice_datetime") == true) {
            *ivceTime = jsonDt["invoice_datetime"].toString();
        }

        if (barcode && jsonDt.contains("barcode") == true) {
            *barcode = jsonDt["barcode"].toString();
        }

        if (balance && jsonDt.contains("balance") == true) {
            *balance = QString::number(jsonDt["balance"].toInt());
        }
    }

    return err;
}

QByteArray CommonFunction::parseDataToReaderDevice(quint32 amount, quint16 appId, const QString &token)
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    // Header
    dataStream << (qint8)0xA0;

    // Len
    dataStream << (qint8)0x1C; //28

    // Cmd
    dataStream << (qint8)0x01;

    // SeqID
    dataStream << (qint8)0x01;

    // Direction Flag
    dataStream << (qint8)0x07;

    //uchar outAmount[4];
    //qToLittleEndian<quint32>(amount, outAmount);

    //dataStream << outAmount;
    dataStream << amount;
    //dataArray.append((char*)outAmount,4);
    //qDebug() << "Amount data: " << dataArray.toHex();

    //uchar outAppId[2];
    //qToLittleEndian<quint16>(amount, outAppId);
    //dataStream << outAppId;
    dataStream << appId;
    //dataArray.append((char*)outAppId,2);
    //qDebug() << "AppId data: " << dataArray.toHex();
    //qDebug() << "Token:" << QByteArray::fromBase64(token.toLocal8Bit(), QByteArray::Base64UrlEncoding).toHex();

    //dataStream << QByteArray::fromBase64(token.toUtf8(), QByteArray::Base64UrlEncoding);

    dataArray.append(QByteArray::fromBase64(token.toLocal8Bit(), QByteArray::Base64UrlEncoding));

    QByteArray dataArray2;
    QDataStream dataStream2(&dataArray2, QIODevice::WriteOnly);
    dataStream2.setByteOrder(QDataStream::LittleEndian);
    //quint16 checksum = deviceDataChecksum(dataArray);
    quint16 checksum = makeCrc16Kermit(dataArray);
    dataStream2 << checksum;

    dataStream2 << (qint8)0xFF;

    dataArray.append(dataArray2);
    //qDebug() << "Compose data: " << dataArray.toHex() << " checksum " << checksum << " lenght " << QByteArray::fromBase64(token.toLocal8Bit(), QByteArray::Base64UrlEncoding).length();
    return dataArray;
}

QByteArray CommonFunction::parseDataToRemoteReaderDevice(quint32 amount, quint16 appId, const QString &token)
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    // Header
    //dataStream << (qint8)0xA0;

    // Len
    //dataStream << (qint8)0x1C; //28

    // Cmd
    //dataStream << (qint8)0x01;

    // SeqID
    //dataStream << (qint8)0x01;

    // Direction Flag
    dataStream << (qint8)0x07;

    dataStream << amount;
    //qDebug() << "Amount data: " << dataArray.toHex();

    dataStream << appId;
    //dataArray.append((char*)outAppId,2);
    //qDebug() << "AppId data: " << dataArray.toHex();
    //qDebug() << "Token:" << QByteArray::fromBase64(token.toLocal8Bit(), QByteArray::Base64UrlEncoding).toHex();

    //dataStream << QByteArray::fromBase64(token.toUtf8(), QByteArray::Base64UrlEncoding);

    dataArray.append(QByteArray::fromBase64(token.toLocal8Bit(), QByteArray::Base64UrlEncoding));

    QByteArray dataArray2;
    QDataStream dataStream2(&dataArray2, QIODevice::WriteOnly);
    dataStream2.setByteOrder(QDataStream::LittleEndian);
    //quint16 checksum = deviceDataChecksum(dataArray);
    quint16 checksum = makeCrc16Kermit(dataArray.right(dataArray.length()-1));
    dataStream2 << checksum;

    //dataStream2 << (qint8)0xFF;

    dataArray.append(dataArray2);
    //qDebug() << "Compose data: " << dataArray.toHex() << " checksum " << checksum << " lenght " << QByteArray::fromBase64(token.toLocal8Bit(), QByteArray::Base64UrlEncoding).length();
    return dataArray;
}

QByteArray CommonFunction::parseCmdStopToReaderDevice()
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray, QIODevice::WriteOnly);

    // Header
    dataStream << (qint8)0xA0;

    // Len
    dataStream << (qint8)0x03; //28

    // Cmd
    dataStream << (qint8)0x02;

    // SeqID
    dataStream << (qint8)0x01;

    // Checksum
    dataStream << (qint8)0xFF;

    return dataArray;
}

quint16 CommonFunction::deviceDataChecksum(const QByteArray& data)
{
    if (data.length() % 2 != 0)
    {
        return 0;
    }

    quint16 checksum = 0;
    quint16 value;


    for (int i = 0; i < data.length(); i+=2)
    {
        QDataStream dataStream(data.mid(i, 2));
        dataStream.setByteOrder(QDataStream::LittleEndian);

        dataStream >> value;

        //qDebug() << "Data value: " << value;
        checksum += value;
    }

    return checksum;
}

quint16 CommonFunction::makeCrc16Kermit(const QByteArray &data)
{
    quint16         valuehex = 0;
    quint16         CRC = 0;
    int             size = data.size();
    const quint8    *pData  = reinterpret_cast<const quint8*>(data.constData());

    CRC = 0;
    for(int i=0; i<size; ++i) {
        valuehex = ((*pData ^ CRC) & 0x0fu) * 0x1081;
        CRC >>= 4;
        CRC ^= valuehex;
        valuehex = ( (*pData >> 4) ^ (CRC & 0x00ffu) ) & 0x0fu;
        CRC >>= 4;
        CRC ^= (valuehex * 0x1081u);

        ++pData;
    }
    quint16 ret = ( (CRC & 0x00ffu) << 8) | ((CRC & 0xff00u) >> 8);
    return ret;
}

QString CommonFunction::getNotifyValue(const QString& msg, const QString& key)
{
    QJsonObject jsonMain;
    QJsonDocument jsonDoc;

    jsonDoc = QJsonDocument::fromJson(QByteArray(msg.toUtf8()));
    jsonMain = jsonDoc.object();

    return jsonMain[key].toString();
}

bool CommonFunction::parseNotifyInfo(const QString &notify, QString *dt, QString *token)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray(notify.toUtf8()));
    QJsonObject jsonMain = jsonDoc.object();

    if( jsonMain.empty() )
        return false;

    if( jsonMain.contains("dt") ) {
        if( dt ) {
            *dt = jsonMain["dt"].toString();
        }
    }

    if( jsonMain.contains("token") ) {
        if( token ) {
            *token = jsonMain["token"].toString();
        }
    }

    return true;
}

bool CommonFunction::parseNotifyInfo(const QString &responeMsg,
                                     int *n_type,
                                     QString* ivceCode,
                                     QString* zptransid,
                                     QString* ivceTime,
                                     QString *barcode,
                                     QString *discount,
                                     QString *amount,
                                     QString *payment,
                                     QJsonArray* items,
                                     QJsonObject* bill_note)
{
    // this function parsing Zalo payment infos: invoice_code, invoice_datetime
    // responeMsg: respone when Zalo do-pay successfully and send msg to vpos-server, and then forward to client

    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray(responeMsg.toUtf8()));
    QJsonObject jsonMain = jsonDoc.object();
    if(jsonMain.contains("type") == true) {
        *n_type = jsonMain["type"].toInt();
    } else {
        *n_type = -1;
        return false;
    }

    if (jsonMain.contains("dt") == true)
    {
        QJsonObject jsonDt = jsonMain["dt"].toObject();
        if( jsonDt.empty() )
        {
            jsonDt = QJsonDocument::fromJson(QByteArray(jsonMain["dt"].toString().toUtf8())).object();
            if( jsonDt.empty() ) {
                return false;
            }
        }

        if(jsonDt.contains("note") == true) {
            if(bill_note) {
                *bill_note =  QJsonDocument::fromJson(QByteArray(jsonDt["note"].toString().toUtf8())).object();//jsonDt["note"].toObject();
            }
        }

        if (jsonDt.contains("invoice_code") == true) {
            if (ivceCode) {
                *ivceCode = jsonDt["invoice_code"].toString();
            }
        }
        else {
            return false;
        }

        if(jsonDt.contains("zptransid") == true) {
            if(zptransid) {
                *zptransid = jsonDt["zptransid"].toString();
            }
        } else {
            return false;
        }

        if (jsonDt.contains("barcode") == true) {
            if (barcode) {
                *barcode = jsonDt["barcode"].toString();
            }
        }
        else {
            return false;
        }

        if (ivceTime && jsonDt.contains("invoice_datetime") == true) {
            *ivceTime = jsonDt["invoice_datetime"].toString();
        }

        if (discount && jsonDt.contains("discount") == true) {
            *discount = QString::number(jsonDt["discount"].toInt());
        }

        if (payment && jsonDt.contains("payment") == true)
        {
            if (jsonDt["payment"].isDouble()) {
                *payment = QString::number(jsonDt["payment"].toDouble());
            }
            else {
                *payment = QString::number(jsonDt["payment"].toInt());
            }
        }

        if (jsonDt.contains("amount") == true)
        {
            if (jsonDt["amount"].isDouble()) {
                *amount = QString::number(jsonDt["amount"].toDouble());
            }
            else {
                *amount = QString::number(jsonDt["amount"].toInt());
            }
        }
        else {
            *amount = *payment;
        }

        if(jsonDt.contains("items") == true)
        {
            *items = jsonDt["items"].toArray();
        }
        else {
            return false;
        }

        return true;
    }

    return false;
}

QString CommonFunction::ViettoLatin(const QString& strIn)
{
    QString strOut;
    strOut=strIn;
    //strOut=strOut.toLower();
    strOut=strOut.replace(QRegExp("à|á|ạ|ả|ã|â|ầ|ấ|ậ|ẩ|ẫ|ă|ằ|ắ|ặ|ẳ|ẵ"),"a");
    strOut=strOut.replace(QRegExp("À|Á|Ạ|Ả|Ã|Ă|Ằ|Ắ|Ặ|Ẳ|Ẵ|Â|Ấ|Ầ|Ậ|Ẩ|Ẫ"),"A");
    strOut=strOut.replace(QRegExp("è|é|ẹ|ẻ|ẽ|ê|ề|ế|ệ|ể|ễ"),"e");
    strOut=strOut.replace(QRegExp("Ể|Ế|Ệ|Ể|Ê|Ễ|É|È|Ẻ|Ẽ|Ẹ"),"E");
    strOut=strOut.replace(QRegExp("ì|í|ị|ỉ|ĩ"),"i");
    strOut=strOut.replace(QRegExp("Ì|Í|Ị|Ỉ|Ĩ"),"I");
    strOut=strOut.replace(QRegExp("ò|ó|ọ|ỏ|õ|ô|ồ|ố|ộ|ổ|ỗ|ơ|ờ|ớ|ợ|ở|ỡ"),"o");
    strOut=strOut.replace(QRegExp("Ò|Ó|Ọ|Ỏ|Õ|Ơ|Ờ|Ớ|Ợ|Ở|Ỡ|Ô|Ố|Ổ|Ộ|Ồ|Ỗ"),"O");
    strOut=strOut.replace(QRegExp("ù|ú|ụ|ủ|ũ|ư|ừ|ứ|ự|ử|ữ"),"u");
    strOut=strOut.replace(QRegExp("Ừ|Ứ|Ự|Ử|Ư|Ữ|Ù|Ú|Ụ|Ủ|Ũ"),"U");
    strOut=strOut.replace(QRegExp("ỳ|ý|ỵ|ỷ|ỹ"),"y");
    strOut=strOut.replace(QRegExp("Ỳ|Ý|Ỵ|Ỷ|Ỹ"),"y");
    strOut=strOut.replace(QRegExp("đ"),"d");
    strOut=strOut.replace(QRegExp("Đ"),"D");
    return strOut;
}

void CommonFunction::getLastUpdateTime(const QString &qstrData, qulonglong& qullLUT){
    QJsonDocument jsonDoc;
    QJsonObject jsonObjMain;

    jsonDoc = QJsonDocument::fromJson(QByteArray(qstrData.toUtf8()));
    jsonObjMain = jsonDoc.object();

    qullLUT = jsonObjMain["dt"].toObject()["last_update"].toDouble();
}

QString CommonFunction::updatePrintBillTime(const QString& billDetail, const QString& datetime, const QString &barcode)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray(billDetail.toUtf8()));
    QJsonObject jsonMain = jsonDoc.object();
    jsonMain["datetime"] = datetime;
    jsonMain["barcode"] = barcode;

    QJsonDocument outjsonDoc(jsonMain);
    return QString::fromUtf8(outjsonDoc.toJson().data());
}

QString CommonFunction::formatWSConnectString( const QString& url,
                                    const QString& merchantCode,
                                    const QString& deviceId,
                                    const QString& wsSession)
{
    QString request, token;

    token = HMacSha256HexStringEncode(merchantCode + deviceId + wsSession, gVPOS1());
    request = url + "?appuser=" + merchantCode + "&devid=" + deviceId + "&ws_session=" + wsSession + "&tk=" + token + "&apptype=1";

    return request;
}

QString CommonFunction::HMacSha256HexStringEncode(const QString& data, const QString& key)
{
    return QMessageAuthenticationCode::hash(data.toUtf8(), key.toUtf8(), QCryptographicHash::Sha256).toHex();
}
