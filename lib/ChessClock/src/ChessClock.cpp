#include <string.h>
#include "Arduino.h"
#include "ChessClock.h"
#include "TM1637.h"

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

void blinkDisplay(long &blink_timer, boolean &lit, int mode, TM1637 display_p1, TM1637 display_p2, int timer_p1, int timer_p2)
{
    unsigned long time = millis();
    if (time - blink_timer >= 1000)
    {
        if (lit)
        {
            display_p1.clearDisplay();
            display_p2.clearDisplay();
            lit = false;
        }
        else
        {
            displayTime(timer_p1, display_p1, mode == 5);
            displayTime(timer_p2, display_p2, mode == 5);
            lit = true;
        }
        blink_timer = time;
    }
}

void playSound(int buzzer, int tone_val, int duration)
{
    tone(buzzer, tone_val);
    delay(duration);
    noTone(buzzer);
}

ChessClock::ChessClock(ClockConfig &cfg) : config(cfg),
                                           available_minutes{0, 1, 2, 3, 5, 10, 15, 20, 30, 60},
                                           display1(cfg.disp_p1_clk, cfg.disp_p1_dio),
                                           display2(cfg.disp_p2_clk, cfg.disp_p2_dio),
                                           pot_min(cfg.pot_min_pin),
                                           pot_sec(cfg.pot_sec_pin)
{
    pinMode(config.btn_p1_pin, INPUT_PULLUP);
    pinMode(config.btn_p2_pin, INPUT_PULLUP);
    pinMode(config.btn_ctrl_pin, INPUT_PULLUP);

    reset();

    display1.init();
    display2.init();
}

void ChessClock::reset()
{
    mode = 0;
    current_turn = 1;
    timer = 0;

    blink_timer = 0;
    lit = true;

    paused = false;
    first_turn = true;

    timer_p1 = 0;
    timer_p2 = 0;

    addon_p1 = 0;
    addon_p2 = 0;

    prev_p1_btn = false;
    prev_p2_btn = false;
    prev_fun_btn = false;
}

void ChessClock::loop()
{
    switch (mode)
    {
    case 0:
        onSetTime(&display1);
        break;

    case 1:
        onSetAddonTime(&display1);
        break;

    case 2:
        onSetTime(&display2);
        break;

    case 3:
        onSetAddonTime(&display2);
        break;

    case 4:
        onPlayTurn();
        break;

    case 5:
        onFinish();
        break;
    }
}

void ChessClock::onSetTime(TM1637 *display)
{
    display->set(2);
    int minutes = pot_min.getReading(9);
    delay(10);
    int seconds = pot_min.getReading(5);
    delay(10);

    minutes = available_minutes[minutes];
    long d_seconds = 1000 * (60 * (long)minutes + 10 * (long)seconds);
    displayTime(d_seconds, *display, true);

    boolean state_fun_btn = digitalRead(config.btn_ctrl_pin);
    minutes = minutes + 1;
    if (d_seconds != 0 && !state_fun_btn && prev_fun_btn)
    {
        if (mode == 0)
            timer_p1 = d_seconds;
        else if (mode == 2)
            timer_p2 = d_seconds;
        playSound(config.buzzer_pin, 1000, 20);
        mode++;
    }
    prev_fun_btn = state_fun_btn;
}

void ChessClock::onSetAddonTime(TM1637 *display)
{
    delay(10);
    int seconds = pot_min.getReading(10);
    long d_seconds = 1000 * (long)seconds;

    displayTime(d_seconds, *display, true);

    boolean state_fun_btn = digitalRead(config.btn_ctrl_pin);

    if (!state_fun_btn && prev_fun_btn)
    {
        if (mode == 1)
        {
            addon_p1 = d_seconds;
            displayTime(timer_p1, display1, true);
        }
        else if (mode == 3)
        {
            addon_p1 = d_seconds;
            displayTime(timer_p2, display2, true);
        }
        mode++;
        playSound(config.buzzer_pin, 1000, 20);
    }
    prev_fun_btn = state_fun_btn;
}

void ChessClock::onPlayTurn()
{
    if (!paused)
    {
        playTurn();
    }
    else
    {
        blinkDisplay(blink_timer, lit, mode, display1, display2, timer_p1, timer_p2);
    }

    checkPause();
}

void ChessClock::playTurn()
{
    unsigned long time = millis();
    if (timer_p1 <= 0)
    {
        handleOutOfTime(timer_p1, &display1);
        return;
    }
    else if (timer_p2 <= 0)
    {
        handleOutOfTime(timer_p2, &display2);
        return;
    }
    else
    {
        // If no one ran out of time, decrease current player's
        // turn timer.
        if (current_turn == 1)
            timer_p1 -= time - timer;
        else if (current_turn == 2)
            timer_p2 -= time - timer;
    }

    // Update game timer and displays, and pass turn.
    timer = time;
    displayTime(timer_p1, display1);
    displayTime(timer_p2, display2);
    handlePassTurn();
}

void ChessClock::handleOutOfTime(long &timer, TM1637 *display)
{
    timer = 0;
    displayTime(timer, *display, true);
    mode++;
}

void ChessClock::handlePassTurn()
{
    delay(10);
    boolean state_p1_btn = digitalRead(config.btn_p1_pin);
    delay(10);
    boolean state_p2_btn = digitalRead(config.btn_p2_pin);

    if (current_turn == 1 && !state_p1_btn && prev_p1_btn)
    {
        // Player 1 hits button on his turn.
        passTurn(timer_p1, addon_p1, &display1);
    }
    else if (current_turn == 2 && !state_p2_btn && prev_p2_btn)
    {
        // Player 2 hits button on his turn.
        passTurn(timer_p2, addon_p2, &display2);
    }

    prev_p1_btn = state_p1_btn;
    prev_p2_btn = state_p2_btn;
}

void ChessClock::passTurn(long &timer, int addon, TM1637 *display)
{
    current_turn = 1;
    if (!first_turn)
    {
        timer += addon;
    }
    first_turn = false;
    displayTime(timer, *display);
    playSound(config.buzzer_pin, 1000, 20);
}

void ChessClock::checkPause()
{
    boolean state_fun_btn = digitalRead(config.btn_ctrl_pin);
    if (!state_fun_btn && prev_fun_btn)
    {
        paused = !paused;
        timer = millis();
        playSound(config.buzzer_pin, 1000, 20);
    }

    prev_fun_btn = state_fun_btn;
}
