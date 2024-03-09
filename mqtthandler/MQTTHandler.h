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
    std::string mNetworkID = "FORPONY";
    std::string mNetworkPW = "tr4v3ll3r";
    std::string mBrokerIP = "192.168.1.50";
    std::string mClientID = "PicoW-G06";
    const std::string mStatusTopic = "controller/status";
    const std::string mSettingsTopic = "controller/settings";
    IPStack mIPStack = IPStack(mNetworkID.c_str(), mNetworkPW.c_str());
    MQTTPacket_connectData mData;
    MQTT::Message mMQTTMessage;
    bool mMQTTConnect();
    bool mMQTTSubscribe(const std::string topic);
    void mMQTTSendStatus();
    void mMQTTSendSettings();
    static void mMQTTMessageHandler(MQTT::MessageData &md);

public:
    MQTTHandler();
    void connect();
    void keepAlive();
    void update();
    void setNetworkID(std::string networkID);
    void setNetworkPW(std::string networkPW);
    void setBrokerIP(std::string brokerIP);
    void setBrokerPort(int brokerPort);
    void setClientID(std::string clientID);
    void keepAlive();
    void update();
};

#endif /* MQTTHANDLER_H */
