#ifndef Potentiometer_h
#define Potentiometer_h

#include "Arduino.h"

class Potentiometer
{
public:
    Potentiometer(uint8_t pin);
    uint8_t getPin();
    int getReading(int max);

private:
    uint8_t pin;
};

#endif
