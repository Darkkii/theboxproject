#include "JSONPacket.h"
#include <string>

using namespace nlohmann;

JSONPacket::JSONPacket(std::string message) :
    mJSONObject(json::parse(message))
{
    mJSONObject.at("auto").get_to(mAuto);
    mJSONObject.at("setpoint").get_to(mSetPoint);
}

bool JSONPacket::getAuto() { return mAuto; }

int JSONPacket::getSetPoint() { return mSetPoint; }
