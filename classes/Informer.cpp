#include "Informer.h"

Informer::Informer(const std::shared_ptr<I2CHandler>& i2cHandler,
                   const std::shared_ptr<GMP252>& gmp252,
                   const std::shared_ptr<HMP60>& hmp60,
                   const std::shared_ptr<MIO12V>& mio12V,
                   const std::shared_ptr<SDP600>& sdp600) :
                   mDisplay(i2cHandler->getI2CBus(1)),
                   mGMP252{gmp252},
                   mHMP60{hmp60},
                   mFanController(mio12V),
                   mSDP600{sdp600}
{}

void Informer::fetchValues()
{
    mManual;
    mCO2 = mGMP252->getCO2();
    mTemperature = (mGMP252->getTemperature() + mHMP60->getTemperature()) / 2;
    mRH = mHMP60->getRelativeHumidity();
    mCurrentPressure = mSDP600->getPressure() / 240;
    mTargetPressure;
    mCurrentFanSpeed = mFanController->getFanSpeed() / 10;
    mTargetFanSpeed;
}

void Informer::updateOLED()
{
    char CO2[17];
    snprintf(CO2, 17, "%4s: %c%-5.0f %s", "CO2", ' ', mCO2, "ppm");
    mDisplay.text(CO2,0,0);
    char TEMP[17];
    snprintf(TEMP, 17, "%4s: %c%-5.1f %s", "Temp",
             mTemperature < 0 ? '-' : '+', abs(mTemperature), "C");
    mDisplay.text(TEMP,0,9);
    char RH[17];
    snprintf(RH, 17, "%4s:  %-5.1f %s", "RH", mRH, "%");
    mDisplay.text(RH,0,18);

    char header[17] = "      Curr  Tar";
    mDisplay.text(header,0,36);
    char pres[17];
    snprintf(pres, 17, "%4s: %c%-4hd%c%c%-5.0f",
             "Pres",
             mCurrentPressure < 0 ? '-' : '+', abs(mCurrentPressure),
             !mManual ? '>' : ' ',
             mTargetPressure < 0 ? '-' : '+', mTargetPressure);
    mDisplay.text(pres,0,45);
    char fan[17];
    snprintf(fan, 17, "%4s:  %-4.0f%c %-5.0f",
             "Fan", mCurrentFanSpeed,
             mManual ? '>' : ' ', mTargetFanSpeed);
    mDisplay.text(fan,0,54);
    mDisplay.show();
}

void Informer::updateCout()
{
    printf("%4s: %c%-5.0f %s\n",
           "CO2", ' ', mCO2, "ppm");
    printf("%4s: %c%-5.1f %s\n",
           "Temp", mTemperature < 0 ? '-' : '+', abs(mTemperature), "C");
    printf("%4s:  %-5.1f %s\n",
           "RH", mRH, "%");
    printf("%4s: %c%-4hd%c%c%-5.0f\n",
           "Pres", mCurrentPressure < 0 ? '-' : '+', abs(mCurrentPressure),
           !mManual ? '>' : ' ', mTargetPressure < 0 ? '-' : '+', mTargetPressure);
    printf("%4s:  %-4.0f%c %-5.0f",
           "Fan", mCurrentFanSpeed,
           mManual ? '>' : ' ', mTargetFanSpeed);
    printf("----------------------\n");
}

void Informer::update()
{
    fetchValues();
    updateOLED();
    updateCout();
}