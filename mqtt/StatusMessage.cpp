#include "StatusMessage.h"
#include <string>

using namespace nlohmann;

StatusMessage::StatusMessage(int speed = 0,
                             int setpoint = 0,
                             int pressure = 0,
                             bool mode = false,
                             bool error = false,
                             int co2 = 0,
                             int relativeHumidity = 0,
                             int temperature = 0) :
    mMessageNumber{ 0 },
    mSpeed{ speed },
    mSetpoint{ setpoint },
    mPressure{ pressure },
    mAuto{ mode },
    mError{ error },
    mCO2{ co2 },
    mRelativeHumidity{ relativeHumidity },
    mTemperature{ temperature }
{}

StatusMessage::operator const std::string()
{
    mJSONObject = ordered_json{ {"nr", mMessageNumber},
                            {"speed", mSpeed},
                            {"setpoint", mSetpoint},
                            {"pressure", mPressure},
                            {"auto", mAuto},
                            {"error", mError},
                            {"co2", mCO2},
                            {"rh", mRelativeHumidity},
                            {"temp", mTemperature} };
    return mJSONObject.dump();
}

void StatusMessage::setMessageNumber(const int messageNumber)
{
    mMessageNumber = messageNumber;
    // mJSONObject.at("nr") = mMessageNumber;
}
