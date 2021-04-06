#ifndef IMESSAGESENDER_H
#define IMESSAGESENDER_H
#include <QString>
#include <QObject>

class iMessageProvider: public QObject
{
    Q_OBJECT
signals:
    void messageReady(const std::string &source, const std::string &destination, const std::string &message);
};

#endif // IMESSAGESENDER_H
