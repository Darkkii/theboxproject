#include "MQTTHandler.h"
// #include <cstring>
#include "cJSON.h"

using namespace std;

static void mqttMessageHandler(MQTT::MessageData &md);

MQTTHandler::MQTTHandler()
{}

void MQTTHandler::connect()
{
    if (mMQTTClient.isConnected())
    {
        mMQTTClient.disconnect();
        mIPStack.disconnect();
    }

    if (mMQTTConnect())
    {
        mMQTTEnabled = true;
        // mMQTTSubscribe(mStatusTopic);
        mMQTTSubscribe(mSettingsTopic);
    }
}

void MQTTHandler::setNetworkID(string networkID) { mNetworkID = networkID; }

void MQTTHandler::setNetworkPW(string networkPW) { mNetworkPW = networkPW; }

void MQTTHandler::setBrokerIP(string brokerIP) { mBrokerIP = brokerIP; }

void MQTTHandler::setBrokerPort(int brokerPort) { mBrokerPort = brokerPort; }

void MQTTHandler::setClientID(string clientID) { mClientID = clientID; }

void MQTTHandler::keepAlive()
{
    if (mMQTTEnabled)
    {
        cyw43_arch_poll(); // obsolete? - see below
        mMQTTClient.yield(100); // socket that client uses calls cyw43_arch_poll()
    }
}

void MQTTHandler::update()
{
    // if (!mMQTTClient.isConnected()) {
    //     printf("Not connected...\n");
    //     mRC = mMQTTClient.connect(mData);
    //     if (mRC != 0) {
    //         printf("rc from MQTT connect is %d\n", mRC);
    //     }

    // }

    if (mMQTTEnabled)
    {
        mMQTTSendStatus();
        // mMQTTSendSettings();
    }

}

bool MQTTHandler::mMQTTConnect()
{
    char cString[80] = { '\0' };

    strncpy(cString, mClientID.c_str(), 80);
    cString[79] = '\0';

    int mRC = mIPStack.connect(mBrokerIP.c_str(), mBrokerPort);
    if (mRC != 1) {
        printf("rc from TCP connect is %d\n", mRC);
    }

    printf("MQTT connecting\n");
    mData.MQTTVersion = 3;
    mData.clientID.cstring = cString;
    mRC = mMQTTClient.connect(mData);
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
    mRC = mMQTTClient.subscribe(topic.c_str(), MQTT::QOS2, mqttMessageHandler);
    if (mRC != 0) {
        printf("rc from MQTT subscribe is %d\n", mRC);
        return false;
    }
    printf("MQTT subscribed\n");
    return true;
}

void MQTTHandler::mMQTTSendStatus()
{
    char buf[100];
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;

    sprintf(buf, "Msg nr: %d QoS 0 message", ++mMessageCount);
    printf("%s\n", buf);
    message.qos = MQTT::QOS0;
    message.payloadlen = strlen(buf) + 1;
    mRC = mMQTTClient.publish(mStatusTopic.c_str(), message);
    printf("Publish rc=%d\n", mRC);
}

void MQTTHandler::mMQTTSendSettings()
{
    char buf[100];
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;

    sprintf(buf, "Msg nr: %d QoS 1 message", ++mMessageCount);
    printf("%s\n", buf);
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf) + 1;
    mRC = mMQTTClient.publish(mSettingsTopic.c_str(), message);
    printf("Publish rc=%d\n", mRC);
}

void MQTTHandler::mMQTTMessageHandler(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *)message.payload);
}

static void mqttMessageHandler(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *)message.payload);
}