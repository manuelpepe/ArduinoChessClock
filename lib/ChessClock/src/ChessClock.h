#ifndef ChessClock_h
#define ChessClock_h

#include "Arduino.h"
#include "Potentiometer.h"
#include "Display.h"
#include "ClockConfig.h"

class ChessClock
{

public:
    ChessClock(ClockConfig &cfg);
    void loop();  // Should be called once in `loop()`
    void reset(); // Resets state

    enum Button
    {
        PLAYER1 = 0,
        PLAYER2,
        CTRL,
        _TOTAL_BUTTONS = CTRL
    };

private:
    // State handlers
    void onSetTime(Display *display);
    void onSetAddonTime(Display *display);
    void onGameStart();
    void onPlayTurn();
    void onFinish();

    // Helper methods
    void handleOutOfTime(unsigned long &timer, Display *display);
    void handlePassTurn();
    void passTurn(unsigned long &timer, int addon, Display *display);
    void playTurn();
    void checkPause();
    void blinkDisplays();

    void updateButtonsStatus();
    boolean getButtonStatus(Button button);
    boolean wasButtonPressed(Button button);
    void handleResetOnHold();

    // Attributes
    ClockConfig &config;

    int mode;
    int current_turn;    // 1 or 2 depending on which player's turn it is.
    unsigned long timer; // Running time of game as miliseconds

    unsigned long reset_on_hold_timer;

    unsigned long blink_timer; // For synchronized display blinking

    const int available_minutes[10];

    boolean paused;
    boolean first_turn; // Used to avoid addon time on first turn.

    Display display1;
    Display display2;

    Potentiometer pot_min;
    Potentiometer pot_sec;

    unsigned long timer_p1; // Current time for player 1
    unsigned long timer_p2; // Current time for player 2

    int addon_p1; // Increment time for player 1
    int addon_p2; // Increment time for player 2

    int button_to_pin[_TOTAL_BUTTONS + 1];
    boolean prev_btn_status[_TOTAL_BUTTONS + 1];
};

#endif
