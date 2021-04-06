#ifndef IPARSINGSTRUCT_H
#define IPARSINGSTRUCT_H

#include <memory>
#include <vector>
#include <cstdint>
#include <cstring>
#include <array>

class IParsingStruct {
protected:
    std::vector<IParsingStruct*> children;

public:
    IParsingStruct(IParsingStruct *parent = nullptr) {
        if (parent != nullptr) {
            parent->addChild(this);
        }
    }

    static uint8_t calcCheckSumETC(const uint8_t * data, size_t len) {
        int sum = 0;
        for (int i = 0; i < (int) len; ++i) {
            sum += data[i];
        }

        return (uint8_t)(sum & 0xff);
    }

    void addChild(IParsingStruct *child) {
        children.push_back(child);
    }

    inline virtual int getSize() {
        int sum = 0;
        for (auto &child: children) {
            sum += child->getSize();
        }

        return sum;
    }

    inline virtual std::vector<uint8_t> getBuffer() {
        std::vector<uint8_t> buffer;
        for (auto &child: children) {
            auto buff = child->getBuffer();
            buffer.insert(buffer.end(), buff.begin(), buff.end());
        }

        return buffer;
    }

    virtual void parse(const uint8_t* buff) = 0;
};

class CompoundType: public IParsingStruct {
    // IParsingStruct interface
public:
    CompoundType(IParsingStruct *parent = nullptr): IParsingStruct(parent) {}
    void parse(const uint8_t *buff) override {
        for (auto &child: children) {
            child->parse(buff);
            buff = buff + child->getSize();
        }
    }
};


template<typename T>
class BasicType: public IParsingStruct {
    // IParsingStruct interface
public:
    T val;


    // IParsingStruct interface
public:
    BasicType(IParsingStruct *parent = nullptr): IParsingStruct(parent) {}
    int getSize() {
        return sizeof(T);
    }

    std::vector<uint8_t> getBuffer() override {
        std::vector<uint8_t> result(sizeof(T));
        std::copy(reinterpret_cast<uint8_t*>(&val), reinterpret_cast<uint8_t*>(&val) + sizeof(T), result.begin() );
        return result;
}

    void parse(const uint8_t *buff) override {
        std::copy(buff, buff + getSize(),  reinterpret_cast<uint8_t*>(&val));
    }
};

template <class T, std::size_t N>
class ArrayType: public IParsingStruct {
public:
    std::array<T, N> val;

    // IParsingStruct interface
public:
    ArrayType(IParsingStruct *parent = nullptr): IParsingStruct(parent) {}

    int getSize() {
        return val.size();
    }

    std::vector<uint8_t> getBuffer() override {
        std::vector<uint8_t> result(val.size());
        std::copy(reinterpret_cast<uint8_t*>(val.begin()), reinterpret_cast<uint8_t*>(val.end()) + sizeof(T), result.begin() );
        return result;
}

    void parse(const uint8_t *buff) override {
        std::copy(buff, buff + getSize(),  reinterpret_cast<uint8_t*>(val.data()));
    }
};




#endif // IPARSINGSTRUCT_H
