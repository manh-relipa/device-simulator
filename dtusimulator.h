#ifndef DTUSIMULATOR_H
#define DTUSIMULATOR_H

#include "Interfaces/iMessageHandler.h"
#include "Interfaces/iMessageProvider.h"
#include "Interfaces/iSetting.h"
#include "DtuFrame.h"

#include <QObject>
#include <QTimer>

class DtuSimulator : public iMessageProvider, public iMessageHandler
{
    Q_OBJECT
public:
    explicit DtuSimulator(QObject *parent = nullptr);

    enum DtuState {
        DTU_IDLE,
        DTU_GOT_START_REQUEST,
        DTU_GOT_START_STATUS_REQUEST,
        DTU_GOT_CONFIRM_START_REQUEST,
        DTU_REALTIME_DATA,

        DUT_GOT_STOP_STATUS_REQUEST,
        DTU_GOT_CONFIRM_STOP_REQUEST,
        DTU_GOT_FILE_REQUEST
    };

signals:




    // iMessageHandler interface
public:
    void handleMessage(const std::string &source, const std::string &destination, const std::string &message) override;

    void handleStartRequest();
    void handleStatusRequestForStart();
    void handleStatusRequestForStop();
    void handleConfirmStartRequest();
    void handleConfirmStopRequeset();

    void sendRealtimeData();

    Q_INVOKABLE void handleSpeedChanged(float speed);
    void packAndSend(DtuFrame & frame);

private:
    float currentSpeed;
    DtuState currentState{DTU_IDLE};

    QTimer realtimeDataTimer;
    static uint16_t sequenceID;

};

#endif // DTUSIMULATOR_H
