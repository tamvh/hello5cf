#include "deviceclientservice.h"
#include "configsetting.h"

DeviceClientService::DeviceClientService(QObject *parent) : QThread(parent), deviceClient(Q_NULLPTR), running(false)
{

}

void DeviceClientService::startService(const QString& ip)
{
    if (!running) {
        running = true;
        serverIp = ip;
        start();
    } else {
        connectToServer(ip);
    }
}

void DeviceClientService::stopService()
{
    if (isConnected()) {
        disconnectFromServer();
    }
    quit();
    wait();
}

qint16 DeviceClientService::connectToServer(const QString &strIp)
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "connectToServer",Qt::QueuedConnection,
            Q_ARG(QString, strIp));
    return returnedValue;
}

qint16 DeviceClientService::disconnectFromServer()
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "disconnectFromServer",Qt::QueuedConnection);
    return returnedValue;
}

qint16 DeviceClientService::print(const QString &printer, const QString& ivcode, const QString &data, int tpl)
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "print",Qt::QueuedConnection,
            Q_ARG(QString, printer),
            Q_ARG(QString, ivcode),
            Q_ARG(QString, data),
            Q_ARG(int, tpl));
    return returnedValue;
}

qint16 DeviceClientService::startReaderDevice(const QByteArray &data)
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "startReaderDevice",Qt::QueuedConnection,
            Q_ARG(QByteArray, data));
    return returnedValue;
}

qint16 DeviceClientService::stopReaderDevice()
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "stopReaderDevice",Qt::QueuedConnection);
    return returnedValue;
}

qint16 DeviceClientService::updateWifi(const QString &ssid, const QString &key, const QString &encrypt)
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "updateWifi",Qt::QueuedConnection,
            Q_ARG(QString, ssid),
            Q_ARG(QString, key),
            Q_ARG(QString, encrypt));
    return returnedValue;
}

qint16 DeviceClientService::restartService()
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "restartService",Qt::QueuedConnection);
    return returnedValue;
}

qint16 DeviceClientService::checkPrinterStatus()
{
    qint16 returnedValue = 0;
    QMetaObject::invokeMethod(deviceClient, "checkPrinterStatus",Qt::QueuedConnection);
    return returnedValue;
}

void DeviceClientService::dockStatistics()
{
    QMetaObject::invokeMethod(deviceClient, "dockStatistics",Qt::QueuedConnection);
}

bool DeviceClientService::isConnected()
{
    if (deviceClient && deviceClient->isConnected()) {
        return true;
    }

    return false;
}

void DeviceClientService::run()
{
    deviceClient = new DeviceClient();
    connect(deviceClient, SIGNAL(connectionChanged(int)),  this, SIGNAL(connectionChanged(int)));
    connect(deviceClient, SIGNAL(printDone(quint16,QString)),  this, SIGNAL(printDone(quint16,QString)));
    connect(deviceClient, SIGNAL(printerReadyChanged(bool)),  this, SIGNAL(printerReadyChanged(bool)));
#ifdef CHOOSE_BY_BARCODE
    connect(deviceClient, SIGNAL(barcodeFound(QString)),  this, SIGNAL(barcodeFound(QString)));
#endif
    connect(deviceClient, SIGNAL(mifareFound(QString)),  this, SIGNAL(mifareFound(QString)));
    connect(deviceClient, SIGNAL(dockStatistics(QString)),  this, SIGNAL(dockStat(QString)));
    deviceClient->connectToServer(serverIp);

    exec();

    delete deviceClient;
}
