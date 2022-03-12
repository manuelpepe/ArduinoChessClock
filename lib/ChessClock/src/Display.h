#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include "TM1637.h"

class Display
{
public:
    Display(int dio_pin, int clk_pin);

    void displayTime(unsigned long time, boolean force_low_precission = false);
    void blink(unsigned long current_time = 0);
    void clear();
    void setBrightness(int level = BRIGHT_TYPICAL);

private:
    TM1637 disp;
    boolean lit;
    unsigned long blink_timer;

    unsigned long current_value;
    boolean current_precision;
};

#endif
