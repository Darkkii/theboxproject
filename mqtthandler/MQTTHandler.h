#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H

#include <string>
#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"

class MQTTHandler
{
private:
    std::string mNetworkSSID;
    std::string mNetworkPassword;
    std::string mBrokerIP;
    IPStack mIPStack;
    MQTT::Client<IPStack, Countdown> mMQTTClient;

public:
    explicit MQTTHandler();
};

#endif /* MQTTHANDLER_H */
