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

    // prev_ flags are used to detect button pressing.
    // If the prev_ flag is set to true on pressed,
    // and to false on release. If the prev_ flag is true and the
    // current status is released, the button has been pressed.
    boolean prev_p1_btn;
    boolean prev_p2_btn;
    boolean prev_fun_btn;
};

#endif
