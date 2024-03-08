#include "Informer.h"

using namespace std;

Informer::Informer(const shared_ptr<I2CHandler>& i2cHandler,
                   const shared_ptr<GMP252>& gmp252,
                   const shared_ptr<HMP60>& hmp60,
                   const shared_ptr<MIO12V>& mio12V,
                   const shared_ptr<SDP600>& sdp600) :
                   mDisplay(i2cHandler->getI2CBus(1)),
                   mGMP252{gmp252},
                   mHMP60{hmp60},
                   mFanController(mio12V),
                   mSDP600{sdp600}
{}

void Informer::fetchValues()
{
    mManual = mState->getModeAuto();
    mCO2 = mGMP252->getCO2();
    mTemperature = (mGMP252->getTemperature() + mHMP60->getTemperature()) / 2;
    mRH = mHMP60->getRelativeHumidity();
    mCurrentPressure = mSDP600->getPressure() / 240;
    mTargetPressure = mState->getTargetPressure();
    mCurrentFanSpeed = mFanController->getFanSpeed() / 10;
    mTargetFanSpeed;
}

void Informer::writeLines()
{
    snprintf(mCO2_line, 17, " CO2: %5.0f ppm",
             mCO2);
    snprintf(mTemp_line, 17, "Temp: %c%5.1f C",
             mTemperature < 0 ? '-' : '+',
             abs(mTemperature));
    snprintf(mRH_line, 17, "  RH:  %5.1f %%",
             mRH);
    snprintf(mPres_line, 17, "Pres: %c%5hd  %3hd",
             mCurrentPressure < 0 ? '-' : '+',
             abs(mCurrentPressure),
             mTargetPressure);
    snprintf(mFan_line, 17, " Fan:  %5 %3",
             mCurrentFanSpeed,
             mTargetFanSpeed);
}

void Informer::updateOLED()
{
    mDisplay.text(mCO2_line,0,0);
    mDisplay.text(mTemp_line,0,9);
    mDisplay.text(mRH_line,0,18);
    mDisplay.text(mHeader_line,0,36);
    mDisplay.text(mPres_line,0,45);
    mDisplay.text(mFan_line,0,54);
    mDisplay.show();
}

void Informer::updateCout()
{
    cout << mCO2_line << endl;
    cout << mTemp_line << endl;
    cout << mRH_line << endl;
    cout << mHeader_line << endl;
    cout << mPres_line << endl;
    cout << mFan_line << endl;
    printf("----------------\n");
}

void Informer::update()
{
    fetchValues();
    writeLines();
    updateOLED();
    updateCout();
}