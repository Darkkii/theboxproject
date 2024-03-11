#include "StatusMessage.h"
#include <string>

using namespace nlohmann;

StatusMessage::StatusMessage(int messageNumber = 0,
                             int speed = 0,
                             int setpoint = 0,
                             int pressure = 0,
                             bool mode = false,
                             bool error = false,
                             int co2 = 0,
                             int relativeHumidity = 0,
                             int temperature = 0) :
    mMessageNumber{ messageNumber },
    mSpeed{ speed },
    mSetPoint{ setpoint },
    mPressure{ pressure },
    mAuto{ mode },
    mError{ error },
    mCO2{ co2 },
    mRelativeHumidity{ relativeHumidity },
    mTemperature{ temperature }
{
    mJSONObject = json{ {"nr", mMessageNumber},
                        {"speed", mSpeed},
                        {"setpoint", mSetPoint},
                        {"pressure", mPressure},
                        {"auto", mAuto},
                        {"error", mError},
                        {"co2", mCO2},
                        {"rh", mRelativeHumidity},
                        {"temp", mTemperature} };
}

StatusMessage::operator const std::string()
{
    return mJSONObject.dump();
}
