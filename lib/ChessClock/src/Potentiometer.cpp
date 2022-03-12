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
    // `analogRead` goes up to 1023 but potentiometers don't always
    // maintain this value, it might go back and forth in a range
    // of, say, 3 to 5 numbers.
    // Also, `map` only returns `out_max` when `x == in_max`.
    // By setting out_max to 1024 (outside the range of analogRead)
    // we can increase out_max by one, inherently increasing the
    // range at which `map` will return `max`.
    return round(map(analogRead(pin), 0, 1024, 0, max + 1));
}