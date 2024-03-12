#include "MQTTHandler.h"

#include <utility>

using namespace std;

MQTTHandler::MQTTHandler(messageHandlerFptr messageHandler) :
    mMessageHandler{ messageHandler },
    mSettings{ SettingsMessage(false, 0) }
{}

bool MQTTHandler::mMQTTConnect()
{
    char cString[80] = { '\0' };

    strncpy(cString, mClientID.c_str(), 80);
    cString[79] = '\0';

    mRC = mIPStack->connect(mBrokerIP.c_str(), mBrokerPort);
    if (mRC != 1) {
        printf("rc from TCP connect is %d\n", mRC);
    }
    if (mRC != 0) { return false; }

    printf("MQTT connecting\n");
    mMQTTClient->setDefaultMessageHandler(mMessageHandler);
    mData = MQTTPacket_connectData_initializer;
    mData.MQTTVersion = 3;
    mData.clientID.cstring = cString;
    mRC = mMQTTClient->connect(mData);
    if (mRC != 0) {
        printf("rc from MQTT connect is %d\n", mRC);
        return false;
    }
    printf("MQTT connected\n");
    return true;
}

bool MQTTHandler::mMQTTSubscribe(const string topic)
{
    // We subscribe QoS2. Messages sent with lower QoS will be delivered using the QoS they were sent with
    mRC = mMQTTClient->subscribe(topic.c_str(), MQTT::QOS0, mMessageHandler);
    if (mRC != 0) {
        printf("rc from MQTT subscribe is %d\n", mRC);
        return false;
    }
    printf("MQTT subscribed\n");
    return true;
}

bool MQTTHandler::connect()
{
    int retry = 0;

    if (mIPStack != nullptr)
    {
        mIPStack->disconnect();
        mIPStack.reset();
    }

    if (mMQTTEnabled)
    {
        mMQTTClient->disconnect();
        mMQTTClient.reset();
    }

    mMQTTEnabled = false;


    mIPStack = make_shared<IPStack>(mNetworkID.c_str(), mNetworkPW.c_str());

    while (!mIPStack->isLinkUp() && ++retry < 3)
    {
        printf("Wifi connection error. Retrying...\n");
        mIPStack->retry(mNetworkID.c_str(), mNetworkPW.c_str());
    }

    if (mIPStack->isLinkUp())
    {
        mIPLinkup = true;
        mMQTTClient = make_shared<MQTT::Client<IPStack, Countdown, 256>>(*mIPStack);
        mMQTTEnabled = mMQTTConnect();

        if (mMQTTEnabled)
        {
            mMQTTSubscribe(mSettingsTopic);
            return true;
        }
    }

    printf("Wifi network unavailable.\n");
    mIPStack->disconnect();
    mIPStack.reset();
    return false;
}

bool MQTTHandler::connect(std::string networkID, std::string networkPW, std::string brokerIP)
{
    mNetworkID = networkID;
    mNetworkPW = networkPW;
    mBrokerIP = brokerIP;

    return connect();
}

void MQTTHandler::send(StatusMessage statusMessage)
{
    statusMessage.setMessageNumber(++mMessageCount);
    string message = statusMessage;
    if (message.length() < 256)
    {
        MQTT::Message mqttMessage;
        char buf[256];

        mqttMessage.qos = MQTT::QOS0;
        mqttMessage.retained = false;
        mqttMessage.dup = false;
        mqttMessage.payload = (void *)buf;

        sprintf(buf, message.c_str(), mMessageCount);
        printf("Sent: %s\n", buf);
        mqttMessage.payloadlen = strlen(buf);
        mRC = mMQTTClient->publish(mStatusTopic.c_str(), mqttMessage);
    }
}

void MQTTHandler::keepAlive()
{
    if (mMQTTEnabled)
    {
        cyw43_arch_poll(); // obsolete? - see below
        mMQTTClient->yield(100); // socket that client uses calls cyw43_arch_poll()
    }
}

void MQTTHandler::update()
{
    if (mMQTTEnabled)
    {
        if (!mMQTTClient->isConnected()) {
            printf("Not connected...\n");
            mRC = mMQTTClient->connect(mData);
            if (mRC != 0) {
                printf("rc from MQTT connect is %d\n", mRC);
            }
        }
        // TODO: Send status message to MQTT
    }
}

void MQTTHandler::addObserver(shared_ptr<Observer> observer) { mObservers.push_back(observer); }

void MQTTHandler::notifyObservers() { for (auto &&observer : mObservers) { observer->update(); } }

void MQTTHandler::setSettingsMessage(const SettingsMessage message)
{
    mSettings = message;
}

SettingsMessage MQTTHandler::getSettingsMessage()
{
    return mSettings;
}
