#include <QMutex>
#include <QSettings>
#include <QDebug>
#include "configsetting.h"

ConfigSetting* ConfigSetting::m_instance = NULL;

ConfigSetting::ConfigSetting(bool autosave, QObject *parent) : QObject(parent)
{
    m_autoSave = autosave;
    Load();
}

ConfigSetting::~ConfigSetting()
{
    if (m_autoSave) {
        Save();
    }
}

ConfigSetting *ConfigSetting::instance()
{
    static QMutex mutex;

    if (m_instance == NULL)
    {
        mutex.lock();

        if (m_instance == NULL) {
            m_instance = new ConfigSetting();
        }

        mutex.unlock();
    }

    return (m_instance);
}

#include <QStandardPaths>
void ConfigSetting::Load()
{
    QSettings sets("vng", "vpos");

    appName         = sets.value(QString("option/appname"), QString("")).toString();
    appTitle        = sets.value(QString("option/apptitle"), QString("")).toString();
    if (appTitle.isEmpty()) {
        appTitle = getTitle_version1(appName); // support title of privious-version
    }

    paymentName     = sets.value(QString("option/payment"),         QString("gbcstaging.zing.vn/vpos")).toString();
    transProtocol   = sets.value(QString("option/transferprotocol"),QString("https")).toString();
    printerName     = sets.value(QString("option/printer"),         QString("xpriner58")).toString();
    dockServer      = sets.value(QString("option/dock"),            QString("192.168.1.1")).toString();
    dockVersion     = sets.value(QString("option/dockversion"),     QString("2.0")).toString();
    printerSize     = sets.value(QString("option/printersize"),     QString("large")).toString();
    sobanin         = sets.value(QString("option/sobanin"), 1).toInt();
    tenbanin        = sets.value(QString("option/tenbanin"),        QString("Lien Bep;Lien Chinh;Lien Khach")).toString();
    billPreinf      = sets.value(QString("option/billpreinf"),      QString("TTTM FLEMINGTON;184 Le Dai Hanh, P.15, Q.11")).toString();
    billPostinf     = sets.value(QString("option/billpostinf"),     QString("Cam on Quy khach, hen gap lai")).toString();
    userEnableConf  = sets.value(QString("option/userenableconf"),  true).toBool();
    rebootDate      = sets.value(QString("option/rebootdate"),      0).toInt();
    rebootTime      = sets.value(QString("option/reboottime"),      3).toInt();
    networkDevice   = sets.value(QString("option/networkdevice"),   true).toBool();
    billItemFontsize = sets.value(QString("option/billitemfontsize"), 14).toInt();
    listemail       = sets.value(QString("option/listemail"),       QString("info@vng.com.vn")).toString();
    listphone       = sets.value(QString("option/listphone"),       QString("0935954858")).toString();

    if( m_autoSave && !sets.contains("option/appname") )
    {
        Save();
    }
}

QString ConfigSetting::getTitle_version1(const QString &appname)
{
    QString s = appname;
    return s;
}

void ConfigSetting::Save()
{
    QSettings sets("vng", "vpos");

    sets.setValue(QString("option/appname"), appName);
    sets.sync();
    sets.setValue(QString("option/apptitle"), appTitle);
    sets.sync();
    sets.setValue(QString("option/printer"), printerName);
    sets.sync();
    sets.setValue(QString("option/payment"), paymentName);
    sets.sync();
    sets.setValue(QString("option/vposserver"), dockServer);
    sets.sync();
    sets.setValue(QString("option/dock"), dockServer);
    sets.sync();
    sets.setValue(QString("option/dockversion"), dockVersion);
    sets.sync();
    sets.setValue(QString("option/printersize"), printerSize);
    sets.sync();
    sets.setValue(QString("option/sobanin"), sobanin);
    sets.sync();
    sets.setValue(QString("option/tenbanin"), tenbanin);
    sets.sync();
    sets.setValue(QString("option/billpreinf"), billPreinf);
    sets.sync();
    sets.setValue(QString("option/billpostinf"), billPostinf);
    sets.sync();
    sets.setValue(QString("option/userenableconf"), userEnableConf);
    sets.sync();
    sets.setValue(QString("option/rebootdate"), rebootDate);
    sets.sync();
    sets.setValue(QString("option/reboottime"), rebootTime);
    sets.sync();
    sets.setValue(QString("option/networkdevice"), networkDevice);
    sets.sync();
    sets.setValue(QString("option/billitemfontsize"), billItemFontsize);
    sets.sync();
    sets.setValue(QString("option/listemail"), listemail);
    sets.sync();
    sets.setValue(QString("option/listphone"), listphone);
    sets.sync();
}

void ConfigSetting::setDocServer(const QString& docsrv)
{
    if( dockServer.compare(docsrv) ) {
        dockServer = docsrv;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/dock"), dockServer);
        sets.sync();
    }
}

void ConfigSetting::setDockVersion(const QString &ver)
{
    if (dockVersion != ver) {
        dockVersion = ver;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/dockversion"), dockVersion);
        sets.sync();
    }
}

void ConfigSetting::setPrintersize(const QString &pagesize)
{
    if (printerSize != pagesize) {
        printerSize = pagesize;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/printersize"), printerSize);
        sets.sync();
    }
}

void ConfigSetting::setSobanin(int num)
{
    if (sobanin != num) {
        sobanin = num;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/sobanin"), sobanin);
        sets.sync();
    }
}

void ConfigSetting::setTenbanin(const QString &ten)
{
    if (tenbanin != ten) {
        tenbanin = ten;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/tenbanin"), tenbanin);
        sets.sync();
    }
}

void ConfigSetting::setBillPreinf(const QString &info)
{
    if (billPreinf != info) {
        billPreinf = info;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/billpreinf"), billPreinf);
        sets.sync();
    }
}

void ConfigSetting::setBillPostinf(const QString &info)
{
    if (billPostinf != info) {
        billPostinf = info;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/billpostinf"), billPostinf);
        sets.sync();
    }
}

void ConfigSetting::setUserenableConfig(bool enableconf)
{
    if (userEnableConf != enableconf) {
        userEnableConf = enableconf;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/userenableconf"), userEnableConf);
        sets.sync();
    }
}

void ConfigSetting::saveRebootDatetime(int type)
{
    QSettings sets("vng", "vpos");

    if( type ) {
        sets.setValue(QString("option/rebootdate"), rebootDate);
        sets.sync();
    }
    else {
        sets.setValue(QString("option/reboottime"), rebootTime);
        sets.sync();
    }
}

void ConfigSetting::setPrinter(const QString &prnName)
{
    if (printerName != prnName) {
        printerName = prnName;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/printer"), printerName);
        sets.sync();
    }
}

void ConfigSetting::setAppName(const QString &appname)
{
    if( appName.compare(appname) ) {
        appName = appname;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/appname"), appName);
        sets.sync();
    }
}

void ConfigSetting::setAppTitle(const QString &apptitle) {
    if( appTitle.compare(apptitle) ) {
        appTitle = apptitle;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/apptitle"), appTitle);
        sets.sync();
    }
}

void ConfigSetting::setTransProtocol(const QString transpro)
{
    if( transProtocol.compare(transpro) ) {
        transProtocol = transpro;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/transferprotocol"), transProtocol);
        sets.sync();
    }
}

bool ConfigSetting::setPaymentName(const QString& pmName)
{
    if( paymentName.compare(pmName) ) {
        paymentName = pmName;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/payment"), paymentName);
        sets.sync();

        return true;
    }

    return false;
}
void ConfigSetting::setBillItemFontsize(const int fntsize)
{
    if (billItemFontsize != fntsize) {
        billItemFontsize = fntsize;

        QSettings sets("vng", "vpos");
        sets.setValue(QString("option/billitemfontsize"), billItemFontsize);
        sets.sync();
    }
}

void ConfigSetting::setListEmail(const QString &l_email) {
    listemail = l_email;
    QSettings sets("vng", "vpos");
    sets.setValue(QString("option/listemail"), listemail);
    sets.sync();
}

void ConfigSetting::setListPhone(const QString &l_phone) {
    listphone = l_phone;
    QSettings sets("vng", "vpos");
    sets.setValue(QString("option/listphone"), listphone);
    sets.sync();
}

