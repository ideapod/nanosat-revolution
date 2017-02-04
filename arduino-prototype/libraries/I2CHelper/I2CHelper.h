#ifndef I2CHelper_h
#define I2CHelper_h

#include "Arduino.h"

class I2CHelper {
 public:
  static void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data);
  static void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data);
  I2CHelper();
    ~I2CHelper();
  private:
    int _pin;

};

#endif
