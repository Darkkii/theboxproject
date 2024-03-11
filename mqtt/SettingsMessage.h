#ifndef SETTINGSMESSAGE_H
#define SETTINGSMESSAGE_H

#include "nlohmann/json.hpp"

class SettingsMessage
{
private:
    nlohmann::json mJSONObject;
public:
    explicit SettingsMessage(const std::string message);
    std::string getString();
    bool getAuto();
    int getSetPoint();
};

#endif /* SETTINGSMESSAGE_H */
