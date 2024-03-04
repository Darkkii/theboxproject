#ifndef GMP252_H
#define GMP252_H

#include "ModbusClient.h"
#include "ModbusRegister.h"

#define MODBUS_ADDRESS 240

// Holds wire addresses for modbus registers.
enum modbusRegisterAddress
{
    CO2_REGISTER_LOW = 0x0000,
    CO2_REGISTER_HIGH,
    TEMPERATURE_REGISTER_LOW = 0x0004,
    TEMPERATURE_REGISTER_HIGH,
    DEVICE_STATUS_REGISTER = 0x0800,
    CO2_STATUS_REGISTER
};

class GMP252
{
private:
    float mCO2;
    float mTemperature;
    ModbusRegister mCO2RegisterLow;
    ModbusRegister mCO2RegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;
    ModbusRegister mDeviceStatusRegister;
    ModbusRegister mCO2StatusRegister;

public:
    GMP252(std::shared_ptr<ModbusClient> modbus);
    GMP252(const GMP252 &) = delete;
    float getCO2();
    float getTemperature();
    uint16_t getDeviceStatus();
    uint16_t getCO2Status();
    void update();
};

#endif /* GMP252_H */
