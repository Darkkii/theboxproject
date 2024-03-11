#include "SettingsMessage.h"
#include <string>

using namespace nlohmann;

SettingsMessage::SettingsMessage(const std::string message) :
    mJSONObject{ json::parse(message) }
{}

std::string SettingsMessage::getString()
{
    return mJSONObject.dump();
}

bool SettingsMessage::getAuto() { return mJSONObject.value("auto", false); }

int SettingsMessage::getSetPoint() { return mJSONObject.value("auto", false) ? mJSONObject.value("pressure", 0) : mJSONObject.value("pressure", 0); }