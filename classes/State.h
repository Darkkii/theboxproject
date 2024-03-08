#ifndef PICO_MODBUS_STATE_H
#define PICO_MODBUS_STATE_H

#include <vector>
#include <memory>
#include <iostream>

#include "ssd1306.h"
#include "GMP252.h"
#include "HMP60.h"
#include "I2CHandler.h"
#include "MIO12V.h"
#include "SDP600.h"

class State: public Observer {
private:
    ssd1306 mDisplay;
    std::shared_ptr<GMP252> mGMP252;
    std::shared_ptr<HMP60> mHMP60;
    std::shared_ptr<MIO12V> mFanController;
    std::shared_ptr<SDP600> mSDP600;
    std::shared_ptr<State> mState;

    bool mMode_auto;
    float mCO2;
    float mTemperature;
    float mRH;

    int16_t mCurrentPressure;
    int16_t mTargetPressure;
    uint16_t mCurrentFanSpeed;
    uint16_t mTargetFanSpeed;

    bool mStateChanged;

    char mCO2_line[17];
    char mTemp_line[17];
    char mRH_line[17];
    char mHeader_line[17];
    char mPres_line[17];
    char mFan_line[17];

    void fetchValues();
    void writeLines();
    void updateOLED();
    void updateCout();
public:
    State(const std::shared_ptr<I2CHandler>& i2cHandler,
          const std::shared_ptr<GMP252>& gmp252,
          const std::shared_ptr<HMP60>& hmp60,
          const std::shared_ptr<MIO12V>& mio12V,
          const std::shared_ptr<SDP600>& sdp600);
    void update() override;

    void toggleMode();
    void setTargetFanSpeed();
    void setTargetPressure();


};


#endif //PICO_MODBUS_STATE_H
