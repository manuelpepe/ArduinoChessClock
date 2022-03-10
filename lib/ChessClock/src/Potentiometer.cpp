#include "Arduino.h"
#include "Potentiometer.h"

Potentiometer::Potentiometer(uint8_t _pin)
{
    pin = _pin;
}

uint8_t Potentiometer::getPin()
{
    return pin;
}

int Potentiometer::getReading(int max)
{
    return round(map(analogRead(pin), 0, 1023, 0, max));
}