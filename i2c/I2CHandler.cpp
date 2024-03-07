#include "I2CHandler.h"

I2CHandler::I2CHandler() :
    mI2CBus0{ 0 },
    mI2CBus1{ 1 }
{}

i2c_inst_t *I2CHandler::getI2CBus(uint i2cNumber)
{
    return i2cNumber ? mI2CBus1 : mI2CBus0;
}
