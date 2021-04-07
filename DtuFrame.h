#ifndef DTU1FRAME_H
#define DTU1FRAME_H

#include "IParsingStruct.h"

#include <QDebug>
#include <vector>
#include <algorithm>

#define STX                0x02       // STX
#define ETX                0x03       // ETX

#define DTU1_START_REQUEST_COMMAND_1 0x00
#define DTU1_START_REQUEST_COMMAND_2 0x01

#define DTU1_START_REQUEST_REP_COMMAND_1 0x80
#define DTU1_START_REQUEST_REP_COMMAND_2 0x01

#define DTU1_STATUS_REQUEST_COMMAND_1 0x00
#define DTU1_STATUS_REQUEST_COMMAND_2 0x02

#define DTU1_STATUS_REQUEST_REP_COMMAND_1 0x80
#define DTU1_STATUS_REQUEST_REP_COMMAND_2 0x02

#define DTU1_STOP_REQUEST_COMMAND_1 0x40
#define DTU1_STOP_REQUEST_COMMAND_2 0x11

#define DTU1_STOP_REQUEST_REP_COMMAND_1 0x80
#define DTU1_STOP_REQUEST_REP_COMMAND_2 0x11

#define DTU_REALTIME_DATA_COMMAND_1 0x20
#define DTU_REALTIME_DATA_COMMAND_2 0x13

#define DTU_LAST_MINUTE_DATA_COMMAND_1 0x00
#define DTU_LAST_MINUTE_DATA_COMMAND_2 0x03

#define DTU_LAST_MINUTE_DATA_REP_COMMAND_1 0x80
#define DTU_LAST_MINUTE_DATA_REP_COMMAND_2 0x03


struct Dtu1Header {
    uint8_t stx{0x02};
    uint8_t commandId[2];
    uint8_t seqNumber[2];
    uint8_t dataSize[2];
};

struct Dtu1Footer {
    uint8_t checksum;
    uint8_t etx{0x03};
};

struct Dtu1StartRequest { // 起動要求リクエスト, 状態確認リクエスト
    uint8_t data1{0x53};
    uint8_t data2{0x54};
};

struct Dtu1StartRequestResponse: public CompoundType { // 起動要求リクエスト応答
    std::shared_ptr<BasicType<uint8_t>> indicator;
    std::shared_ptr<BasicType<uint8_t>> mode;
    std::shared_ptr<ArrayType<uint8_t, 16>> serialNumber;

    Dtu1StartRequestResponse() {
        indicator = std::make_shared<BasicType<uint8_t>>(this);
        mode = std::make_shared<BasicType<uint8_t>>(this);
        serialNumber = std::make_shared<ArrayType<uint8_t, 16>>(this);
    }
};

struct Dtu1Time { // size 8
    uint8_t year[2];
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t subsecond;
};

struct Dtu1RunningInfoData { // size 8
    unsigned char speed1;
    uint8_t reserved;
    uint8_t distance1[2];
    uint8_t speed2;
    uint8_t reserved2;
    uint8_t distance2[2];
};

struct Dtu1RunningInfo: public CompoundType  { // 法定三要素リアルタイム出力機能
    std::shared_ptr<ArrayType<uint8_t, 8>> time; //Dtu1Time
    std::shared_ptr<ArrayType<uint8_t, 8>> runningInfoData;//Dtu1RunningInfoData

    Dtu1RunningInfo() {
        time = std::make_shared<ArrayType<uint8_t, 8>>(this);
        runningInfoData = std::make_shared<ArrayType<uint8_t, 8>>(this);
    }
};

struct Dtu1StartStopRequest: public CompoundType {
    std::shared_ptr<BasicType<uint16_t>> indicator;
    std::shared_ptr<ArrayType<uint8_t, 10>> driverId;

    Dtu1StartStopRequest() {
        indicator = std::make_shared<BasicType<uint16_t>>(this);
        driverId = std::make_shared<ArrayType<uint8_t, 10>>(this);

        indicator->val = 0x3335;
    }
};

struct Dtu1LastMinuteDataRequest {
    uint8_t data[3]{0x47, 0x45, 0x54};
};

struct Dtu1LastMinuteDataResponse: public IParsingStruct {
    uint8_t segment;
    uint8_t fileNo;
    std::vector<uint8_t> data;



    // IParsingStruct interface
public:
    Dtu1LastMinuteDataResponse(int dataSize) {
        data.resize(dataSize - 2);
    }

    int getSize() override
    {
        return data.size() + 2;
    }
    void parse(const uint8_t *buff) override
    {
        segment = buff[0];
        fileNo = buff[1];
        std::copy(buff, buff + data.size(), data.begin());
    }
};


struct DtuFrame: public IParsingStruct {
    std::shared_ptr<BasicType<Dtu1Header>> header;
    std::shared_ptr<IParsingStruct> data;
    std::shared_ptr<BasicType<Dtu1Footer>> footer;

    DtuFrame() {
        header = std::make_shared<BasicType<Dtu1Header>>(this);
        footer = std::make_shared<BasicType<Dtu1Footer>>(this);
    }

    inline void setData(std::shared_ptr<IParsingStruct> data_) {
        if (children.size() > 2)
            return;

        data = data_;
        children.insert(children.begin() + 1, data.get());
    }

    // IParsingStruct interface
public:
    inline void parse(const uint8_t *buff_original) override
    {
        const uint8_t* buff_ = buff_original;
        try {


            header->parse(buff_);
            buff_ = buff_ + header->getSize();

            uint8_t commandId1 = header->val.commandId[0];
            uint8_t commandId2 = header->val.commandId[1];

            if (commandId1 == DTU1_START_REQUEST_COMMAND_1 && commandId2 == DTU1_START_REQUEST_COMMAND_2) {
                data = std::make_shared<BasicType<Dtu1StartRequest>>();
            }

            else if (commandId1 == DTU1_STATUS_REQUEST_COMMAND_1 && commandId2 == DTU1_STATUS_REQUEST_COMMAND_2) {
                data = std::make_shared<BasicType<Dtu1StartRequest>>();
            }

            else if (commandId1 == DTU1_STOP_REQUEST_COMMAND_1 && commandId2 == DTU1_STOP_REQUEST_COMMAND_2) {
                data = std::make_shared<Dtu1StartStopRequest>();
            }

            else if (commandId1 == DTU_LAST_MINUTE_DATA_COMMAND_1 && commandId2 == DTU_LAST_MINUTE_DATA_COMMAND_2) {

                data = std::make_shared<BasicType<Dtu1LastMinuteDataRequest>>();

            }

            if (data != nullptr) {
                data->parse(buff_);
                children.insert(children.begin() + 1, data.get());
                buff_ = buff_ + data->getSize();
            }


            footer->parse(buff_);

        }  catch (std::exception &ex) {
            qFatal("Fatal error: %s", ex.what());
        }
    }
};



#endif // DTU1FRAME_H
