#ifndef STATUSMESSAGE_H
#define STATUSMESSAGE_H

#include "nlohmann/json.hpp" // IWYU pragma: keep

// Handles the creation of a JSON object from status data.
class StatusMessage
{
  private:
    int mMessageNumber;
    int mSpeed;
    int mSetpoint;
    int mPressure;
    bool mAuto;
    bool mError;
    int mCO2;
    int mRelativeHumidity;
    int mTemperature;
    nlohmann::ordered_json mJSONObject;

  public:
    StatusMessage(int speed,
                  int setpoint,
                  int pressure,
                  bool mode,
                  bool error,
                  int co2,
                  int relativeHumidity,
                  int temperature);
    operator const std::string();
    void setMessageNumber(const int messageNumber);
};

#endif /* STATUSMESSAGE_H */
