#ifndef GMP252_H
#define GMP252_H

// #include <memory>
#include "ModbusClient.h"
#include "ModbusRegister.h"

#define MODBUS_ADDRESS 240

// Holds wire addresses for modbus registers.
enum modbusRegisterAddress
{
    CO2_REGISTER = 0x0000,
    TEMPERATURE_REGISTER = 0x0004,
    DEVICE_STATUS_REGISTER = 0x0800,
    CO2_STATUS_REGISTER
};

class GMP252
{
private:
    // float mCO2;
    // float mTemperature;
    ModbusRegister mCO2Register;
    ModbusRegister mTemperatureRegister;
    ModbusRegister mDeviceStatusRegister;
    ModbusRegister mCO2StatusRegister;
    // std::shared_ptr<ModbusClient> mModbus;

public:
    GMP252(std::shared_ptr<ModbusClient> modbus);
    GMP252(const GMP252 &) = delete;
    float getCO2();
    float getTemperature();
    uint16_t getDeviceStatus();
    uint16_t getCO2Status();
    // void update();
};

#endif /* GMP252_H */
