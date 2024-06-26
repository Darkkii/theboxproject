#include "State.h"

#include <iostream>

using namespace std;

State::State(const shared_ptr<I2CHandler> &i2cHandler,
             const shared_ptr<GMP252> &gmp252,
             const shared_ptr<HMP60> &hmp60,
             const shared_ptr<MIO12V> &mio12V,
             const shared_ptr<SDP600> &sdp600,
             const shared_ptr<MQTTHandler> &mqttHandler,
             const shared_ptr<Eeprom> &eeprom) :
    mDisplay(i2cHandler->getI2CBus(1)),
    mGMP252{ gmp252 },
    mHMP60{ hmp60 },
    mFanController(mio12V),
    mSDP600{ sdp600 },
    mMQTTHandler(mqttHandler),
    mEEPROM(eeprom)
{
    istringstream sstr;
    sstr.str(mEEPROM->read(EEPROM_REG_MODE));
    sstr >> mMode_auto;
    sstr.str(mEEPROM->read(EEPROM_REG_TAR_FAN));
    sstr >> mTargetFanSpeed;
    if (mTargetFanSpeed < 0 || mTargetFanSpeed > 1000)
        mTargetFanSpeed = 0;
    sstr.str(mEEPROM->read(EEPROM_REG_TAR_PRES));
    sstr >> mTargetPressure;
    if (mTargetPressure < 0 || mTargetPressure > 125)
        mTargetPressure = 0;
    mInputFanSpeed = mTargetFanSpeed;
    mInputPressure = mTargetPressure;
}

void State::writeStatusLines()
{
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

void State::OLED_VentStatus()
{
    bool gettingPressureInput = mTargetPressure != mInputPressure;
    bool gettingFanSpeedInput = mTargetFanSpeed != mInputFanSpeed;

    mDisplay.fill(0);

    mDisplay.text(mCO2_line.str(), 0, 0);
    mDisplay.text(mTemp_line.str(), 0, 9);
    mDisplay.text(mRH_line.str(), 0, 18);
    mDisplay.text(mHeader_line, 0, 36);
    if (gettingPressureInput)
    {
        mDisplay.rect(0, 44, 128, 9, 1, true);
    }
    else if (gettingFanSpeedInput)
    {
        mDisplay.rect(0, 53, 128, 9, 1, true);
    }
    else
    {
        mDisplay.text(">", 73, mMode_auto ? 45 : 54);
    }

    mDisplay.text(mPres_line.str(), 0, 45, !gettingPressureInput);
    mDisplay.text(mFan_line.str(), 0, 54, !gettingFanSpeedInput);

    mDisplay.show();
}

void State::OLED_MQTTCredentials()
{
    mDisplay.fill(0);

    size_t x;
    string cutString;
    bool tooLong;

    switch (mMQTT_input_stage)
    {
        case brokerIP:
            x = mBrokerIP.length();
            tooLong = x >= OLED_MAX_STR_WIDTH;
            if (tooLong)
            {
                cutString = mBrokerIP;
                cutString.erase(0, x - (OLED_MAX_STR_WIDTH - 1));
            }
            mDisplay.text("Broker IP:", 0, 44);
            mDisplay.text(tooLong ? cutString : mBrokerIP, 0, 53);
        case networkPW:
            x = mNetworkPW.length();
            tooLong = x >= OLED_MAX_STR_WIDTH;
            if (tooLong)
            {
                cutString = mNetworkPW;
                cutString.erase(0, x - (OLED_MAX_STR_WIDTH - 1));
            }
            mDisplay.text("Network PW:", 0, 22);
            mDisplay.text(tooLong ? cutString : mNetworkPW, 0, 31);
        case networkID:
            x = mNetworkID.length();
            tooLong = x >= OLED_MAX_STR_WIDTH;
            if (tooLong)
            {
                cutString = mNetworkID;
                cutString.erase(0, x - (OLED_MAX_STR_WIDTH - 1));
            }
            mDisplay.text("Network ID:", 0, 0);
            mDisplay.text(tooLong ? cutString : mNetworkID, 0, 9);
    }

    switch (mMQTT_input_stage)
    {
        case networkID:
            x = mNetworkID.length();
            tooLong = x > OLED_MAX_STR_WIDTH - 1;
            mDisplay.rect(tooLong ? 7 * (OLED_MAX_STR_WIDTH + 1) + 1 : x * 8,
                          8,
                          8,
                          10,
                          1,
                          true);
            mDisplay.text(&mInputChar,
                          tooLong ? 7 * (OLED_MAX_STR_WIDTH + 1) + 1 : x * 8,
                          9,
                          0);
            break;
        case networkPW:
            x = mNetworkPW.length();
            tooLong = x > OLED_MAX_STR_WIDTH - 2;
            mDisplay.rect(tooLong ? 7 * (OLED_MAX_STR_WIDTH + 1) + 1 : x * 8,
                          30,
                          8,
                          10,
                          1,
                          true);
            mDisplay.text(&mInputChar,
                          tooLong ? 7 * (OLED_MAX_STR_WIDTH + 1) + 1 : x * 8,
                          31,
                          0);
            break;
        case brokerIP:
            x = mBrokerIP.length();
            tooLong = x > OLED_MAX_STR_WIDTH - 2;
            mDisplay.rect(tooLong ? 7 * (OLED_MAX_STR_WIDTH + 1) + 1 : x * 8,
                          52,
                          8,
                          10,
                          1,
                          true);
            mDisplay.text(&mInputChar,
                          tooLong ? 7 * (OLED_MAX_STR_WIDTH + 1) + 1 : x * 8,
                          53,
                          0);
            break;
    }

    mDisplay.show();
}

bool State::ConnectMQTT(string networkID, string networkPW, string brokerIP)
{
    mDisplay.fill(0);
    mDisplay.text("Connecting...", 0, 0);
    mDisplay.text("SSID:", 0, 11);
    mDisplay.text(networkID, 0, 22);
    mDisplay.show();
    bool success = mMQTTHandler->connect(
        std::move(networkID), std::move(networkPW), std::move(brokerIP));
    if (success)
    {
        mDisplay.text("Success!", 0, 33);
        mEEPROM->write(EEPROM_REG_NETWORK_ID, networkID);
        mEEPROM->write(EEPROM_REG_NETWORK_PW, networkPW);
        mEEPROM->write(EEPROM_REG_BROKER_IP, brokerIP);
    }
    else
    {
        mDisplay.text("Failed!", 0, 33);
    }
    mDisplay.show();
    sleep_ms(5000);
    return success;
}

void State::updateOLED()
{
    if (mStatusScreen)
    {
        OLED_MQTTCredentials();
    }
    else
    {
        OLED_VentStatus();
    }
}

void State::updateCout()
{
    cout << mCO2_line.str() << endl;
    cout << mTemp_line.str() << endl;
    cout << mRH_line.str() << endl;
    cout << mHeader_line << endl;
    cout << mPres_line.str() << endl;
    cout << mFan_line.str() << endl;
    printf("----------------\n");
}

void State::fetchValues()
{
    mCO2 = mGMP252->getCO2();
    mTemperature = (mGMP252->getTemperature() + mHMP60->getTemperature()) / 2;
    mRH = mHMP60->getRelativeHumidity();
    mCurrentPressure = static_cast<int16_t>(mSDP600->getPressure() / 240);
    mCurrentFanSpeed = mFanController->getFanSpeed();
}

void State::update()
{
    fetchValues();
    writeStatusLines();
    updateOLED();
    // updateCout(); // slows system down a lot
}

void State::update(SettingsMessage sm)
{
    mMode_auto = sm.getAuto();
    mMode_auto ? mTargetPressure = static_cast<int16_t>(sm.getSetpoint())
               : mTargetFanSpeed = static_cast<uint16_t>(sm.getSetpoint()) * 10;
    mInputPressure = mTargetPressure;
    mInputFanSpeed = mTargetFanSpeed;
    mEEPROM->write(EEPROM_REG_MODE, mMode_auto ? "1" : "0");
    mEEPROM->write(EEPROM_REG_TAR_PRES, to_string(mTargetPressure));
    mEEPROM->write(EEPROM_REG_TAR_FAN, to_string(mTargetFanSpeed));
    update();
    updateMQTT();
}

void State::toggleScreen()
{
    mStatusScreen = !mStatusScreen;
    if (mStatusScreen)
    {
        mNetworkID = "";
        mNetworkPW = "";
        mBrokerIP = "";
        mInputChar = '0';
        mMQTT_input_stage = networkID;
    }
    update();
}

void State::toggleMode()
{
    if (mStatusScreen)
    {
        switch (mMQTT_input_stage)
        {
            case networkID:
                mMQTT_input_stage = networkPW;
                break;
            case networkPW:
                mMQTT_input_stage = brokerIP;
                break;
            case brokerIP:
                mStatusScreen = false;
                // Hard code the IP, as using the user input value hard locks
                // the system. (No idea why).
                ConnectMQTT(mNetworkID, mNetworkPW, "192.168.137.1");
                break;
        }
    }
    else
    {
        mMode_auto = !mMode_auto;
        mInputPressure = mTargetPressure;
        mInputFanSpeed = mCurrentFanSpeed;
        mTargetFanSpeed = mCurrentFanSpeed;
        mEEPROM->write(EEPROM_REG_MODE, mMode_auto ? "1" : "0");
        mEEPROM->write(EEPROM_REG_TAR_FAN, to_string(mTargetFanSpeed));
    }
    update();
}

void State::setTarget()
{
    if (mStatusScreen)
    {
        switch (mMQTT_input_stage)
        {
            case networkID:
                mNetworkID += mInputChar;
                break;
            case networkPW:
                mNetworkPW += mInputChar;
                break;
            case brokerIP:
                mBrokerIP += mInputChar;
                break;
        }
    }
    else
    {
        if (mMode_auto)
        {
            mTargetPressure = mInputPressure;
            mEEPROM->write(EEPROM_REG_TAR_PRES, to_string(mTargetPressure));
        }
        else
        {
            mTargetFanSpeed = mInputFanSpeed;
            mEEPROM->write(EEPROM_REG_TAR_FAN, to_string(mTargetFanSpeed));
        }
    }
    update();
}

void State::backspace()
{
    if (mStatusScreen)
    {
        switch (mMQTT_input_stage)
        {
            case networkID:
                if (mNetworkID.length() > 0)
                {
                    mNetworkID.pop_back();
                }
                else
                {
                    mStatusScreen = false;
                }
                break;
            case networkPW:
                if (mNetworkPW.length() > 0)
                {
                    mNetworkPW.pop_back();
                }
                else
                {
                    mMQTT_input_stage = networkID;
                }
                break;
            case brokerIP:
                if (mNetworkID.length() > 0)
                {
                    mBrokerIP.pop_back();
                }
                else
                {
                    mMQTT_input_stage = networkPW;
                }
                break;
        }
    }
    else
    {
        if (mMode_auto)
        {
            mInputPressure = mTargetPressure;
        }
        else
        {
            mInputFanSpeed = mTargetFanSpeed;
        }
    }
    update();
}

void State::clockwise()
{
    if (mStatusScreen)
    {
        switch (mInputChar)
        {
            case '9':
                mInputChar = 'A';
                break;
            case 'Z':
                mInputChar = 'a';
                break;
            case 'z':
                mInputChar = '!';
                break;
            case '/':
                mInputChar = ':';
                break;
            case '@':
                mInputChar = '[';
                break;
            case '`':
                mInputChar = '{';
                break;
            case '~':
                break;
            default:
                ++mInputChar;
                break;
        }
    }
    else
    {
        if (mMode_auto)
        {
            adjustInputPressure(+1);
        }
        else
        {
            adjustInputFanSpeed(+10);
        }
    }
    update();
}

void State::counter_clockwise()
{
    if (mStatusScreen)
    {
        switch (mInputChar)
        {
            case '0':
                break;
            case 'A':
                mInputChar = '9';
                break;
            case 'a':
                mInputChar = 'Z';
                break;
            case '!':
                mInputChar = 'z';
                break;
            case ':':
                mInputChar = '/';
                break;
            case '[':
                mInputChar = '@';
                break;
            case '{':
                mInputChar = '`';
                break;
            default:
                --mInputChar;
                break;
        }
    }
    else
    {
        if (mMode_auto)
        {
            adjustInputPressure(-1);
        }
        else
        {
            adjustInputFanSpeed(-10);
        }
    }
    update();
}

void State::adjustInputFanSpeed(int x)
{
    int temp = mInputFanSpeed + x;
    if (0 < temp)
    {
        if (temp < 1000)
        {
            mInputFanSpeed = temp;
        }
        else
        {
            mInputFanSpeed = 1000;
        }
    }
    else
    {
        mInputFanSpeed = 0;
    }
}

void State::adjustInputPressure(int x)
{
    int temp = mInputPressure + x;
    if (0 < temp)
    {
        if (temp < 125)
        {
            mInputPressure = static_cast<int16_t>(temp);
        }
        else
        {
            mInputPressure = 125;
        }
    }
    else
    {
        mInputPressure = 0;
    }
}

void State::adjustFan()
{
    if (mMode_auto)
    {
        if (mTargetPressure > MIN_PRESSURE_TARGET)
        {
            if (time_reached(mFanAdjustmentTimeout_ms))
            {
                mSDP600->update();
                auto currentPressure =
                    static_cast<int16_t>(mSDP600->getPressure() / 240);
                auto targetDelta =
                    static_cast<int16_t>(mTargetPressure - currentPressure);
                if (targetDelta != 0)
                {
                    int newFanSpeed = mCurrentFanSpeed + targetDelta * 4;
                    mFanController->setFanSpeed(newFanSpeed);
                    mFanAdjustmentTimeout_ms =
                        make_timeout_time_ms(FAN_ADJUSTMENT_LATENCY_MS);
                }
            }
        }
        else if (mFanController->getFanSpeed() != 0)
        {
            mFanController->setFanSpeed(0);
            mFanAdjustmentTimeout_ms =
                make_timeout_time_ms(FAN_ADJUSTMENT_LATENCY_MS);
        }
    }
    else
    {
        if (mTargetFanSpeed != mFanController->getFanSpeed())
            mFanController->setFanSpeed(mTargetFanSpeed);
    }
}

void State::updateMQTT()
{
    mMQTTHandler->send(
        StatusMessage(mCurrentFanSpeed / 10,
                      mMode_auto ? mTargetPressure : mTargetFanSpeed / 10,
                      mCurrentPressure,
                      mMode_auto,
                      false,
                      static_cast<int>(mCO2),
                      static_cast<int>(mRH),
                      static_cast<int>(mTemperature)));
}
