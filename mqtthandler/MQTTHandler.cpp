#include "MQTTHandler.h"

using namespace std;

MQTTHandler::MQTTHandler()
{
    mMQTTClient.setDefaultMessageHandler(mMQTTMessageHandler);
}

bool MQTTHandler::mMQTTConnect()
{
    char cString[80] = { '\0' };

    strncpy(cString, mClientID.c_str(), 80);
    cString[79] = '\0';

    mRC = mIPStack.connect(mBrokerIP.c_str(), mBrokerPort);
    if (mRC != 1) {
        printf("rc from TCP connect is %d\n", mRC);
    }

    printf("MQTT connecting\n");
    mData = MQTTPacket_connectData_initializer;
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
    mRC = mMQTTClient.subscribe(topic.c_str(), MQTT::QOS2, mMQTTMessageHandler);
    if (mRC != 0) {
        printf("rc from MQTT subscribe is %d\n", mRC);
        return false;
    }
    printf("MQTT subscribed\n");
    return true;
}

void MQTTHandler::mMQTTSendStatus()
{
    char buf[256];
    mMQTTMessage.retained = false;
    mMQTTMessage.dup = false;
    mMQTTMessage.payload = (void *)buf;

    sprintf(buf, "Msg nr: %d QoS 0 message", ++mMessageCount);
    printf("%s\n", buf);
    mMQTTMessage.qos = MQTT::QOS0;
    mMQTTMessage.payloadlen = strlen(buf) + 1;
    mRC = mMQTTClient.publish(mStatusTopic.c_str(), mMQTTMessage);
    printf("Publish rc=%d\n", mRC);
}

void MQTTHandler::mMQTTSendSettings()
{
    char buf[256];
    mMQTTMessage.retained = false;
    mMQTTMessage.dup = false;
    mMQTTMessage.payload = (void *)buf;

    sprintf(buf, "Msg nr: %d QoS 1 message", ++mMessageCount);
    printf("%s\n", buf);
    mMQTTMessage.qos = MQTT::QOS1;
    mMQTTMessage.payloadlen = strlen(buf) + 1;
    mRC = mMQTTClient.publish(mSettingsTopic.c_str(), mMQTTMessage);
    printf("Publish rc=%d\n", mRC);
}

void MQTTHandler::mMQTTMessageHandler(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char *)message.payload);
    // printf("Topic %s", message.)
}

void MQTTHandler::connect()
{
    mMQTTEnabled = mMQTTClient.isConnected();

    if (mMQTTEnabled)
    {
        mMQTTClient.disconnect();
        mIPStack.disconnect();
    }

    mMQTTEnabled = mMQTTConnect();

    if (mMQTTEnabled)
    {
        mMQTTSubscribe(mStatusTopic);
        mMQTTSubscribe(mSettingsTopic);
    }
}

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
    if (!mMQTTClient.isConnected()) {
        printf("Not connected...\n");
        mRC = mMQTTClient.connect(mData);
        if (mRC != 0) {
            printf("rc from MQTT connect is %d\n", mRC);
        }
    }

    if (mMQTTEnabled)
    {
        mMQTTSendStatus();
        // mMQTTSendSettings();
    }
}

void MQTTHandler::setNetworkID(string networkID) { mNetworkID = networkID; }

void MQTTHandler::setNetworkPW(string networkPW) { mNetworkPW = networkPW; }

void MQTTHandler::setBrokerIP(string brokerIP) { mBrokerIP = brokerIP; }

void MQTTHandler::setBrokerPort(int brokerPort) { mBrokerPort = brokerPort; }

void MQTTHandler::setClientID(string clientID) { mClientID = clientID; }