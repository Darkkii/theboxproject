#ifndef JSONPACKET_H
#define JSONPACKET_H

#include "nlohmann/json.hpp"

class JSONPacket
{
private:
    bool mAuto;
    int mSetPoint;
    nlohmann::json mJSONObject;
public:
    JSONPacket(std::string message);
    bool getAuto();
    int getSetPoint();
};

#endif /* JSONPACKET_H */
