#include "dtusimulator.h"


#include <QDateTime>

uint16_t DtuSimulator::sequenceID = 0;
DtuSimulator::DtuSimulator(QObject *parent)
{
    QObject::connect(&realtimeDataTimer, &QTimer::timeout, this, &DtuSimulator::sendRealtimeData);
    realtimeDataTimer.setInterval(1000);
}

void DtuSimulator::handleMessage(const std::string &source, const std::string &destination, const std::string &message)
{
    const char* buffer = message.data();
    const char* backup_address = message.data();

    DtuFrame frame;
    frame.parse(reinterpret_cast<const uint8_t*>(buffer));
    auto package_length = frame.getSize();
    qInfo() << "Message length: " << message.size();
    qInfo() << "package length: " << package_length;
    auto checksum = IParsingStruct::calcCheckSumETC(reinterpret_cast<const uint8_t*>(backup_address + 1), package_length - 3);

    qInfo() << "Calculated checksum: " << QString("%1").arg(checksum,2, 16, QChar('0'))
            << ", buffer checksum: " << QString("%1").arg(frame.footer->val.checksum , 2, 16, QChar('0'));

    if (frame.footer->val.checksum != checksum) {

//            auto nak_buffer = createNakMessage(ERROR_CODE_ENUM_WRONG_CHECKSUM);

//            emit messageReady(ETC_DEVICE, ETC_DEVICE,
//                              std::string(reinterpret_cast<const char *>(nak_buffer.data()), nak_buffer.size()));
        return;
    }

    uint8_t commandId1 = frame.header->val.commandId[0];
    uint8_t commandId2 = frame.header->val.commandId[1];

    if (commandId1 == DTU1_START_REQUEST_COMMAND_1 && commandId2 == DTU1_START_REQUEST_COMMAND_2) {
         if (currentState == DTU_IDLE) {
            handleStartRequest();
         }
    }
    else if (commandId1 == DTU1_STATUS_REQUEST_COMMAND_1 && commandId2 == DTU1_STATUS_REQUEST_COMMAND_2 ) {
        if (currentState == DTU_GOT_START_REQUEST) {
            handleStatusRequestForStart();
        }
        else if (currentState == DTU_REALTIME_DATA) {
            handleStatusRequestForStop();
        }
    }

    else if (commandId1 == DTU1_STOP_REQUEST_COMMAND_1 && commandId2 == DTU1_STOP_REQUEST_COMMAND_2 ) {
        if (currentState == DTU_GOT_START_STATUS_REQUEST) {
            handleConfirmStartRequest();
        }
        else if (currentState == DUT_GOT_STOP_STATUS_REQUEST) {
            handleConfirmStopRequeset();
        }

    }

    else if (commandId1 == DTU_LAST_MINUTE_DATA_COMMAND_1 && commandId2 == DTU_LAST_MINUTE_DATA_COMMAND_2) {
        if (currentState == DTU_GOT_CONFIRM_STOP_REQUEST || currentState == DTU_GOT_FILE_REQUEST) {

        }
    }

}

void DtuSimulator::handleStartRequest()
{
    DtuFrame startRequestResponse;

    startRequestResponse.header->val.commandId[0] = DTU1_START_REQUEST_REP_COMMAND_1;
    startRequestResponse.header->val.commandId[1] = DTU1_START_REQUEST_REP_COMMAND_2;

    auto startResponseData = std::make_shared<Dtu1StartRequestResponse>();
    startResponseData->indicator->val = 0x06;
    startResponseData->mode->val = 0x02;
    std::array<uint8_t, 16> serial{'D', 'T', 'U', '1', '-', '1', '2', '3',
                                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'
                                 };
    startResponseData->serialNumber->val.swap(serial);

    startRequestResponse.setData(startResponseData);
    packAndSend(startRequestResponse);

    currentState = DTU_GOT_START_REQUEST;
}

void DtuSimulator::handleStatusRequestForStart()
{
    DtuFrame statusResponse;

    statusResponse.header->val.commandId[0] = DTU1_STATUS_REQUEST_REP_COMMAND_1;
    statusResponse.header->val.commandId[1] = DTU1_STATUS_REQUEST_REP_COMMAND_2;

    auto data = std::make_shared<BasicType<uint8_t>>();
    data->val = 0x00;
    statusResponse.setData(data);
    packAndSend(statusResponse);

    currentState = DTU_GOT_START_STATUS_REQUEST;
    realtimeDataTimer.start();
}

void DtuSimulator::handleStatusRequestForStop()
{
    realtimeDataTimer.stop();
    DtuFrame statusResponse;
    statusResponse.header->val.commandId[0] = DTU1_STATUS_REQUEST_REP_COMMAND_1;
    statusResponse.header->val.commandId[1] = DTU1_STATUS_REQUEST_REP_COMMAND_2;

    auto data = std::make_shared<BasicType<uint8_t>>();
    data->val = 0x01;
    statusResponse.setData(data);
    packAndSend(statusResponse);

    currentState = DTU_REALTIME_DATA;

}

void DtuSimulator::handleConfirmStartRequest()
{
    DtuFrame confirmStartReponse;

    confirmStartReponse.header->val.commandId[0] = DTU1_STOP_REQUEST_REP_COMMAND_1;
    confirmStartReponse.header->val.commandId[1] = DTU1_STOP_REQUEST_REP_COMMAND_2;

    auto data = std::make_shared<BasicType<uint8_t>>();
    data->val = 0x06;
    confirmStartReponse.setData(data);
    packAndSend(confirmStartReponse);

}

void DtuSimulator::handleConfirmStopRequeset()
{
    DtuFrame confirmStartReponse;

    confirmStartReponse.header->val.commandId[0] = DTU1_STOP_REQUEST_REP_COMMAND_1;
    confirmStartReponse.header->val.commandId[1] = DTU1_STOP_REQUEST_REP_COMMAND_2;

    auto data = std::make_shared<BasicType<uint8_t>>();
    data->val = 0x06;
    confirmStartReponse.setData(data);
    packAndSend(confirmStartReponse);
}

void DtuSimulator::sendRealtimeData()
{
    DtuFrame realtimeDataFrame;

    realtimeDataFrame.header->val.commandId[0] = DTU_LAST_MINUTE_DATA_REP_COMMAND_1;
    realtimeDataFrame.header->val.commandId[1] = DTU_LAST_MINUTE_DATA_REP_COMMAND_2;

    auto realtimeData = std::make_shared<Dtu1RunningInfo>();
    Dtu1Time timeData;
    auto currentTime = QDateTime::currentDateTime();
    uint16_t year = currentTime.date().year();
    timeData.year[0] = (uint8_t) (year & 0xff);
    timeData.year[1] = (uint8_t) ((year >> 8) & 0xff);

    timeData.month = currentTime.date().month();
    timeData.date = currentTime.date().day();
    timeData.hour = currentTime.time().hour();
    timeData.minute = currentTime.time().minute();
    timeData.second = currentTime.time().second();
    timeData.subsecond = 0;

    std::copy((uint8_t*) &timeData, (uint8_t*) &timeData + sizeof(Dtu1Time),
               realtimeData->time->val.begin());

    Dtu1RunningInfoData runData;
    runData.speed1 = currentSpeed;
    int distance = (currentSpeed * 1e5) / 7200;
    runData.distance1[0] = (uint8_t) (distance & 0xff);
    runData.distance1[0] = (uint8_t) ((distance >> 8) & 0xff);

    runData.speed2 = currentSpeed;
    runData.distance2[0] = runData.distance1[0];
    runData.distance2[1] = runData.distance1[1];

    std::copy((uint8_t*) &runData, (uint8_t*) &runData + sizeof(Dtu1RunningInfoData),
               realtimeData->runningInfoData->val.begin());

    realtimeDataFrame.setData(realtimeData);
    packAndSend(realtimeDataFrame);
}

void DtuSimulator::handleSpeedChanged(float speed)
{
    currentSpeed = speed;
}

void DtuSimulator::packAndSend(DtuFrame &frame)
{
    uint16_t datasize = frame.data->getSize();
    frame.header->val.dataSize[0] =  (uint8_t) (datasize & 0xff);
    frame.header->val.dataSize[1] =  (uint8_t) ((datasize >> 8) & 0xff);

    frame.header->val.seqNumber[0] =  (uint8_t) (sequenceID & 0xff);
    frame.header->val.seqNumber[1] =  (uint8_t) ((sequenceID >> 8) & 0xff);

    sequenceID++;

    auto buffer = frame.getBuffer();

    auto checksum = IParsingStruct::calcCheckSumETC(buffer.data() + 1, buffer.size() - 3);
    buffer[buffer.size()-2] = checksum;

    emit messageReady("DTU1_PROTOCOL", "DTU1_DEVICE",
                      std::string(reinterpret_cast<const char *>(buffer.data()), buffer.size()));
}
