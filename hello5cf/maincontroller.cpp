#include "maincontroller.h"
#include "commonfunction.h"
#include "../common/prntdataformat.h"
#include "../common/configsetting.h"
#include <QDebug>
#include <QUrl>
#include <QThread>
#include <QMessageBox>
#include <QtConcurrent>
#include <QNetworkInterface>
#include <QGuiApplication>
#include <QJsonObject>
#include <QSet>
#include <QFile>
#include <QIODevice>

MainController::MainController(QObject *parent) :
    QObject(parent),
    deviceService(Q_NULLPTR),
    check_alert_ws_disconnected(false),
    is_Disconnect(false),
    wsClient(0)
{
}

MainController::~MainController()
{
}

void MainController::initialize() {
    qDebug() << "Start Service!";
    read_config();
    deviceId = QString(QUuid::createUuid().toRfc4122().toHex());
    if (dockserver.isEmpty()) {
        qDebug() << "Found dock server config: " << dockserver;
        connectToDock(dockserver);
    } else {
        qDebug() << "Not found dock server config, do auto scan Dock ...";
        doScanDockAutosetting();
    }
    init_ws();
}

void MainController::read_config() {
    QString cf;
    //QFile *file = new QFile("/usr/local/qt5pi/vpos/config");
    QFile *file = new QFile("/home/tamvh/Documents/xxx/hello5cf/config");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QByteArray line;
    while (!file->atEnd()) {
        line.append(file->readLine().trimmed());
    }
    cf = QString::fromStdString(line.toStdString());
    QJsonDocument   jsd = QJsonDocument::fromJson(cf.toUtf8());
    QJsonObject     jso = jsd.object();
    if(jso.contains("app_name")) {
        app_name = jso["app_name"].toString();
    } else {
        app_name = "";
    }

    if(jso.contains("server")) {
        serverurl = jso["server"].toString();
    } else {
        serverurl = "";
    }

    if (jso.contains("mc")) {
        mc = jso["mc"].toString();
    } else {
        mc = "";
    }

    if (jso.contains("httptrans")) {
        httpTranspro = jso["httptrans"].toString();
    } else {
        httpTranspro = "";
    }

    if (jso.contains("wstrans")) {
        wsTranspro = jso["wstrans"].toString();
    } else {
        wsTranspro = "";
    }

    if (jso.contains("apptitle")) {
        appTitle = jso["apptitle"].toString();
    } else {
        appTitle = "";
    }

    if (jso.contains("billpreinf")) {
        billPreinf = jso["billpreinf"].toString();
    } else {
        billPreinf = "";
    }

    if (jso.contains("billPostinf")) {
        billPostinf = jso["billpostinf"].toString();
    } else {
        billPostinf = "";
    }

    if (jso.contains("sobanin")) {
        sobanin = jso["sobanin"].toInt();
    } else {
        sobanin = 1;
    }

    if (jso.contains("tenbanin")) {
        tenbanin = jso["tenbanin"].toString();
    } else {
        tenbanin = ";";
    }

    if (jso.contains("printersize")) {
        printersize = jso["printersize"].toString();
    } else {
        printersize = "";
    }

    if (jso.contains("dockversion")) {
        dockversion = jso["dockversion"].toString();
    } else {
        dockversion = "";
    }

    if (jso.contains("dockserver")) {
        dockserver = jso["dockserver"].toString();
    } else {
        dockserver = "";
    }

    if(jso.contains("listemailto")) {
        listemailto = jso["listemailto"].toString();
    } else {
        listemailto = "";
    }

    if(jso.contains("listemailcc")) {
        listemailcc = jso["listemailcc"].toString();
    } else {
        listemailcc = "";
    }

    if(jso.contains("email_sub")) {
        email_sub = jso["email_sub"].toString();
    } else {
        email_sub = "";
    }

    if(jso.contains("device_type")) {
        device_type = jso["device_type"].toString();
    } else {
        device_type = "";
    }

    if(jso.contains("branch")) {
        branch = jso["branch"].toString();
    } else {
        branch = "";
    }

    file->close();
}

void MainController::init_ws() {

    if(wsClient )
    {
        wsClient->stop();
        wsClient->wait(10000);

        delete wsClient;
        wsClient = 0;
    }

    if (wsClient == 0)
    {
        QUrl wsUrl = QUrl(wsTranspro + "://" + serverurl + "/ntf/");
        qDebug() << "wsUrl: " << wsUrl;
        QUrl httpUrl = QUrl(httpTranspro + "://" + serverurl + "/api/common/");
        qDebug() << "httpUrl: " << httpUrl;
        wsClient = new WSClient(wsUrl, httpUrl, mc, deviceId, true, 0);
        QObject::connect(wsClient, SIGNAL(textMessageReceived(QString)), this, SLOT(onNotify(const QString&)));
        QObject::connect(wsClient, SIGNAL(connected()), this, SLOT(onCloudConnection()));
        QObject::connect(wsClient, SIGNAL(closed()), this, SLOT(onCloudDisconnection()));
        QObject::connect(wsClient, SIGNAL(onCannotRecievePong()), this, SLOT(onCannotRecievePong()));
        wsClient->start();
    }
}

void MainController::onCannotRecievePong() {
    log_msg =  "Không thể nhận tín hiệu PONG";
    this->alertEmail(log_msg);
    this->write_log(LOG_CMD_WS, log_msg);
}

void MainController::onCloudConnection() {
    check_alert_ws_disconnected  = false;
    if(is_Disconnect) {
        log_msg =  "WebSocket reconnected " + QTime::currentTime().toString("HH:mm:ss.zzz");
        is_Disconnect = false;
    } else {
        log_msg =  "WebSocket connected " + QTime::currentTime().toString("HH:mm:ss.zzz");
    }
    this->write_log(LOG_CMD_WS, log_msg);
    this->alertEmail(log_msg);

    emit cloudConnectionChange(true);
}

void MainController::onCloudDisconnection() {
    qDebug() << "onCloudDisconnection: " << check_alert_ws_disconnected;
    is_Disconnect = true;
    if(check_alert_ws_disconnected == false) {
        log_msg =  "WebSocket disconnected " + QTime::currentTime().toString("HH:mm:ss.zzz");
        this->write_log(LOG_CMD_WS, log_msg);
    }
    emit cloudConnectionChange(false);
}

void MainController::alertEmail(const QString& body) {
    m_httpAlertEmail = new HttpBase(QString(""),this);
    QObject::connect(m_httpAlertEmail, SIGNAL(done(QVariant)), this, SLOT(onAlertEmailDone(QVariant)), Qt::UniqueConnection);
    QObject::connect(m_httpAlertEmail, SIGNAL(error(int,QString)), this, SLOT(onAlertEmailError(int, QString)), Qt::UniqueConnection);

    m_httpAlertEmail->setUrl(QUrl(URL_SYS_LOG));
    m_httpAlertEmail->addParameter("cm", "alert", true);
    m_httpAlertEmail->addParameter("dt", CommonFunction::formatAlertEmail(device_type, listemailto, listemailcc, email_sub, branch, body));
    m_httpAlertEmail->process();
}

void MainController::onAlertEmailDone(QVariant data) {
    qDebug() << "onAlertEmailDone: " << data.toString();
    check_alert_ws_disconnected = true;
}

void MainController::onAlertEmailError(int err, const QString& msg) {
    qDebug() << "onAlertEmailError, err: " + QString::number(err) + ", msg: " + msg;
}

void MainController::onNotify(const QString &message) {
    qDebug() << "onNotify msg: " << message;
    QString invceCode, zptransid, invceDate, barcode, discount, amount, payment;
    QJsonArray items;
    QJsonObject bill_note;
    int notify_type;
    BillInfo bi;
    if(CommonFunction::parseNotifyInfo(message, &notify_type, &invceCode, &zptransid, &invceDate, &barcode, &discount, &amount, &payment, &items, &bill_note)) {
        if(notify_type == 1) {
            // tạo nội dung xuất ra máy in
            QString printdata = bi.toPrinter(barcode,
                                             invceCode,
                                             zptransid,
                                             invceDate,
                                             discount,
                                             payment,
                                             items,
                                             bill_note);
            // print data
            printBill(invceCode, printdata);
        }
    }
}

void MainController::printBill(const QString& ivcode, const QString &printData)
{
    qDebug() << "printData: " + printData;

    if (deviceService)
    {
        QString fooddata = CommonFunction::ViettoLatin(printData);
        qDebug() << "fooddata: " + fooddata;

        if (dockversion.compare("1.0") == 0) {
            // original print server service
            qDebug() << "print in dock version 1.0";
            deviceService->print(QString(""), ivcode, fooddata, DeviceClient::VPOS_Payment);
        }
        else
        {
            // format print data for Dock Print Server 2.0
            if (sobanin < 2)
            {
                qDebug() << "print in dock verson 2.0, sobanin < 2";
                QByteArray prntcode = createPrintFormat(fooddata, billPreinf, billPostinf);
                QString prntdata = Prntdataformat::makePrintDataFromPrncode(prntcode);
                deviceService->print(QString(""), ivcode, prntdata, DeviceClient::VPOS_Payment);
            }
            else
            {
                qDebug() << "print in dock verson 2.0, sobanin >= 2";
                QByteArray prntcode;
                QStringList namelist = tenbanin.split(";");

                for (int i=0; i<sobanin; i++)
                {
                    QString namebanin;

                    if (i < namelist.size()) {
                        namebanin = QString("#%1 %2").arg(i+1).arg(namelist[i]);
                    } else {
                        namebanin = QString("#%1").arg(i+1);
                    }

                    prntcode = createPrintFormat(fooddata, billPreinf, billPostinf, namebanin, (i+1)==sobanin);
                    QString prntdata = Prntdataformat::makePrintDataFromPrncode(prntcode);
                    deviceService->print(QString(""), ivcode, prntdata, DeviceClient::VPOS_Payment);
                }
            }
        }
    }
}

QByteArray MainController::createPrintFormat(const QString& fooddata,
                                             const QString &billPreinf,
                                             const QString &billPostinf,
                                             const QString &banin,
                                             bool cutpageSeperate)
{
    int PAGESZ = 33;
    int TAB1, TAB2, TAB3, TAB4;

    if (printersize == "small") {
        PAGESZ = 33;
        TAB1 = 10;
        TAB2 = 12;
        TAB3 = 23;
        TAB4 = 33;
    }
    else {
        PAGESZ = 48;
        TAB1 = 12;
        TAB2 = 27;
        TAB3 = 38;
        TAB4 = 48;
    }

    QJsonDocument jsd = QJsonDocument::fromJson(fooddata.toUtf8());
    if (!jsd.isObject()) {
        return "";
    }

    QJsonObject jso = jsd.object();

    Prntdataformat pdf;

    pdf.codeFontType(pdf.font_A);
    pdf.codeFontBold(true);
    pdf.codeLine(appTitle, pdf.Justify_center);
    pdf.codeFontBold(false);

    if ( !billPreinf.isEmpty() ) {
        QStringList infs = billPreinf.split(";");
        for (int i=0; i<infs.length(); i++) {
            pdf.codeLine(infs.at(i));
        }
        //pdf.codeLine("Tang 5-TTTM Flemington");
        //pdf.codeLine("184 Le Dai Hanh, P.15, Q.11");
    }
    pdf.codeLine(banin, pdf.Justify_center);

    pdf.codeFeedline();
    //QString vposInvoice = jso["receiptNum"].toString();
    QString zptransid = jso["zptransid"].toString();
    QString localtion = jso["location"].toString();
    QString customer_name = jso["customer_name"].toString();
    QString notice = jso["bill_note"].toString();
    QString phone_no = jso["phone_no"].toString();
    pdf.codeLine(QString("Hoa don   :\t%1").arg(zptransid), pdf.Justify_left, TAB1);
    pdf.codeLine(QString("Ten KH    :\t%1").arg(customer_name), pdf.Justify_left, TAB1);
    pdf.codeLine(QString("DT        :\t%1").arg(phone_no), pdf.Justify_left, TAB1);
    pdf.codeLine(QString("Vi tri    :\t%1").arg(localtion), pdf.Justify_left, TAB1);
    pdf.codeLine(QString("Ngay      :\t%1").arg(jso["datetime"].toString()), pdf.Justify_left, TAB1);

    pdf.codeChars('-', PAGESZ, pdf.Justify_center);
    pdf.codeText("TEN", pdf.Justify_left);
    pdf.codeText("\tSL", pdf.Justify_none, TAB2);
    pdf.codeText("\tD.GIA", pdf.Justify_right, TAB3);
    pdf.codeLine("\tT.TIEN", pdf.Justify_right, TAB4);
    pdf.codeChars('-', PAGESZ, pdf.Justify_center);

    // ...
    if (jso["item"].isArray())
    {
        QJsonArray receiptItem = jso["item"].toArray();
        for (int i = 0; i < receiptItem.size(); i++)
        {
            QJsonObject item = receiptItem[i].toObject();

            QString name;

            if(QString::compare(item["notetext"].toString().trimmed(), "", Qt::CaseInsensitive) == 0) {
                name = item["item_name"].toString();
            }
            else {
                name = item["item_name"].toString() + " ("+ item["notetext"].toString() +")";
            }
            int isBold = QString::compare(name, "com trua", Qt::CaseInsensitive);
            if (isBold == 0) {
                pdf.emphasized_on();
                name = name.toUpper();
                pdf.prntUnderline(Prntdataformat::udl_1dot);
            }

            if (name.length() >= TAB2)
                pdf.codeLine(name, pdf.Justify_left);
            else
                pdf.codeText(name, pdf.Justify_left);

            if (isBold == 0) {
                pdf.prntUnderline(Prntdataformat::udl_none);
            }

            pdf.codeText("\t" + item["quantity"].toString(), pdf.Justify_left, TAB2);
            pdf.codeText("\t" + item["price"].toString(), pdf.Justify_right, TAB3);
            pdf.codeLine("\t" + item["amount"].toString(), pdf.Justify_right, TAB4);
            if(i < receiptItem.size() - 1) {
                pdf.codeChars(' ', PAGESZ, pdf.Justify_left);
            }
            if (isBold == 0) {
                pdf.emphasized_off();
            }
        }
    }

    pdf.codeChars('-', PAGESZ, pdf.Justify_left);

    QString total = "\t" + jso["total"].toString();

    if (jso["ext"].isArray()) {
        pdf.codeLine(QString("TONG:%1").arg(total), pdf.Justify_left, TAB4-total.length());
    }
    else {
        pdf.emphasized_on();
        pdf.codeLine(QString("TONG:%1").arg(total), pdf.Justify_left, TAB4-total.length());
        pdf.emphasized_off();
    }

    // Extended field
    if (jso["ext"].isArray())
    {
        QJsonArray extItems = jso["ext"].toArray();
        for (int i = 0; i < extItems.size(); i++)
        {
            QJsonObject item = extItems[i].toObject();

            QString name = item["name"].toString();
            QString value = "\t" + item["value"].toString();

            if( (i+1) == extItems.size() ) {
                pdf.codeChars('-', PAGESZ, pdf.Justify_left);
            }

            if( (i+1) < extItems.size() )
            {
                pdf.codeLine(QString("%1%2").arg(name).arg(value), pdf.Justify_left, TAB4-value.length());
            }
            else
            {
                pdf.emphasized_on();
                pdf.codeLine(QString("%1%2").arg(name).arg(value), pdf.Justify_left, TAB4-value.length());
                pdf.emphasized_off();
            }
        }
    }
    // Extended field

    pdf.codeChars('-', PAGESZ, pdf.Justify_left);
    pdf.codeFeedline(1);

    pdf.codeLine(QString("%1%2").arg("(*) Ghi chu: ").arg(notice), pdf.Justify_left, 80);

    pdf.codeBarcode(jso["barcode"].toString(), 2, 80, pdf.Justify_center);
    pdf.codeFeedline(1);

    if ( !billPostinf.isEmpty() ) {
        QStringList infs = billPostinf.split(";");
        for (int i=0; i<infs.length(); i++) {
            pdf.codeLine(infs.at(i));
        }
        //pdf.codeLine("Cam on quy khach");
        //pdf.codeLine("Hen gap lai");
    }

    pdf.codeFeedline(PAGESZ < 40 ? 2 : 1);

    if (!cutpageSeperate) {
        pdf.codeChars('-', PAGESZ, pdf.Justify_left);
        pdf.codeFeedline(2);
    }
    else {
        pdf.codeFeedline(0);
    }

    pdf.codeCutpage(cutpageSeperate);

    return pdf.getPrintDeviceCode();
}

bool MainController::isConnected()
{
    if (deviceService && deviceService->isConnected()) {
        return true;
    }

    return false;
}

bool MainController::connectToDock(const QString &serverIp) {
    if (Q_NULLPTR == deviceService) {
        deviceService = new DeviceClientService();
        connect(deviceService, &DeviceClientService::connectionChanged, [=](int iConnect) {
            if (iConnect == DeviceClient::Connected) {
                log_msg = "Đã kết nối Dock";
                this->write_log(LOG_CMD_PRINTER, log_msg);
                emit dockConnectionChange(true);
            } else {
                log_msg = "Không thể kết nối tới Dock";
                this->write_log(LOG_CMD_PRINTER, log_msg);
                emit dockConnectionChange(false);
            }
        });

        connect(deviceService, &DeviceClientService::printerReadyChanged, [=](bool ready) {
            emit printerReadyChanged(ready);
            if(ready) {
                log_msg = "Máy in sẵn sàng";
            } else {
                log_msg = "Máy in chưa sẵn sàng";
            }
            this->write_log(LOG_CMD_PRINTER, log_msg);
        });
    }

    deviceService->startService(serverIp);
    return true;
}

void MainController::doScanDockAutosetting() {
    if (!isConnected())
    {
        QList<QHostAddress> listServer;
        DeviceClient::discoverDeviceServerBroadcast(listServer);

        foreach (const QHostAddress& address, listServer)
        {
            QString srvIp = address.toString();
            connectToDock(srvIp);

            break;
        }
    }
}

void MainController::write_log(const QString &command, const QString &msg) {
    QString program     = app_name;
    int level_number    = 3;
    int facility_num    = 1;
    QString pid         = "123";
    int pri             = facility_num* 8 + level_number;
    QString objid       = mc;
    QString objtype     = device_type;
    QString objname     = branch;
    QString cm          = command;
    QString message     = msg;
    Logging::getInstance()->push_log(program, pid, pri, level_number, facility_num, objid, objtype, objname, cm, message);
}
