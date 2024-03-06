#ifndef MIO12V_H
#define MIO12V_H

#include "ModbusClient.h"
#include "ModbusRegister.h"

class MIO12V
{
private:
    const int mModbusAddress = 1;
    const int mFanRotationRegisterAddress = 0x0004;
    const int mFanSpeedRegisterAddress = 0x0000;
    uint16_t mFanSpeed;
    ModbusRegister mFanRotationRegister;
    ModbusRegister mFanSpeedRegister;

public:
    MIO12V(std::shared_ptr<ModbusClient> modbus);
    MIO12V(const MIO12V &) = delete;
    uint16_t getFanRotation();
    uint16_t getFanSpeed();
    void setFanSpeed(int speed);
};

#endif /* MIO12V_H */
