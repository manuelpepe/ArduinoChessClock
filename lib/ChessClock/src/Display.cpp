#include "Arduino.h"
#include "Display.h"
#include "TM1637.h"

Display::Display(int dio_pin, int clk_pin) : disp(dio_pin, clk_pin)
{
    disp.init();
    blink_timer = 0;
    lit = true;
}

void Display::displayTime(unsigned long time, boolean force_low_precission)
{
    /* Displays formatted `current_time` (miliseconds) into `d` as `minutes:seconds`. If `force_low_precission` is false,
    `seconds:miliseconds` format will be used when displaying less than 10000 ms (10 seconds).  */
    current_value = time;
    int parts[2];
    if (force_low_precission || time >= 10000)
    {
        time /= 1000;
        int minutes = time / 60;
        int seconds = time % 60;
        parts[0] = minutes;
        parts[1] = seconds;
    }
    else
    {
        int seconds = (time / 1000) % 60;
        int millis = time % (seconds * 1000) / 10;
        parts[0] = seconds;
        parts[1] = millis;
    }

    disp.point(1);
    disp.display(0, parts[0] / 10 % 10);
    disp.display(1, parts[0] % 10);
    disp.display(2, parts[1] / 10 % 10);
    disp.display(3, parts[1] % 10);
    current_precision = force_low_precission;
}

void Display::blink(unsigned long current_time)
{
    unsigned long time = (current_time == 0) ? millis() : current_time;
    if (time - blink_timer >= 1000)
    {
        if (lit)
        {
            clear();
            lit = false;
        }
        else
        {
            displayTime(current_value, current_precision);
            lit = true;
        }
        blink_timer = time;
    }
}

void Display::clear()
{
    disp.clearDisplay();
}

void Display::setBrightness(int level)
{
    disp.set(level);
}
