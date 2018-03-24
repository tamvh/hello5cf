#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QJsonArray>
#include <QEvent>
#include <QProcess>
#include <QSet>
#include "http/httpbase.h"
#include "http/httpbase2.h"
#include "../common/deviceclientservice.h"
#include "../common/logging.h"
#include "wsclient.h"
#include "billdetail.h"

#define LOG_CMD_WS      "ws"
#define LOG_CMD_PRINTER "printer"
class MainController: public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    ~MainController();
    void initialize();
private:
    void read_config();
    void init_ws();
    bool isConnected();
    bool connectToDock(const QString &serverIp);
    void doScanDockAutosetting();
    void printBill(const QString& ivcode, const QString &printData);
    QByteArray createPrintFormat(const QString& fooddata,
                                    const QString &billPreinf="",
                                    const QString &billPostinf="",
                                    const QString &banin="",
                                    bool cutpageSeperate=true);
    void alertEmail(const QString& body);
    void write_log(const QString &cm, const QString& msg);
signals:
    void dockConnectionChange(bool connected);
    void printerReadyChanged(bool ready);
    void cloudConnectionChange(bool connected);

public slots:
    void onCloudConnection();
    void onCloudDisconnection();
    void onNotify(const QString &message);
    void onAlertEmailDone(QVariant data);
    void onAlertEmailError(int err, const QString& msg);
    void onCannotRecievePong();
private:
    HttpBase*           m_httpAlertEmail;
    WSClient            *wsClient;
    DeviceClientService *deviceService;
    QString             deviceId;
    QString             app_name;
    QString             wsTranspro;
    QString             httpTranspro;
    QString             serverurl;
    QString             mc;
    QString             appTitle;
    QString             billPreinf;
    QString             billPostinf;
    QString             sobanin;
    QString             tenbanin;
    QString             printersize;
    QString             dockserver;
    QString             dockversion;
    QString             listemailto;
    QString             listemailcc;
    QString             email_sub;
    QString             device_type;
    QString             branch;
    bool                check_alert_ws_disconnected;
    bool                is_Disconnect;
    QString             log_msg;
};

#endif // MAINCONTROLLER_H
