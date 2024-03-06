#include "I2CBus.h"
#include "hardware/gpio.h"

I2CBus::I2CBus(uint i2cNumber) :
    mI2C{ i2cNumber == 0 ? i2c0 : i2c1 },
    mSDA{ i2cNumber == 0 ? I2C_SDA_0 : I2C_SDA_1 },
    mSCL{ i2cNumber == 0 ? I2C_SCL_0 : I2C_SCL_1 },
    mBaudRate{ i2cNumber == 0 ? I2C_BAUDRATE_0 : I2C_BAUDRATE_1 }
{
    i2c_init(mI2C, mBaudRate);
    gpio_set_function(mSDA, GPIO_FUNC_I2C);
    gpio_set_function(mSCL, GPIO_FUNC_I2C);
}

I2CBus::operator i2c_inst_t *() { return mI2C; }
