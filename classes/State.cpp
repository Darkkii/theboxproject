#include "State.h"

using namespace std;

State::State(const shared_ptr<I2CHandler> &i2cHandler,
             const shared_ptr<GMP252> &gmp252,
             const shared_ptr<HMP60> &hmp60,
             const shared_ptr<MIO12V> &mio12V,
             const shared_ptr<SDP600> &sdp600) :
        mDisplay(i2cHandler->getI2CBus(1)),
        mGMP252{gmp252},
        mHMP60{hmp60},
        mFanController(mio12V),
        mSDP600{sdp600} {
    mMode_auto = true;
    mTargetFanSpeed = 0; // get from EEPROM
    mTargetPressure = 0; // get from EEPROM
    mFanController->setFanSpeed(mTargetFanSpeed);
    mPrevPressureAdjustment = 0;

    mInputFanSpeed = mTargetFanSpeed;
    mInputPressure = mTargetPressure;
}

void State::fetchValues() {
    mCO2 = mGMP252->getCO2();
    mTemperature = (mGMP252->getTemperature() + mHMP60->getTemperature()) / 2;
    mRH = mHMP60->getRelativeHumidity();
    mCurrentPressure = mSDP600->getPressure() / 240;
    mCurrentFanSpeed = mFanController->getFanSpeed();
}

void State::writeLines() {
    mCO2_line.str("");
    mCO2_line << " CO2: ";
    mCO2_line << setw(5) << setprecision(0) << mCO2;
    mCO2_line << " ppm";

    mTemp_line.str("");
    mTemp_line << "Temp: ";
    mTemp_line << setw(5) << setprecision(1) << fixed << mTemperature;
    mTemp_line << " C";

    mRH_line.str("");
    mRH_line << "  RH: ";
    mRH_line << setw(5) << setprecision(1) << fixed << mRH;
    mRH_line << " %";

    mPres_line.str("");
    mPres_line << "Pres: ";
    mPres_line << setw(3) << mCurrentPressure << " ";
    mPres_line << setw(3) << mInputPressure << " Pa";

    mFan_line.str("");
    mFan_line << " Fan: ";
    mFan_line << setw(3) << mCurrentFanSpeed / 10 << " ";
    mFan_line << setw(3) << mInputFanSpeed / 10 << " %";
}

void State::updateOLED() {
    bool gettingPressureInput = mTargetPressure != mInputPressure;
    bool gettingFanSpeedInput = mTargetFanSpeed != mInputFanSpeed;

    mDisplay.fill(0);

    mDisplay.text(mCO2_line.str(), 0, 0);
    mDisplay.text(mTemp_line.str(), 0, 9);
    mDisplay.text(mRH_line.str(), 0, 18);
    mDisplay.text(mHeader_line, 0, 36);
    if (gettingPressureInput) {
        mDisplay.rect(0, 44, 128, 9, 1, true);
    } else if (gettingFanSpeedInput) {
        mDisplay.rect(0, 53, 128, 9, 1, true);
    } else if (mMode_auto) {
        mDisplay.text(">", 73, 45);
    } else {
        mDisplay.text(">", 73, 54);
    }
    mDisplay.text(mPres_line.str(), 0, 45, !gettingPressureInput);
    mDisplay.text(mFan_line.str(), 0, 54, !gettingFanSpeedInput);

    mDisplay.show();
}

void State::updateCout() {
    cout << mCO2_line.str() << endl;
    cout << mTemp_line.str() << endl;
    cout << mRH_line.str() << endl;
    cout << mHeader_line << endl;
    cout << mPres_line.str() << endl;
    cout << mFan_line.str() << endl;
    printf("----------------\n");
}

void State::update() {
    fetchValues();
    writeLines();
    updateOLED();
    updateCout();
}

void State::toggleMode() {
    mMode_auto = !mMode_auto;
    mInputPressure = mTargetPressure;
    mInputFanSpeed = mCurrentFanSpeed;
    mTargetFanSpeed = mCurrentFanSpeed;
    update();
}

void State::setTarget() {
    if (mMode_auto) {
        mTargetPressure = mInputPressure;
    } else {
        mTargetFanSpeed = mInputFanSpeed;
    }
}

void State::adjustInputFanSpeed(int x) {
    int temp = mInputFanSpeed + x;
    if (0 < temp) {
        if (temp < 1000) {
            mInputFanSpeed = temp;
        } else {
            mInputFanSpeed = 1000;
        }
    } else {
        mInputFanSpeed = 0;
    }
}

void State::adjustInputPressure(int x) {
    int temp = mInputPressure + x;
    if (0 < temp) {
        if (temp < 125) {
            mInputPressure = temp;
        } else {
            mInputPressure = 125;
        }
    } else {
        mInputPressure = 0;
    }
}

void State::clockwise() {
    if (mMode_auto) {
        adjustInputPressure(+1);
    } else {
        adjustInputFanSpeed(+10);
    }
    update();
}

void State::counter_clockwise() {
    if (mMode_auto) {
        adjustInputPressure(-1);
    } else {
        adjustInputFanSpeed(-10);
    }
    update();
}

void State::adjustFan() {
    if (mMode_auto) {
        uint32_t currTime = time_us_32();
        if (mTargetPressure > MIN_PRESSURE_TARGET) {
            if (currTime - mPrevPressureAdjustment > PRESSURE_CHECK_LATENCY_US) {
                int16_t currentPressure = mSDP600->getPressure() / 240;
                int16_t targetDelta = mTargetPressure - currentPressure;
                if (abs(targetDelta) > PRESSURE_TARGET_ACCURACY) {
                    int newFanSpeed = mCurrentFanSpeed + 10 * targetDelta / 2;
                    mFanController->setFanSpeed(newFanSpeed);
                    mPrevPressureAdjustment = currTime;
                }
            }
        } else {
            if (mFanController->getFanSpeed() != 0)
                mFanController->setFanSpeed(0);
        }
    } else {
        if (mTargetFanSpeed != mFanController->getFanSpeed())
            mFanController->setFanSpeed(mTargetFanSpeed);
    }
}
