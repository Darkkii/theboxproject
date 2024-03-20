#ifndef I2CHANDLER_H
#define I2CHANDLER_H

#include "I2CBus.h"
#include "hardware/i2c.h"

class I2CHandler
{
  private:
    I2CBus mI2CBus0;
    I2CBus mI2CBus1;

  public:
    I2CHandler();
    I2CHandler(I2CHandler &) = delete;
    i2c_inst_t *getI2CBus(uint i2cNumber);
};

#endif /* I2CHANDLER_H */
