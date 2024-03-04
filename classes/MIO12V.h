#ifndef MIO12V_H
#define MIO12V_H

#include "ModbusClient.h"
#include "ModbusRegister.h"

class MIO12V
{
private:
    const int mModbusAddress = 1;
    const int mFanSpeedRegisterAddress = 0x0000;
    ModbusRegister mFanSpeedRegister;
public:
    MIO12V(std::shared_ptr<ModbusClient> modbus);
    MIO12V(const MIO12V &) = delete;
    void setFanSpeed(uint16_t speed);
};

#endif /* MIO12V_H */
