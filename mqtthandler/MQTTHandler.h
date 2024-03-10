#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H

#include <string>
#include <vector>
#include <memory>
#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "Observer.h"
#include "Subject.h"

class MQTTHandler : public Observer, public Subject
{
private:
    std::string mNetworkID = "FORPONY";
    std::string mNetworkPW = "tr4v3ll3r";
    std::string mBrokerIP = "192.168.1.50";
    int mBrokerPort = 1883;
    std::string mClientID = "PicoW-G06";
    IPStack mIPStack = IPStack(mNetworkID.c_str(), mNetworkPW.c_str());
    MQTT::Client<IPStack, Countdown, 256> mMQTTClient = MQTT::Client<IPStack, Countdown, 256>(mIPStack);
    int mRC = 0;
    int mMessageCount = 0;
    bool mMQTTEnabled = false;
    const std::string mStatusTopic = "controller/status";
    const std::string mSettingsTopic = "controller/settings";
    MQTTPacket_connectData mData;
    std::vector<std::shared_ptr<Observer>> mObservers;
    bool mMQTTConnect();
    bool mMQTTSubscribe(const std::string topic);
    typedef void (*messageHandlerFptr)(MQTT::MessageData &md);
    messageHandlerFptr mMessageHandler;

public:
    MQTTHandler(messageHandlerFptr messageHandler);
    enum topicNumber : int;
    void connect();
    void send(topicNumber topicNumber, std::string message);
    void keepAlive();
    void addObserver(std::shared_ptr<Observer> observer) override;
    void notifyObservers() override;
    void setNetworkID(std::string networkID);
    void setNetworkPW(std::string networkPW);
    void setBrokerIP(std::string brokerIP);
    void setBrokerPort(int brokerPort);
    void setClientID(std::string clientID);
    enum topicNumber : int
    {
        TOPIC_STATUS,
        TOPIC_SETTINGS
    };
};

#endif /* MQTTHANDLER_H */
