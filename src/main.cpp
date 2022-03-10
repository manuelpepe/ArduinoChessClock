#include <Arduino.h>
#include <ChessClock.h>
#include "Potentiometer.h"
#include "TM1637.h"

#define d1_btn 4
#define d2_btn 10
#define fun_btn 11
#define buzzer 3
#define min_pot A0
#define sec_pot A2

ClockConfig config;
ChessClock *clock;

void setup()
{
  config.btn_ctrl_pin = 11;
  config.btn_p1_pin = 4;
  config.btn_p2_pin = 10;

  config.buzzer_pin = 3;

  config.pot_min_pin = A0;
  config.pot_sec_pin = A2;

  config.disp_p1_clk = 7;
  config.disp_p1_dio = 6;
  config.disp_p2_clk = 9;
  config.disp_p2_dio = 8;

  clock = new ChessClock(config);
}

void loop()
{
  clock->loop();
}