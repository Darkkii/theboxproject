#include "MQTTHandler.h"

MQTTHandler::MQTTHandler()
{
    const char *topic = "test-topic";

    // IPStack ipstack("PICO-Q59k95", "Q5-9k195");
    // auto client = MQTT::Client<IPStack, Countdown>(ipstack);

    // int rc = ipstack.connect("192.168.137.1", 1883);
    // if (rc != 1) {
    //     printf("rc from TCP connect is %d\n", rc);
    // }

    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"PicoW-sample";
    // rc = client.connect(data);
    // if (rc != 0) {
    //     printf("rc from MQTT connect is %d\n", rc);
    //     while (true) {
    //         tight_loop_contents();
    //     }
    // }
    printf("MQTT connected\n");

    // We subscribe QoS2. Messages sent with lower QoS will be delivered using the QoS they were sent with
    // rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    // if (rc != 0) {
    //     printf("rc from MQTT subscribe is %d\n", rc);
    // }
    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
    int msg_count = 0;


    // if (time_reached(mqtt_send)) {
    //     mqtt_send = delayed_by_ms(mqtt_send, 2000);
    //     if (!client.isConnected()) {
    //         printf("Not connected...\n");
    //         rc = client.connect(data);
    //         if (rc != 0) {
    //             printf("rc from MQTT connect is %d\n", rc);
    //         }

    //     }
    //     char buf[100];
    //     int rc = 0;
    //     MQTT::Message message;
    //     message.retained = false;
    //     message.dup = false;
    //     message.payload = (void *)buf;
    //     switch (mqtt_qos) {
    //         case 0:
    //             // Send and receive QoS 0 message
    //             sprintf(buf, "Msg nr: %d QoS 0 message", ++msg_count);
    //             printf("%s\n", buf);
    //             message.qos = MQTT::QOS0;
    //             message.payloadlen = strlen(buf) + 1;
    //             rc = client.publish(topic, message);
    //             printf("Publish rc=%d\n", rc);
    //             ++mqtt_qos;
    //             break;
    //         case 1:
    //             // Send and receive QoS 1 message
    //             sprintf(buf, "Msg nr: %d QoS 1 message", ++msg_count);
    //             printf("%s\n", buf);
    //             message.qos = MQTT::QOS1;
    //             message.payloadlen = strlen(buf) + 1;
    //             rc = client.publish(topic, message);
    //             printf("Publish rc=%d\n", rc);
    //             ++mqtt_qos;
    //             break;
    //         case 2:
    //             // Send and receive QoS 2 message
    //             sprintf(buf, "Msg nr: %d QoS 2 message", ++msg_count);
    //             printf("%s\n", buf);
    //             message.qos = MQTT::QOS2;
    //             message.payloadlen = strlen(buf) + 1;
    //             rc = client.publish(topic, message);
    //             printf("Publish rc=%d\n", rc);
    //             ++mqtt_qos;
    //             break;
    //         default:
    //             mqtt_qos = 0;
    //             break;
    //     }
    // }
}

void messageArrived(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *)message.payload);
}