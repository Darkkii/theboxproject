#ifndef I2CHANDLER_H
#define I2CHANDLER_H

#include "hardware/i2c.h"

class I2CHandler
{
private:
    i2c_inst_t *mI2C;
    uint8_t mSCL;
    uint8_t mSDA;

public:
    I2CHandler(i2c_inst_t *i2c, uint baud_rate);
    I2CHandler(I2CHandler &) = delete;
};




#endif /* I2CHANDLER_H */
