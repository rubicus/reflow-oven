#ifndef INIT_H
#define INII_H

#include "Arduino.h"
#include <LiquidCrystal.h>

extern LiquidCrystal lcd;

// some constant pin definitions
extern const int startstop_pin;
extern const int mode_pin;
extern const int relay_pin;

//some constant speed info
extern const long CLK_SPEED;
extern const unsigned int TIMER_DIV;
extern const int INTS_PER_SEC;
extern const float MS_PER_INT;
extern const int MIN_MS_PERCYCLE;
// minimum cycles to be turned on:
extern const int INTS_PER_MINCYCLE;

void init_lcd();
void setup();

#endif
