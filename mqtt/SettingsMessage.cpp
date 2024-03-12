#include "SettingsMessage.h"
#include <string>
#include <iostream>

using namespace std;

SettingsMessage::SettingsMessage(const bool mode, const int setpoint) :
    mMode{ mode },
    mSetpoint{ setpoint }
{}

bool SettingsMessage::getAuto() { return mMode; }

int SettingsMessage::getSetpoint() { return mSetpoint; }