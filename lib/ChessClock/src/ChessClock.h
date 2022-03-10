#ifndef ChessClock_h
#define ChessClock_h

#include "Arduino.h"
#include "Potentiometer.h"
#include "ClockConfig.h"
#include "TM1637.h"

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
    void onSetTime(TM1637 *display);
    void onSetAddonTime(TM1637 *display);
    void onPlayTurn();
    void onFinish();

    // Helper methods
    void handleOutOfTime(long &timer, TM1637 *display);
    void handlePassTurn();
    void passTurn(long &timer, int addon, TM1637 *display);
    void playTurn();
    void checkPause();
    void updateButtonsStatus();
    boolean getButtonStatus(Button button);
    boolean wasButtonPressed(Button button);

    // Attributes
    ClockConfig &config;

    int mode;
    int current_turn;    // 1 or 2 depending on which player's turn it is.
    unsigned long timer; // Running time of game as miliseconds

    long blink_timer; // For display blinking
    boolean lit;      // For display blinking

    const int available_minutes[10];

    boolean paused;
    boolean first_turn; // Used to avoid addon time on first turn.

    TM1637 display1;
    TM1637 display2;

    Potentiometer pot_min;
    Potentiometer pot_sec;

    long timer_p1; // Current time for player 1
    long timer_p2; // Current time for player 2

    int addon_p1; // Increment time for player 1
    int addon_p2; // Increment time for player 2

    int button_to_pin[_TOTAL_BUTTONS];
    boolean prev_btn_status[_TOTAL_BUTTONS];
};

#endif
