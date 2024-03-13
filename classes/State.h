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
#include "StatusMessage.h"
#include "SettingsMessage.h"
#include "Eeprom.h"

#define PRESSURE_ADJUSTMENT_LATENCY_US 3000000
#define PRESSURE_TARGET_ACCURACY 4
#define MIN_PRESSURE_TARGET 10
#define OLED_MAX_STR_WIDTH 16

#define EEPROM_REG_NETWORK_ID 0x00
#define EEPROM_REG_NETWORK_PW 0x40
#define EEPROM_REG_BROKER_IP  0x80
#define EEPROM_REG_MODE       0xC0
#define EEPROM_REG_TAR_PRES   0x100
#define EEPROM_REG_TAR_FAN    0x140

class State : public Observer
{
private:
    ssd1306 mDisplay;
    std::shared_ptr<GMP252> mGMP252;
    std::shared_ptr<HMP60> mHMP60;
    std::shared_ptr<MIO12V> mFanController;
    std::shared_ptr<SDP600> mSDP600;
    std::shared_ptr<State> mState;
    std::shared_ptr<MQTTHandler> mMQTTHandler;
    std::shared_ptr<Eeprom> mEEPROM;

    bool mMode_auto;
    bool mMQTT_input{false};
    float mCO2{0};
    float mTemperature{0};
    float mRH{0};

    uint16_t mCurrentFanSpeed{0};
    uint16_t mTargetFanSpeed;
    uint16_t mInputFanSpeed;
    int16_t mCurrentPressure{0};
    int16_t mTargetPressure;
    int16_t mInputPressure;

    uint32_t mPrevFanAdjustment_us{0};

    std::stringstream mCO2_line;
    std::stringstream mTemp_line;
    std::stringstream mRH_line;
    const std::string mHeader_line = "     Curr Tar";
    std::stringstream mPres_line;
    std::stringstream mFan_line;

    enum MQTTinput_stage_enum
    {
        networkID,
        networkPW,
        brokerIP
    };

    enum MQTTinput_stage_enum mMQTT_input_stage{networkID};

    char mInputChar{'0'};
    int mInputNumber{0};
    int mBrokerPeriods{0};
    std::string mNetworkID;
    std::string mNetworkPW;
    std::string mBrokerIP;

    void fetchValues();
    void writeStatusLines();
    void updateOLED();
    void OLED_VentStatus();
    void OLED_MQTTCredentials();
    void updateCout();
public:
    State(const std::shared_ptr<I2CHandler> &i2cHandler,
          const std::shared_ptr<GMP252> &gmp252,
          const std::shared_ptr<HMP60> &hmp60,
          const std::shared_ptr<MIO12V> &mio12V,
          const std::shared_ptr<SDP600> &sdp600,
          const std::shared_ptr<MQTTHandler> &mqttHandler,
          const std::shared_ptr<Eeprom> &eeprom);
    void update() override;
    void update(SettingsMessage sm) override;

    void toggleMode();
    void toggle_MQTT_input();
    void setTarget();
    void backspace();
    void clockwise();
    void counter_clockwise();
    void adjustInputFanSpeed(int x);
    void adjustInputPressure(int x);
    void adjustFan();
    bool ConnectMQTT(std::string networkID, std::string networkPW, std::string BrokerIP);
    void updateMQTT();
};


#endif //PICO_MODBUS_STATE_H
