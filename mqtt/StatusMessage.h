#ifndef STATUSMESSAGE_H
#define STATUSMESSAGE_H

#include "nlohmann/json.hpp"


class StatusMessage
{
private:
    int mMessageNumber;
    int mSpeed;
    int mSetPoint;
    int mPressure;
    bool mAuto;
    bool mError;
    int mCO2;
    int mRelativeHumidity;
    int mTemperature;
    nlohmann::json mJSONObject;
public:
    StatusMessage(int messageNumber,
                  int speed,
                  int setpoint,
                  int pressure,
                  bool mode,
                  bool error,
                  int co2,
                  int relativeHumidity,
                  int temperature);
    operator const std::string();
};

#endif /* STATUSMESSAGE_H */
