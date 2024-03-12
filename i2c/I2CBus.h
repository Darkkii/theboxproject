#ifndef I2CBUS_H
#define I2CBUS_H

#include "hardware/i2c.h"

class I2CBus
{
private:
    i2c_inst_t *mI2C;
    uint mSDA;
    uint mSCL;
    uint mBaudRate;

    enum mI2CPin
    {
        I2C_SDA_0 = 16,
        I2C_SCL_0,
        I2C_SDA_1 = 14,
        I2C_SCL_1
    };

    enum mI2CSpeed
    {
        I2C_BAUDRATE_0 = 9600,
        I2C_BAUDRATE_1 = 400000
    };

public:
    I2CBus(uint i2cNumber);
    I2CBus(I2CBus &) = delete;
    operator i2c_inst_t *();
};


#endif /* I2CBUS_H */
