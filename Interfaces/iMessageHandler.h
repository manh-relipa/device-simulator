#ifndef IMESSAGEHANDLER_H
#define IMESSAGEHANDLER_H
#include <string>
#include <unordered_map>

class iMessageHandler {
protected:
    std::unordered_map<std::string, int> requestSequenceMap;
public:
    virtual void handleMessage(const std::string &source, const std::string &destination, const std::string &message) = 0;
};





#endif // IMESSAGEHANDLER_H
