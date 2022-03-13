#include <string.h>
#include "Arduino.h"
#include "ChessClock.h"
#include "Display.h"

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

    button_to_pin[PLAYER1] = config.btn_p1_pin;
    button_to_pin[PLAYER2] = config.btn_p2_pin;
    button_to_pin[CTRL] = config.btn_ctrl_pin;
}

void ChessClock::reset()
{
    display1.clear();
    display2.clear();

    mode = 0;
    current_turn = 1;
    timer = 0;

    reset_on_hold_timer = 0;

    paused = false;

    timer_p1 = 0;
    timer_p2 = 0;

    addon_p1 = 0;
    addon_p2 = 0;

    for (int i = 0; i < _TOTAL_BUTTONS; i++)
    {
        prev_btn_status[i] = false;
    }
}

boolean ChessClock::wasButtonPressed(Button button)
{
    /* Returns true if `button` was pressed on the previous loop and
    is now released */
    return prev_btn_status[button] && !getButtonStatus(button);
}

boolean ChessClock::getButtonStatus(Button button)
{
    int pin = button_to_pin[button];
    return static_cast<boolean>(digitalRead(pin));
}

void ChessClock::updateButtonsStatus()
{
    for (int i = 0; i <= _TOTAL_BUTTONS; i++)
    {
        Button button = static_cast<Button>(i);
        prev_btn_status[i] = getButtonStatus(button);
        delay(10);
    }
}

void ChessClock::loop()
{
    if (getButtonStatus(CTRL))
    {
        handleResetOnHold();
    }

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
        onGameStart();
        break;

    case 5:
        onPlayTurn();
        break;

    case 6:
        onFinish();
        break;
    }

    // Update button status at the end, otherwise prev_status == current_status will always
    // be true and wasButtonPressed() will always be false.
    updateButtonsStatus();
}

void ChessClock::handleResetOnHold()
{
    unsigned long time = millis();
    if (!prev_btn_status[CTRL])
    {
        reset_on_hold_timer = time;
    }
    if (time - reset_on_hold_timer >= 3000)
    {
        reset();
        reset_on_hold_timer = time;
    }
}

void ChessClock::onSetTime(Display *display)
{
    display->setBrightness(2);
    int minutes = pot_min.getReading(9);
    delay(10);
    int seconds = pot_sec.getReading(5);
    delay(10);

    minutes = available_minutes[minutes];
    unsigned long d_seconds = 1000 * (60 * (long)minutes + 10 * (long)seconds);
    display->displayTime(d_seconds, true);

    // CTRL confirms time but 0 seconds game is not allowed
    if (d_seconds != 0 && wasButtonPressed(CTRL))
    {
        if (mode == 0)
            timer_p1 = d_seconds;
        else if (mode == 2)
            timer_p2 = d_seconds;
        playSound(config.buzzer_pin, 1000, 20);
        mode++;
    }
}

void ChessClock::onSetAddonTime(Display *display)
{
    delay(10);
    int seconds = pot_min.getReading(10);
    unsigned long d_seconds = 1000 * (long)seconds;

    display->displayTime(d_seconds, true);

    // CTRL confirms time
    if (wasButtonPressed(CTRL))
    {
        if (mode == 1)
        {
            addon_p1 = d_seconds;
            display->displayTime(timer_p1, true);
        }
        else if (mode == 3)
        {
            addon_p1 = d_seconds;
            display->displayTime(timer_p2, true);
        }
        mode++;
        playSound(config.buzzer_pin, 1000, 20);
    }
}

void ChessClock::onGameStart()
{
    timer = millis();
    mode++;
}

void ChessClock::onPlayTurn()
{
    if (!paused)
    {
        playTurn();
    }
    else
    {
        blinkDisplays();
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
    display1.displayTime(timer_p1);
    display2.displayTime(timer_p2);
    handlePassTurn();
}

void ChessClock::handleOutOfTime(unsigned long &timer, Display *display)
{
    timer = 0;
    display->displayTime(timer, true);
    mode++;
    playSound(config.buzzer_pin, 200, 1000);
}

void ChessClock::handlePassTurn()
{
    if (current_turn == 1 && wasButtonPressed(PLAYER1))
    {
        passTurn(timer_p1, addon_p1, &display1);
    }
    else if (current_turn == 2 && wasButtonPressed(PLAYER2))
    {
        passTurn(timer_p2, addon_p2, &display2);
    }
}

void ChessClock::passTurn(unsigned long &timer, int addon, Display *display)
{
    current_turn = (current_turn == 1) ? 2 : 1;
    timer += addon;
    display->displayTime(timer);
    playSound(config.buzzer_pin, 1000, 20);
}

void ChessClock::checkPause()
{
    if (wasButtonPressed(CTRL))
    {
        paused = !paused;
        timer = millis();
        playSound(config.buzzer_pin, 1000, 20);
    }
}

void ChessClock::blinkDisplays()
{
    unsigned long time = millis();
    display1.blink(time);
    display2.blink(time);
}

void ChessClock::onFinish()
{
    blinkDisplays();

    if (wasButtonPressed(CTRL))
    {
        reset();
    }
}
