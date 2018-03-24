#ifndef CONFIGSETTING_H
#define CONFIGSETTING_H

#include <QObject>

class ConfigSetting : public QObject
{
    Q_OBJECT
public:
    static ConfigSetting *instance();
    explicit ConfigSetting(bool autosave = false, QObject *parent = 0);
    ~ConfigSetting();

    void Load();
    void Save();
    void setDocServer(const QString& docsrv);
    void setDockVersion(const QString &ver);
    void setPrintersize(const QString &pagesize);
    void setSobanin(int num);
    void setTenbanin(const QString &ten);
    void setBillPreinf(const QString &info);
    void setBillPostinf(const QString &info);
    void setUserenableConfig(bool enableconf);
    void setUsername(const QString &uname, const QString &upwd);
    void saveRebootDatetime(int type);
    void setPrinter(const QString &prnName);
    void setAppName(const QString& appname);
    void setAppTitle(const QString& appTitle);
    bool setPaymentName(const QString& pmName);
    void setTransProtocol(const QString transpro);
    void setBillItemFontsize(const int fntsize);

    void setListEmail(const QString &l_email);
    void setListPhone(const QString &l_phone);

    QString transProtocol;  // http / https
    QString appName;        // "canteenvng", "vpos", ...
    QString appTitle;       // "CĂN TIN"
    QString printerName;    // "xpriner58"
    QString listemail;
    QString listphone;

    QString paymentName;    // "104.199.170.120", "payment.vng.com.vn"
    QString dockServer;     // "192.168.1.1"
    QString dockVersion;    // DockServerService version; 1.0 is original, 2.0 support extent command
    QString printerSize;    // small/large
    int     sobanin;        // so luong ban in 1..n
    QString tenbanin;       // Prefix-sobanin
    QString billPreinf;     // Bill prefix sau Title "Đ/c: 182 Lê Đại Hành"
    QString billPostinf;    // Bill postfix
    bool    userEnableConf;


    int     rebootDate;     // sunday=1,monday=2 ... saturday=7
    int     rebootTime;     // time to reboot from 00:00 - 23:59
    int     billItemFontsize;

    bool    networkDevice;

private:
    static ConfigSetting *m_instance;

    QString getTitle_version1(const QString &appname);
    bool    m_autoSave;

signals:

public slots:
};

#endif // CONFIGSETTING_H
