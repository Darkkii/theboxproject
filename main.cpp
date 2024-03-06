#include <stdio.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "ssd1306.h"
#include "GMP252.h"
#include "HMP60.h"
#include "SDP600.h"
#include "MIO12V.h"
#include "PicoSW.h"

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#if 0
#define UART_NR 0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#else
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif

#define BAUD_RATE 9600

//#define USE_MODBUS
//#define USE_MQTT
//#define USE_SSD1306
#define TEST_SENSORS
#define TEST_FAN_MOTOR
#define TEST_SW
#define TEST_PRESSURE_SENSOR


#ifdef USE_SSD1306
static const uint8_t raspberry26x32[] =
{ 0x0, 0x0, 0xe, 0x7e, 0xfe, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xfe,
 0xfe, 0xff, 0xff,0xff, 0xff, 0xff, 0xfe, 0x7e,
 0x1e, 0x0, 0x0, 0x0, 0x80, 0xe0, 0xf8, 0xfd,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd,
 0xf8, 0xe0, 0x80, 0x0, 0x0, 0x1e, 0x7f, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0x7f, 0x1e, 0x0, 0x0,
 0x0, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x3f, 0x3f,
 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f,
 0x3f, 0x1f, 0x1f, 0xf, 0x7, 0x3, 0x0, 0x0 };
#endif // USE_SSD1306

#ifdef USE_MQTT
void messageArrived(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *)message.payload);
}

static const char *topic = "test-topic";
#endif // USE_MQTT

using namespace std;

union pressure_conversion {
    uint16_t u16;
    int16_t _int;
};

int main()
{
    const uint led_pin = 22;
    const uint button = 9;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);

    // Initialize chosen serial port
    stdio_init_all();

    uint16_t fanSpeed = 0;

    printf("\nBoot\n");

#ifdef TEST_SENSORS
    auto modbus_poll = make_timeout_time_ms(3000);
    GMP252 gmp252{ rtu_client };
    HMP60 hmp60{ rtu_client };
    gmp252.update();
    sleep_ms(5);
    gmp252.update();
    sleep_ms(5);
    hmp60.update();
    sleep_ms(5);
#endif // TEST_SENSORS

#ifdef TEST_SW
    PicoSW picoSW(true, true, true);
    PicoSW_event swEvent;
#endif // TEST_SW

#ifdef USE_SSD1306
    // I2C is "open drain",
    // pull ups to keep signal high when no data is being sent
    ssd1306 display(i2c1);
    display.fill(0);
    display.text("Hello", 0, 0);
    mono_vlsb rb(raspberry26x32, 26, 32);
    display.blit(rb, 20, 20);
    display.rect(15, 15, 35, 45, 1);
    display.line(60, 5, 120, 60, 1);
    display.line(60, 60, 120, 5, 1);
    display.show();
#if 1
    for (int i = 0; i < 128; ++i) {
        sleep_ms(50);
        display.scroll(1, 0);
        display.show();
    }
    display.text("Done", 20, 20);
    display.show();
#endif // display scroll

#endif // USE_SSD1306

#ifdef USE_MQTT
    //IPStack ipstack("SSID", "PASSWORD"); // example
    IPStack ipstack("SmartIotMQTT", "SmartIot"); // example
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);

    int rc = ipstack.connect("192.168.1.10", 1883);
    if (rc != 1) {
        printf("rc from TCP connect is %d\n", rc);
    }

    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"PicoW-sample";
    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
        while (true) {
            tight_loop_contents();
        }
    }
    printf("MQTT connected\n");

    // We subscribe QoS2. Messages sent with lower QoS will be delivered using the QoS they were sent with
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    }
    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
    int msg_count = 0;
#endif // USE_MQTT

#ifdef USE_MODBUS
    auto uart_modbus{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client_modbus{ std::make_shared<ModbusClient>(uart_modbus) };
    ModbusRegister rh(rtu_client_modbus, 241, 256);
    auto modbus_poll = make_timeout_time_ms(3000);
#endif // USE_MODUS

#ifdef TEST_FAN_MOTOR
    MIO12V fanController(rtu_client);
    sleep_ms(100);
    fanController.setFanSpeed(fanSpeed);
    printf("Fan Speed: %u\n", fanSpeed);
#endif // TEST_FAN_MOTOR
#ifdef TEST_PRESSURE_SENSOR
    SDP600 sdp600{};
#endif // TEST_PRESSURE_SENSOR

    while (true) {
#ifdef USE_MODBUS
        if (time_reached(modbus_poll)) {
            gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
            modbus_poll = delayed_by_ms(modbus_poll, 3000);
            printf("RH=%5.1f%%\n", rh.read() / 10.0);
        }
#endif // USE_MODBUS

#ifdef USE_MQTT
        if (time_reached(mqtt_send)) {
            mqtt_send = delayed_by_ms(mqtt_send, 2000);
            if (!client.isConnected()) {
                printf("Not connected...\n");
                rc = client.connect(data);
                if (rc != 0) {
                    printf("rc from MQTT connect is %d\n", rc);
                }

            }
            char buf[100];
            int rc = 0;
            MQTT::Message message;
            message.retained = false;
            message.dup = false;
            message.payload = (void *)buf;
            switch (mqtt_qos) {
                case 0:
                    // Send and receive QoS 0 message
                    sprintf(buf, "Msg nr: %d QoS 0 message", ++msg_count);
                    printf("%s\n", buf);
                    message.qos = MQTT::QOS0;
                    message.payloadlen = strlen(buf) + 1;
                    rc = client.publish(topic, message);
                    printf("Publish rc=%d\n", rc);
                    ++mqtt_qos;
                    break;
                case 1:
                    // Send and receive QoS 1 message
                    sprintf(buf, "Msg nr: %d QoS 1 message", ++msg_count);
                    printf("%s\n", buf);
                    message.qos = MQTT::QOS1;
                    message.payloadlen = strlen(buf) + 1;
                    rc = client.publish(topic, message);
                    printf("Publish rc=%d\n", rc);
                    ++mqtt_qos;
                    break;
#if MQTTCLIENT_QOS2
                case 2:
                    // Send and receive QoS 2 message
                    sprintf(buf, "Msg nr: %d QoS 2 message", ++msg_count);
                    printf("%s\n", buf);
                    message.qos = MQTT::QOS2;
                    message.payloadlen = strlen(buf) + 1;
                    rc = client.publish(topic, message);
                    printf("Publish rc=%d\n", rc);
                    ++mqtt_qos;
                    break;
#endif // MQTTCLIENT_QOS2
                default:
                    mqtt_qos = 0;
                    break;
            }
        }

        cyw43_arch_poll(); // obsolete? - see below
        client.yield(100); // socket that client uses calls cyw43_arch_poll()
#endif // USE_MQTT

#ifdef TEST_SENSORS
        if (time_reached(modbus_poll)) {
            gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
            modbus_poll = delayed_by_ms(modbus_poll, 3000);
            printf("CO2: %.0f\n", gmp252.getCO2());
            printf("Temp (GMP252): %.1f\n", gmp252.getTemperature());
            printf("RH: %.1f\n", hmp60.getRelativeHumidity());
            printf("Temp (HMP60): %.1f\n", hmp60.getTemperature());
#ifdef TEST_PRESSURE_SENSOR
            printf("Pressure: %hd\n", sdp600.getPressure());
#endif // TEST_PRESSURE_SENSOR
            printf("----------------\n");
            gmp252.update();
            sleep_ms(5);
            hmp60.update();
            sleep_ms(5);
        }
#endif // TEST_SENSORS

#ifdef TEST_SW
        while ((swEvent = picoSW.getEvent()) != NO_EVENT) {
            switch (swEvent) {
#ifdef TEST_FAN_MOTOR
                case CLOCKWISE:
                    if (fanSpeed < 1000) fanSpeed += 2;
                    break;
                case COUNTER_CLOCKWISE:
                    if (fanSpeed > 0) fanSpeed -= 2;
                    break;
                case SW_0_PRESS:
#ifdef TEST_PRESSURE_SENSOR
                    cout << "SW_0 press" << endl;
                    break;
#endif // TEST_PRESSURE_SENSOR
#else
                case CLOCKWISE:
                    cout << "Rot Clockwise!" << endl;
                    break;
                case COUNTER_CLOCKWISE:
                    cout << "Rot Counter-Clockwise!" << endl;
                    break;
#endif // TEST_SENSORS
            }
        }
#ifdef TEST_FAN_MOTOR
        if (fanController.getFanSpeed() != fanSpeed) {
            fanController.setFanSpeed(fanSpeed);
            cout << "New fanspeed: " << fanSpeed << endl;
        }
#endif // TEST_FAN_MOTOR
#endif // TEST_SW

    }
}