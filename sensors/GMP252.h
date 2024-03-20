#ifndef GMP252_H
#define GMP252_H

#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Sensor32bitRegister.h"

#include <memory>

// Handles the GMP252 CO2 sensor.
class GMP252
{
  private:
    const int mModbusAddress = 240;
    Sensor32bitRegister mCO2;
    Sensor32bitRegister mTemperature;
    ModbusRegister mCO2RegisterLow;
    ModbusRegister mCO2RegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;

    enum modbusRegisterAddress
    {
        CO2_REGISTER_LOW = 0x0000,
        CO2_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0004,
        TEMPERATURE_REGISTER_HIGH
    };

  public:
    GMP252(std::shared_ptr<ModbusClient> modbus);
    GMP252(const GMP252 &) = delete;
    float getCO2();
    float getTemperature();
    void update();
};

#endif /* GMP252_H */