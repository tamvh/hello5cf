#ifndef DEVICECLIENT_H
#define DEVICECLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QAbstractSocket>

class QTcpSocket;
class DeviceClient : public QObject
{
    Q_OBJECT
public:
    enum DeviceError {
        Success = 0,
        Failed,
        ConnectFailed,
        SendDataToServerFailed,
        ServerResponseTimeout,
        SendBroacastFailed,
        LostConnectToServer,
        UnknownError
    };

    enum ConnectStatus {
        Connected,
        Disconnected,
        Connecting
    };

    enum PrintTemplate {
        Payment = 0,
        Recharge,
        VPOS_Payment
    };

    explicit DeviceClient(QObject *parent = 0);

public slots:
    static qint16 discoverDeviceServerBroadcast(const QHostAddress& hostAddress, QList<QHostAddress>& server);
    static qint16 discoverDeviceServerBroadcast(QList<QHostAddress>& server);
    void connectToServer(const QHostAddress& hostAddress);
    void connectToServer(const QString& strIp);
    void connectToServer();
    void disconnectFromServer();
    void getDeviceCapabilities(quint8& capabilities);
    void getPrinterList(const QString& listPrinter);
    void print(const QString& printer, const QString& ivcode, const QString& data, int tpl);
    void startReaderDevice(const QByteArray& data);
    void stopReaderDevice();
    void updateWifi(const QString& ssid, const QString& key, const QString& encrypt);
    void restartService();
    void checkPrinterStatus();
    void dockStatistics();

    bool isConnected() {return connected;}
signals:
    void connectionChanged(int status);
    void printDone(quint16 errcode, const QString& invoiceCode);
    void printerReadyChanged(bool ready);
#ifdef CHOOSE_BY_BARCODE
    void barcodeFound(const QString& barcode);
#endif
    void mifareFound(const QString& mfdata);
    void dockStatistics(const QString& stat);
protected:
    void timerEvent(QTimerEvent * event);

private slots:
    void onError(QAbstractSocket::SocketError socketError);
    void onConnected();
    void onReadyRead();

    void onApplicationStatusChanged(Qt::ApplicationState state);

private:
    void doDiscoverDeviceServer();
    bool doPing();
    void switchToDisconnect();
    void pingAndSwitch();
    void checkConnection();

    bool sendDataToServer(const QByteArray& data);
    QByteArray composeMessage(quint16 cmd, const QByteArray& data = QByteArray());
    void processMessage(const QByteArray& msgData);

private:
    QTcpSocket *_client;
    QHostAddress _hostAddress;
    bool connected;
    bool autoReconnect;
    int timerReconnect;

    int timerPing;
    int pingFlags;
};

#endif // DEVICECLIENT_H
