#ifndef ClockConfig_h
#define ClockConfig_h

#include "Arduino.h"

struct ClockConfig
{
    // Displays
    int disp_p1_clk;
    int disp_p1_dio;

    int disp_p2_clk;
    int disp_p2_dio;

    // Buttons
    int btn_p1_pin;
    int btn_p2_pin;
    int btn_ctrl_pin;

    // Potentiometers
    uint8_t pot_min_pin;
    uint8_t pot_sec_pin;

    // Buzzer
    int buzzer_pin;
};

#endif
