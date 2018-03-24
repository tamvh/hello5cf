#ifndef DEVICECLIENTSERVICE_H
#define DEVICECLIENTSERVICE_H

#include <QThread>
#include "deviceclient.h"

class DeviceClientService : public QThread
{
    Q_OBJECT
public:
    explicit DeviceClientService(QObject *parent = 0);

    void startService(const QString& ip);
    void stopService();

    qint16 connectToServer(const QString& strIp);
    qint16 disconnectFromServer();
    qint16 print(const QString& printer, const QString& ivcode, const QString& data, int tpl);
    qint16 startReaderDevice(const QByteArray& data);
    qint16 stopReaderDevice();
    qint16 updateWifi(const QString& ssid, const QString& key, const QString& encrypt);
    qint16 restartService();
    qint16 checkPrinterStatus();
    void dockStatistics();

    bool isConnected();
protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void connectionChanged(int status);
    void printDone(quint16 errcode, const QString& ivcode);
    void error(const QString &s);
    void printerReadyChanged(bool ready);
#ifdef CHOOSE_BY_BARCODE
    void barcodeFound(const QString& barcode);
#endif
    void mifareFound(const QString& mfdata);
    void dockStat(const QString& stat);

public slots:
private:
    DeviceClient* deviceClient;
    QString serverIp;
    bool running;
};

#endif // DEVICECLIENTSERVICE_H
