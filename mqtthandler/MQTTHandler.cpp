#include "MQTTHandler.h"

using namespace std;

MQTTHandler::MQTTHandler()
{
    mMQTTClient.setDefaultMessageHandler(sMQTTMessageHandler);
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
    mRC = mMQTTClient.subscribe(topic.c_str(), MQTT::QOS2, sMQTTMessageHandler);
    if (mRC != 0) {
        printf("rc from MQTT subscribe is %d\n", mRC);
        return false;
    }
    printf("MQTT subscribed\n");
    return true;
}

void MQTTHandler::sMQTTMessageHandler(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("On topic %.*s\n", md.topicName.lenstring.len, md.topicName.lenstring.data);
    printf("Payload %.*s\n", (int)message.payloadlen, (char *)message.payload);
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
        mMQTTSubscribe(mSettingsTopic);
    }
}

void MQTTHandler::send(topicNumber topicNumber, std::string message)
{
    if (message.length() < 256)
    {
        MQTT::Message mqttMessage;
        string topic = topicNumber ? mSettingsTopic : mStatusTopic;
        char buf[256];

        mqttMessage.qos = topicNumber ? MQTT::QOS1 : MQTT::QOS0;
        mqttMessage.retained = false;
        mqttMessage.dup = false;
        mqttMessage.payload = (void *)buf;

        sprintf(buf, message.c_str(), ++mMessageCount);
        printf("Sent: %s\n", buf);
        mqttMessage.payloadlen = strlen(buf);
        mRC = mMQTTClient.publish(topic.c_str(), mqttMessage);
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
        // TODO: Change to fetch actual data from either JSON object or sensors.
        send(TOPIC_STATUS, R"({"nr": )" + to_string(mMessageCount) + R"(, "speed": 99, "setpoint": 40, "pressure": 16, "auto": false, "error": false, "co2": 300, "rh": 37, "temp": 20})");
    }
}

void MQTTHandler::addObserver(shared_ptr<Observer> observer) { mObservers.push_back(observer); }

void MQTTHandler::notifyObservers() { printf("test\n"); for (auto &&observer : mObservers) { observer->update(); } }

void MQTTHandler::setNetworkID(string networkID) { mNetworkID = networkID; }

void MQTTHandler::setNetworkPW(string networkPW) { mNetworkPW = networkPW; }

void MQTTHandler::setBrokerIP(string brokerIP) { mBrokerIP = brokerIP; }

void MQTTHandler::setBrokerPort(int brokerPort) { mBrokerPort = brokerPort; }

void MQTTHandler::setClientID(string clientID) { mClientID = clientID; }