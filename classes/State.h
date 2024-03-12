#ifndef PICO_MODBUS_STATE_H
#define PICO_MODBUS_STATE_H

#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>

#include "ssd1306.h"
#include "GMP252.h"
#include "HMP60.h"
#include "I2CHandler.h"
#include "MIO12V.h"
#include "SDP600.h"
#include "MQTTHandler.h"

#define PRESSURE_ADJUSTMENT_LATENCY_US 2000000
#define PRESSURE_TARGET_ACCURACY 4
#define MIN_PRESSURE_TARGET 10
#define OLED_MAX_STR_WIDTH 16


class State: public Observer {
private:
    ssd1306 mDisplay;
    std::shared_ptr<GMP252> mGMP252;
    std::shared_ptr<HMP60> mHMP60;
    std::shared_ptr<MIO12V> mFanController;
    std::shared_ptr<SDP600> mSDP600;
    std::shared_ptr<State> mState;
    std::shared_ptr<MQTTHandler> mMQTTHandler;

    bool mMode_auto;
    bool mMQTT_input;
    float mCO2;
    float mTemperature;
    float mRH;

    uint16_t mCurrentFanSpeed;
    uint16_t mTargetFanSpeed;
    uint16_t mInputFanSpeed;
    int16_t mCurrentPressure;
    int16_t mTargetPressure;
    int16_t mInputPressure;

    absolute_time_t mFanAdjustmentTimer;
    uint32_t mPrevFanAdjustment_us;

    std::stringstream mCO2_line;
    std::stringstream mTemp_line;
    std::stringstream mRH_line;
    const std::string mHeader_line = "     Curr Tar";
    std::stringstream mPres_line;
    std::stringstream mFan_line;

    enum MQTTinput_stage_enum {
        networkID,
        networkPW,
        brokerIP
    };

    enum MQTTinput_stage_enum mMQTT_input_stage;

    char mInputChar;
    std::string mNetworkID;
    std::string mNetworkPW;
    std::string mBrokerIP;

    void fetchValues();
    void writeStatusLines();
    void updateOLED();
    void OLED_VentStatus();
    void OLED_MQTTCredentials();
    void OLED_MQTTConnection();
    void updateCout();
public:
    State(const std::shared_ptr<I2CHandler>& i2cHandler,
          const std::shared_ptr<GMP252>& gmp252,
          const std::shared_ptr<HMP60>& hmp60,
          const std::shared_ptr<MIO12V>& mio12V,
          const std::shared_ptr<SDP600>& sdp600,
          const std::shared_ptr<MQTTHandler>& mqttHandler);
    void update() override;

    void toggleMode();
    void toggle_MQTT_input();
    void setTarget();
    void backspace();
    void clockwise();
    void counter_clockwise();
    void adjustInputFanSpeed(int x);
    void adjustInputPressure(int x);
    void adjustFan();
};


#endif //PICO_MODBUS_STATE_H
