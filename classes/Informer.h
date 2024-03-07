#ifndef PICO_MODBUS_INFORMER_H
#define PICO_MODBUS_INFORMER_H

#include <memory>

#include "Observer.h"
#include "ssd1306.h"
#include "GMP252.h"
#include "HMP60.h"
#include "I2CHandler.h"
#include "MIO12V.h"
#include "SDP600.h"

class Informer: public Observer {
private:
    ssd1306 mDisplay;
    std::shared_ptr<GMP252> mGMP252;
    std::shared_ptr<HMP60> mHMP60;
    std::shared_ptr<MIO12V> mFanController;
    std::shared_ptr<SDP600> mSDP600;

    bool mManual;
    float mCO2;
    float mTemperature;
    float mRH;
    int16_t mCurrentPressure;
    int16_t mTargetPressure;
    float mCurrentFanSpeed;
    float mTargetFanSpeed;

    void fetchValues();
    void updateOLED();
    void updateCout();
public:
    Informer(const std::shared_ptr<I2CHandler>& i2cHandler,
             const std::shared_ptr<GMP252>& gmp252,
             const std::shared_ptr<HMP60>& hmp60,
             const std::shared_ptr<MIO12V>& mio12V,
             const std::shared_ptr<SDP600>& sdp600);
    void update() override;
};


#endif //PICO_MODBUS_INFORMER_H
