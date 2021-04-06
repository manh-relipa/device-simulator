#ifndef ICOMMUNICATOR_H
#define ICOMMUNICATOR_H
#include "iMessageHandler.h"
#include "iMessageProvider.h"

#include <vector>
#include <memory>

class ICommunicator {
public:
    virtual void addHandler(const std::shared_ptr<iMessageHandler> handler) = 0;
    virtual void addMessageProvider(const std::shared_ptr<iMessageProvider> handler) = 0;

protected:
    std::vector<std::shared_ptr<iMessageHandler>> messageHanders;
    std::vector<std::shared_ptr<iMessageProvider>> messageProviders;
};

#endif // ICOMMUNICATOR_H
