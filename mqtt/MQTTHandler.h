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
#include "StatusMessage.h"

class MQTTHandler : public Observer, public Subject
{
private:
    std::string mNetworkID = "PICOQ5-9k195";
    std::string mNetworkPW = "Q5-9k195";
    std::string mBrokerIP = "192.168.137.1";
    int mBrokerPort = 1883;
    std::string mClientID = "PicoW-G06";
    std::shared_ptr<IPStack> mIPStack;
    std::shared_ptr<MQTT::Client<IPStack, Countdown, 256>> mMQTTClient;
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
    bool connect();
    void reconnect(std::string networkID, std::string networkPW, std::string brokerIP);
    void send(StatusMessage statusMessage);
    void keepAlive();
    void update() override;
    void addObserver(std::shared_ptr<Observer> observer) override;
    void notifyObservers() override;
    void setNetworkID(std::string networkID);
    void setNetworkPW(std::string networkPW);
    void setBrokerIP(std::string brokerIP);
    enum topicNumber : int
    {
        TOPIC_STATUS,
        TOPIC_SETTINGS
    };
};

#endif /* MQTTHANDLER_H */
