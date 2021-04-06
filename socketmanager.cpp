#include "socketmanager.h"
#include "Interfaces/Singleton.h"

#include <QSettings>
#include <QDataStream>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

SocketManager::SocketManager(QObject *parent) : QObject(parent)
{
    prepare_setting();
    localSocket = std::make_shared<QTcpSocket>();
//    inDataStream.setDevice(localSocket.get());
//    inDataStream.setVersion(QDataStream::Qt_5_13);

    connect(this, SIGNAL(needReconnect()), this, SLOT(setupSocket()));
}

SocketManager::~SocketManager()
{
    if (localSocket) {
        localSocket->disconnectFromHost();
    }
}

void SocketManager::prepare_setting()
{
    QSettings setting("settings.ini", QSettings::IniFormat);
    setting.beginGroup("Service_Setting");
    hostName = setting.value("SERVER_NAME", "127.0.0.1").toString();
    setting.setValue("SERVER_NAME", hostName);
    hostPort = setting.value("Port", 8888).toInt();
    setting.setValue("Port", hostPort);
    setting.endGroup();
}


void SocketManager::addHandler(const std::shared_ptr<iMessageHandler> handler)
{
    this->messageHanders.push_back(handler);
}

void SocketManager::addMessageProvider(const std::shared_ptr<iMessageProvider> handler)
{
    messageProviders.push_back(handler);
    connect(handler.get(), &iMessageProvider::messageReady, this, &SocketManager::sendMessage);
}

void SocketManager::sendMessage(const std::string &source, const std::string &destination, const std::string& message) {
    QByteArray block;
    block = QByteArray::fromStdString(message);

    QString hexBuffer("DTU1->Service: ");
    for (int i = 0; i < block.size(); ++i) {
        hexBuffer += QString("%1").arg(block[i] , 2, 16, QChar('0'));
    }

    qInfo() << hexBuffer;

    emit textChanged(QString(hexBuffer) + "\n");

    qInfo() << "Sending: " << QString(hexBuffer) << " from: " << QString::fromStdString(source);

    auto ret = localSocket->write(block);
    qInfo() << "Written: " << ret << " bytes";

}

void SocketManager::setupSocket() {
    qInfo() << "Setup Socket";
    connect(localSocket.get(), &QTcpSocket ::disconnected, this, &SocketManager::handleDisconnected, Qt::UniqueConnection);
    connect(localSocket.get(), &QTcpSocket::connected, this, &SocketManager::handleConnected, Qt::UniqueConnection);
//    QObject::connect(localSocket.get(), &QLocalSocket::error, this, &SocketManager::displayError, Qt::UniqueConnection);
    connect(localSocket.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)), Qt::UniqueConnection);
    connect(localSocket.get(),&QTcpSocket::readyRead, this, &SocketManager::handleRead, Qt::UniqueConnection);

    localSocket->abort();
//    qInfo() << "Connect to " << serverName;
    localSocket->connectToHost(hostName, hostPort);
}

void SocketManager::handleConnected() {

    qDebug() << "Socket connected";

}

void SocketManager::handleDisconnected() {
    qDebug() << "Socket disconnected";
//    disconnect(tcpSocket.get(), &QIODevice::readyRead, this, &PeripheralManager::publishPeripheralData);

    localSocket->abort();
//    tcpSocket->connectToHost("localhost", tcpPort);
}

void SocketManager::displayError(QAbstractSocket::SocketError socketError)
{
    qInfo() << "Got socket error";
    switch (socketError) {
    case QTcpSocket::HostNotFoundError:
         qInfo("The host was not found. Please make sure "
            "that the server is running and that the "
            "server name is correct.");
        break;
    case QTcpSocket::ConnectionRefusedError:

       qInfo("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the server name "
                                    "is correct.");
        break;
    default:
        break;
    }

    timer.singleShot(5000, [&]() {
        qInfo() << "Retry connect server";
        emit needReconnect();
    });
}

void SocketManager::handleRead()
{
    while (localSocket->bytesAvailable()) {
//        inDataStream.startTransaction();
        QByteArray nextMessage;
//        inDataStream >> nextMessage;
        nextMessage = localSocket->readAll();

//        if (!inDataStream.commitTransaction())
//            return;

        QString hexBuffer("Service->DTU1: ");
        for (int i = 0; i < nextMessage.size(); ++i) {
            hexBuffer += QString("%1").arg(nextMessage[i] , 2, 16, QChar('0'));
        }


        qInfo() << hexBuffer;

        emit textChanged(QString(hexBuffer) + "\n");

        for (auto &handler: messageHanders) {
            handler->handleMessage("","", nextMessage.toStdString());
        }




        QEventLoop loop;
        QTimer::singleShot(100, &loop, SLOT(quit()));
        loop.exec();
    }
}
