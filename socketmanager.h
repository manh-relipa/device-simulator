#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H
#include "Interfaces/iCommunicator.h"

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <memory>
#include <QTimer>

class SocketManager : public QObject, public ICommunicator
{
    Q_OBJECT
public:
    explicit SocketManager(QObject *parent = nullptr);
    ~SocketManager();
    void prepare_setting();
    void sendMessage(const std::string &source, const std::string &destination, const std::string& message);

    // Tcp Client

    void handleConnected();
    void handleDisconnected();

    void handleRead();
public slots:
    void setupSocket();
    void displayError(QAbstractSocket::SocketError socketError);
signals:
    void needReconnect();
    void textChanged(QString text);

    // ICommunicator interface
public:
    void addHandler(const std::shared_ptr<iMessageHandler> handler) override;
    void addMessageProvider(const std::shared_ptr<iMessageProvider> handler) override;

    QString hostName;
    int hostPort;
    std::shared_ptr<QTcpSocket> localSocket;
    QTimer timer;
    QDataStream inDataStream;
};


#endif // SOCKETMANAGER_H
