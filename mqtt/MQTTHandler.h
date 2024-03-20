#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include "Observer.h"
#include "SettingsMessage.h"
#include "StatusMessage.h"
#include "Subject.h"

#include <memory>
#include <string>
#include <vector>

class MQTTHandler : public Subject
{
  private:
    std::string mNetworkID;
    std::string mNetworkPW;
    std::string mBrokerIP;
    int mBrokerPort = 1883;
    std::string mClientID = "PicoW-G06";
    std::shared_ptr<IPStack> mIPStack = nullptr;
    std::shared_ptr<MQTT::Client<IPStack, Countdown, 256>> mMQTTClient =
        nullptr;
    int mRC = 0;
    int mMessageCount = 0;
    bool mIPLinkup = false;
    bool mMQTTEnabled = false;
    const std::string mStatusTopic = "controller/status";
    const std::string mSettingsTopic = "controller/settings";
    MQTTPacket_connectData mData;
    std::vector<std::shared_ptr<Observer>> mObservers;
    bool mMQTTConnect();
    bool mMQTTSubscribe(const std::string topic);
    typedef void (*messageHandlerFptr)(MQTT::MessageData &md);
    messageHandlerFptr mMessageHandler;
    SettingsMessage mSettings;

  public:
    explicit MQTTHandler(messageHandlerFptr messageHandler);
    enum topicNumber : int;
    bool
    connect(std::string networkID, std::string networkPW, std::string brokerIP);
    void send(StatusMessage statusMessage);
    void keepAlive();
    void addObserver(std::shared_ptr<Observer> observer) override;
    void notifyObservers() override;
    void setSettingsMessage(SettingsMessage message);

    enum topicNumber : int
    {
        TOPIC_STATUS,
        TOPIC_SETTINGS
    };
};

#endif /* MQTTHANDLER_H */
