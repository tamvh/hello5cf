#include "deviceclient.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QMutex>
#include <QWaitCondition>
#include <QTimerEvent>
#include <QtConcurrent>
#include <QNetworkInterface>
#include <QCoreApplication>

#define SERVER_PORT 51915
#define BROADCAST_PORT 45454
#define BROADCAST_WAIT 3000
#define BROADCAST_WAIT2 1000

#define RECONNECT_INTERVAL 3000

#define READ_TIMEOUT    1000
#define PING_TIMEOUT    1000
#define CONNECT_TIMEOUT 500

#define CMD_DISCOVERY       0x0001
#define CMD_CAPABILITIES    0x0002

#define CMD_PRINT           0x0004
#define CMD_START_READER    0x0005
#define CMD_STOP_READER     0x0006

#define CMD_PING            0x0007
#define CMD_SET_WIFI        0x0008
#define CMD_RES_SERVICE     0x0009
#define CMD_PRINTER_STATUS  0x000A
#define CMD_BARCODE_FOUND   0x000B
#define CMD_MIFARE_CARD_FOUND   0x000C

#define CMD_DOCK_STAT       0xFF00

const QString LOG_TAG = QString("[DeviceClient]");

DeviceClient::DeviceClient(QObject *parent) :
    QObject(parent),
    _client(Q_NULLPTR),
    connected(false),
    autoReconnect(true),
    timerReconnect(0),
    timerPing(0),
    pingFlags(0)
{

}

qint16 DeviceClient::discoverDeviceServerBroadcast(const QHostAddress& hostAddress, QList<QHostAddress> &server)
{
    QUdpSocket* udpSocket = new QUdpSocket();

    if (udpSocket->writeDatagram(QString("discovered").toLocal8Bit(), hostAddress, BROADCAST_PORT) > 0)
    {
        //qDebug() << LOG_TAG << " Write datagram ready";

        int msReadTimeout = BROADCAST_WAIT;
        while (udpSocket->waitForReadyRead(msReadTimeout)) {
            //qDebug() << LOG_TAG << " Have response";

            while (udpSocket->hasPendingDatagrams()) {
                QByteArray datagram;
                datagram.resize(udpSocket->pendingDatagramSize());
                QHostAddress host;
                quint16 port;
                udpSocket->readDatagram(datagram.data(), datagram.size(), &host, &port);
                server.append(QHostAddress(host.toIPv4Address()));
                //qDebug() << LOG_TAG << " Response from: " << host.toString() << " port: " << port << " data: " << QString(datagram);
            }

            msReadTimeout = BROADCAST_WAIT2;
        }
    } else {
        return SendBroacastFailed;
    }

    delete udpSocket;

    return Success;
}

qint16 DeviceClient::discoverDeviceServerBroadcast(QList<QHostAddress> &server)
{
    foreach (const QNetworkInterface& interface, QNetworkInterface::allInterfaces()) {
        foreach (const QNetworkAddressEntry& address, interface.addressEntries()) {
#ifndef ALLOW_LOCAL_DOCK
            if (address.ip() == QHostAddress::LocalHost) {
                break;
            }
#endif

            if (address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                discoverDeviceServerBroadcast(address.broadcast(), server);
            }
        }
    }
}

void DeviceClient::connectToServer(const QHostAddress &hostAddress)
{
    _hostAddress = hostAddress;
    if (Q_NULLPTR == _client) {
        _client = new QTcpSocket(this);
        qRegisterMetaType<Qt::ApplicationState>();
        connect((QCoreApplication*)qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                this, SLOT(onApplicationStatusChanged(Qt::ApplicationState)), Qt::QueuedConnection);
        connect(_client, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(onError(QAbstractSocket::SocketError)));
        connect(_client, SIGNAL(connected()),
                this, SLOT(onConnected()));
        connect(_client, SIGNAL(readyRead()),
                this, SLOT(onReadyRead()));
    }

    if (0 == timerReconnect) {
        if (autoReconnect) {
            timerReconnect = startTimer(RECONNECT_INTERVAL);
        }
    }

    if (connected) {
        disconnectFromServer();
    }

    connectToServer();
}

void DeviceClient::connectToServer(const QString &strIp)
{
    connectToServer(QHostAddress(strIp));
}

void DeviceClient::disconnectFromServer()
{
    if (0 != timerReconnect) {
        killTimer(timerReconnect);
        timerReconnect = 0;
    }

    if (0 != timerPing) {
        killTimer(timerPing);
        timerPing = 0;
    }

    _client->disconnectFromHost();
    _client->waitForDisconnected();
    connected = false;
    emit connectionChanged(Disconnected);
}

void DeviceClient::getDeviceCapabilities(quint8& capabilities)
{
    sendDataToServer(composeMessage(CMD_CAPABILITIES));
}

void DeviceClient::getPrinterList(const QString &listPrinter)
{

}

void DeviceClient::print(const QString &printer, const QString& ivcode, const QString &data, int tpl)
{
    //qDebug() << "Send print";
    if (!connected) {
        emit printDone(2, ivcode);
        return;
    }

    QJsonObject obj;
    obj["ivcode"] = ivcode;
    obj["dt"] = data;
    obj["tpl"] = tpl;
    QJsonDocument doc(obj);

    bool ret = sendDataToServer(composeMessage(CMD_PRINT, doc.toJson(QJsonDocument::Compact)));
    if (ret) {
        //qDebug() << "Send print data to server Success";
    } else {
        //qDebug() << "Send print data to server Failed";
    }
}

void DeviceClient::startReaderDevice(const QByteArray &data)
{
    sendDataToServer(composeMessage(CMD_START_READER, data));
}

void DeviceClient::stopReaderDevice()
{
    sendDataToServer(composeMessage(CMD_STOP_READER));
}

void DeviceClient::updateWifi(const QString &ssid, const QString &key, const QString &encrypt)
{
    QJsonObject obj;
    obj["ssid"] = ssid;
    obj["key"] = key;
    obj["encrypt"] = encrypt;
    obj["timeout"] = 15;
    QJsonDocument doc(obj);

    sendDataToServer(composeMessage(CMD_SET_WIFI, doc.toJson(QJsonDocument::Compact)));
}

void DeviceClient::restartService()
{
    sendDataToServer(composeMessage(CMD_RES_SERVICE));
}

void DeviceClient::checkPrinterStatus()
{
    sendDataToServer(composeMessage(CMD_PRINTER_STATUS));
}

void DeviceClient::dockStatistics()
{
    sendDataToServer(composeMessage(CMD_DOCK_STAT));
}

void DeviceClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerReconnect) {
        checkConnection();
    } else if (event->timerId() == timerPing) {
        pingAndSwitch();
    }
}

void DeviceClient::onError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        //qDebug() << _client->errorString();
        switchToDisconnect();
        break;
    default:
        break;
    }
}

void DeviceClient::onConnected()
{
    //qDebug() << "Conntect to server";
    connected = true;
    //qDebug() << "Start ping timer";
    timerPing = startTimer(RECONNECT_INTERVAL);
    pingFlags = 0;
    emit connectionChanged(Connected);
}

void DeviceClient::onReadyRead()
{
    //qDebug() << "On Ready read " << QTime::currentTime().toString("HH:mm:ss.zzz");
    QDataStream socketStream(_client);
    socketStream.setVersion(QDataStream::Qt_4_0);

    quint8 head;
    socketStream >> head;
    if (head == 0xA0) {
        //qDebug() << "Read header";
    } else {
        qCritical() << "Invalid header";
        switchToDisconnect();
        return;
    }

    quint16 len;
    socketStream >> len;
    if (len > 0) {
        char* buff = new char[len];
        socketStream.readRawData(buff, len);
        if (NULL != buff) {
            QByteArray dataArray(buff, len);
            //qDebug() << "processMessage() Len: " << len << " Data: " << dataArray.toHex();
            processMessage(dataArray);

            delete []buff;
        } else {
            qCritical() << "Read data error";
            switchToDisconnect();
        }
    }
}

void DeviceClient::onApplicationStatusChanged(Qt::ApplicationState state)
{
    switch (state) {
    case Qt::ApplicationActive:
        checkConnection();
        break;
    case Qt::ApplicationSuspended:
        switchToDisconnect();
        break;
    }
    //qDebug() << "App status change " << state;
}

void DeviceClient::connectToServer()
{
    _client->abort();
    _client->connectToHost(_hostAddress, SERVER_PORT);
}

#if 0
bool DeviceClient::doCheckServer(const QHostAddress &hostAddress, bool disconnectAfterDone)
{
    bool res = false;
    _client->connectToHost(hostAddress, SERVER_PORT);
    if (_client->waitForConnected(CONNECT_TIMEOUT)) {
        //qDebug() << LOG_TAG << " Connect to server from address: " << hostAddress.toString();
        if (doDiscoverDeviceServer()) {
            if (_client->waitForReadyRead(READ_TIMEOUT)) {
                if (doReadResponse() == 0) {
                    //qDebug() << LOG_TAG << " Found server";
                    res = true;
                }
            } else {
                //qDebug() << LOG_TAG << " Server not response";
            }
        }
        if (disconnectAfterDone) {
            _client->disconnectFromHost();
            _client->waitForDisconnected();
        }
    } else {
        ////qDebug() << LOG_TAG << " Connect to server failed";
    }

    return res;
}
#endif

void DeviceClient::doDiscoverDeviceServer()
{
    sendDataToServer(composeMessage(CMD_DISCOVERY));
}

bool DeviceClient::doPing()
{
    return sendDataToServer(composeMessage(CMD_PING));
}

void DeviceClient::switchToDisconnect()
{
    _client->abort();
    _client->disconnectFromHost();
    _client->waitForDisconnected();
    connected = false;
    if (0 != timerPing) {
        //qDebug() << "Stop ping timer";
        killTimer(timerPing);
        timerPing = 0;
    }

    emit connectionChanged(Disconnected);
}

void DeviceClient::pingAndSwitch()
{
    if (connected) {
        if (!doPing()) {
            switchToDisconnect();
            qWarning() << LOG_TAG <<  "Send ping FAILED";
            return;
        }

        pingFlags++;
        if (pingFlags > 3) {
            switchToDisconnect();
            qWarning() << LOG_TAG <<  "Ping FAILED";
        }
    } else {
        //qDebug() << "Not connect don't need check ping";
    }
}

void DeviceClient::checkConnection()
{
    if (!connected)
        connectToServer();
}

bool DeviceClient::sendDataToServer(const QByteArray &data)
{
    qint64 sent =_client->write(data);
    if (_client->waitForBytesWritten()) {
        //qDebug() << "Send data to server success " << sent;
        //qDebug() << "Send data " << data.toHex();
        return true;
    }

    return false;
}

QByteArray DeviceClient::composeMessage(quint16 cmd, const QByteArray &data)
{
    QByteArray dataMessage;
    QDataStream dataStream(&dataMessage, QIODevice::WriteOnly);
    dataStream.setVersion(QDataStream::Qt_4_0);

    /// Message content
    /// Header	Len     Cmd     SeqID	Data	Checksum
    /// 0xA0	2 Byte  2 Byte	2 Byte	N Byte	1 Byte
    ///
    // Header
    dataStream << (quint8)0xA0;
    // Len = cmd(2byte) + seqid(2byte) + data + checksum(1byte)
    quint16 len = 2 + 2 + data.length() + 1;
    dataStream << len;
    // Cmd
    dataStream << cmd;
    // SeqId
    dataStream << (quint16)0x0001;
    // Data
    if (!data.isEmpty()) {
        dataStream.writeRawData(data.data(), data.length());
    }
    // Checksum
    dataStream << (quint8)0xFF;

    return dataMessage;
}

void DeviceClient::processMessage(const QByteArray &msgData)
{
    QDataStream dataStream(msgData);
    dataStream.setVersion(QDataStream::Qt_4_0);
    quint16 cmd, seqId, res;
    dataStream >> cmd;
    dataStream >> seqId;
    dataStream >> res;
    switch (cmd) {
    case CMD_DISCOVERY:
    {
        //qDebug() << "Command discovery:" << msgData.toHex();
        if (res == 0) {
            //qDebug() << "Response success";
        } else {
            //qDebug() << "Response failed";
        }
    }
        break;
    case CMD_CAPABILITIES:
    {
        //qDebug() << "Command get capabilities:" << msgData.toHex();
        if (res == 0) {
            //qDebug() << "Response success";
        } else {
            //qDebug() << "Response failed";
        }
    }
        break;
    case CMD_PING:
    {
        //qDebug() << "Command ping"  << msgData.toHex();
        if (res == 0) {
            pingFlags = 0;
            //qDebug() << "Response success";
        } else {
            //qDebug() << "Response failed";
        }
    }
        break;
    case CMD_PRINT:
    {
        QByteArray respondeData = msgData.mid(6, msgData.length()-7);
        QJsonDocument doc = QJsonDocument::fromJson(respondeData);
        if (!doc.isObject()) {
            //qDebug() << "Invalid response data";
        }

        QJsonObject obj = doc.object();
        QString ivcode = obj["ivcode"].toString();

        //qDebug() << "Command CMD_PRINT"  << msgData.toHex();
        if (res == 0) {
            //qDebug() << "Response success " << ivcode;
        } else {
            //qDebug() << "Response failed " << ivcode;
        }

        emit printDone(res, ivcode);
    }
        break;
    case CMD_START_READER:
        //qDebug() << "Command CMD_START_READER"  << msgData.toHex();
        break;
    case CMD_STOP_READER:
        //qDebug() << "Command CMD_STOP_READER"  << msgData.toHex();
        break;

    case CMD_PRINTER_STATUS:
    {
        //qDebug() << "Command CMD_PRINTER_STATUS"  << msgData.toHex();
        if (res == 0) {
            //qDebug() << "Printer is READY";
            emit printerReadyChanged(true);
        } else {
            //qDebug() << "Printer is NOT READY";
            emit printerReadyChanged(false);
        }
        break;
    }
#ifdef CHOOSE_BY_BARCODE
    case CMD_BARCODE_FOUND:
    {
        QByteArray respondeData = msgData.mid(6, msgData.length()-7);
        QJsonDocument doc = QJsonDocument::fromJson(respondeData);
        if (!doc.isObject()) {
            //qDebug() << "Invalid response data";
        }

        QJsonObject obj = doc.object();
        QString barcode = obj["barcode"].toString();

        //qDebug() << "Command CMD_BARCODE_FOUND: "  << barcode;
        if (res == 0) {
            emit barcodeFound(barcode);
            //qDebug() << "Response success ";
        } else {
            //qDebug() << "Response failed ";
        }
    }
        break;
#endif
    case CMD_MIFARE_CARD_FOUND:
    {
        QByteArray respondeData = msgData.mid(6, msgData.length()-7);
        QJsonDocument doc = QJsonDocument::fromJson(respondeData);
        if (!doc.isObject()) {
            //qDebug() << "Invalid response data";
        }

        QJsonObject obj = doc.object();
        QString barcode = obj["mifare"].toString();

        qDebug() << "Command CMD_MIFARE_CARD_FOUND: "  << barcode;
        if (res == 0) {
            emit mifareFound(barcode);
            //qDebug() << "Response success ";
        } else {
            //qDebug() << "Response failed ";
        }
    }
        break;
    case CMD_DOCK_STAT:
    {
        QByteArray respondeData = msgData.mid(6, msgData.length()-7);
        QJsonDocument doc = QJsonDocument::fromJson(respondeData);
        if (!doc.isObject()) {
            //qDebug() << "Invalid response data";
        }

        QJsonObject obj = doc.object();
        QJsonObject stat = obj["stat"].toObject();

        qDebug() << "Command CMD_DOCK_STAT: "  << stat;
        if (res == 0) {
            QJsonDocument docRes(stat);
            emit dockStatistics(docRes.toJson(QJsonDocument::Indented));
            //qDebug() << "Response success ";
        } else {
            //qDebug() << "Response failed ";
        }
    }
        break;

    default:
        //qDebug() << "Unknow command"  << msgData.toHex();
        break;
    }
}
