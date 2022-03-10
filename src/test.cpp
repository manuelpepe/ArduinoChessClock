#include <Arduino.h>
#include "Potentiometer.h"
#include "TM1637.h"

#define d1_btn 4
#define d2_btn 10
#define fun_btn 11
#define buzzer 3
#define min_pot A0
#define sec_pot A2

void displayTime(long countdown, TM1637 d, boolean force_low_precission = false)
{
    /* Displays formatted `current_time` (miliseconds) into `d` as `minutes:seconds`. If `force_low_precission` is false,
    `seconds:miliseconds` format will be used when displaying less than 10000 ms (10 seconds).  */
    int parts[2];
    if (force_low_precission || countdown >= 10000)
    {
        countdown /= 1000;
        int minutes = countdown / 60;
        int seconds = countdown % 60;
        parts[0] = minutes;
        parts[1] = seconds;
    }
    else
    {
        int seconds = (countdown / 1000) % 60;
        int millis = countdown % (seconds * 1000) / 10;
        parts[0] = seconds;
        parts[1] = millis;
    }

    d.point(1);
    d.display(0, parts[0] / 10 % 10);
    d.display(1, parts[0] % 10);
    d.display(2, parts[1] / 10 % 10);
    d.display(3, parts[1] % 10);
}

Potentiometer *pot;
TM1637 *disp;
long timer;

void setup()
{
    Serial.begin(9600);
    // pot = new Potentiometer(A0);
    disp = new TM1637(7, 6);
    disp->init();
    disp->point(1);
    disp->display(0, 1);
    disp->display(1, 2);
    disp->display(2, 3);
    disp->display(3, 4);

    timer = 180000;
    disp->set(BRIGHT_TYPICAL);
    displayTime(timer, *disp);

    Serial.println("setup complete");
}

void loop()
{
}