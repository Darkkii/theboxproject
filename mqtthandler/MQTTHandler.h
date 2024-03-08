#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H

#include <string>
#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "Observer.h"

class MQTTHandler : public Observer
{
private:
    bool mMQTTEnabled = false;
    int mBrokerPort = 1883;
    int mRC = 0;
    int mMessageCount = 0;
    std::string mNetworkID = "PICO-Q59k95";
    std::string mNetworkPW = "Q5-9k195";
    std::string mBrokerIP = "192.168.137.1";
    std::string mClientID = "PicoW-G06";
    const std::string mStatusTopic = "controller/status";
    const std::string mSettingsTopic = "controller/settings";
    IPStack mIPStack = IPStack(mNetworkID.c_str(), mNetworkPW.c_str());
    MQTT::Client<IPStack, Countdown> mMQTTClient = MQTT::Client<IPStack, Countdown>(mIPStack);
    MQTTPacket_connectData mData = MQTTPacket_connectData_initializer;
    bool mMQTTConnect();
    bool mMQTTSubscribe(const std::string topic);
    void mMQTTSendStatus();
    void mMQTTSendSettings();
    static void mMQTTMessageHandler(MQTT::MessageData &md);

public:
    explicit MQTTHandler();
    void connect();
    void setNetworkID(std::string networkID);
    void setNetworkPW(std::string networkPW);
    void setBrokerIP(std::string brokerIP);
    void setBrokerPort(int brokerPort);
    void setClientID(std::string clientID);
    void keepAlive();
    void update();
};

#endif /* MQTTHANDLER_H */
