#include <LiquidCrystal.h>
#include "temperature.h"
#include <math.h>
#include "init.h"

enum State {NONE, MENU, RAMPUP1, SOAK, RAMPUP2, PEAK, COOLDOWN};
String state_names[7] =
  { "Not initialized", "Menu", "Heating up...   ", "Soaking...      ",
    "Going up!       ", "peaking!        ", "Cooling down... "
  };

// Some flags for events
volatile char check_temp_f = 0;
volatile char check_button_f = 0;
volatile char next_PWM_cycle = 0; //the value for the next PWM cycle
volatile char update_pwm_f = 0;
// 0 is fully off, 255 fully on

// a soldering profile:
// Rampup1 speed (0-255), soak target, soak time, rampup2 speed(0-255),
// peak target temp, time at peak
const long pb_profile[6] = {255,140,45,200,205,20};
const char profile_name[] = "Old Fashioned Pb";

// when code in the main code wants to change the pwm duty cycle, this
// funtction should be called for safe access. This changes the duty
// cycle for the next PWM cycle.
void update_pwm_cycle(unsigned char new_cycle) {
  cli(); // clear interrupt flag for safe access to shared vars
  // Make sure the cycle is longer than the minimum cycle length
  // and to max out if not turned off enough
  if (new_cycle < INTS_PER_MINCYCLE)
    next_PWM_cycle = 0;
  else if (new_cycle > (255-INTS_PER_MINCYCLE))
    next_PWM_cycle = 255;
  else
    next_PWM_cycle = new_cycle;
  sei(); // reenable interrupts
  return;
}

void pwm_routine(unsigned char count_val) {
  static unsigned char duty_cycle = 255;
  if (duty_cycle == 255)
    digitalWrite(relay_pin, LOW); // 255 always means off
  else if (count_val == duty_cycle) {
    digitalWrite(relay_pin, HIGH);
  }

  if (count_val == 0) {
    // read next PWM cycle
    duty_cycle = 255-next_PWM_cycle;

    // Turn off, but in special case 0, keep on
    if (duty_cycle == 0)
      digitalWrite(relay_pin, HIGH);
    else
      digitalWrite(relay_pin, LOW);
  }
  return;
}

// here goes the interrupt vector. It's called at 256 Hz
ISR(TIMER1_COMPA_vect) {
  static unsigned char tmr_int_cnt = 0; // static so it retains value between calls
  ++tmr_int_cnt;

  pwm_routine(tmr_int_cnt);

  if(tmr_int_cnt % 64 == 0) {  // Check temp at 4 Hz (the chip is slow)
    check_temp_f = 1;
  }
  if(tmr_int_cnt % 4 == 0) { //check buttons every 16ms (64Hz) for debounce
    check_button_f = 1;
  }
  if(tmr_int_cnt == 230) // somewhere around here is a good place to update
    update_pwm_f = 1;
}
void reprint_state(State the_state) {
  lcd.setCursor (0,0);
  if (the_state != MENU) {
    lcd.print(state_names[the_state]);
  }
  else {
    lcd.print(profile_name);
  }
}

void loop () {
  //this loop needs to check button and temp and update things accordingly
  // it also needs to check buttons and do the right thing depending on state

  // Some local variables (statically to be retained between calls)
  static State the_state = NONE;
  static char last_startstopb_s = digitalRead(startstop_pin);
  static char last_modeb_s = digitalRead(mode_pin);
  static char startstop_pressed = 0;
  static char mode_pressed = 0;
  static unsigned int last_temp = itotemp(20);
  static long millis_at_stage_start = millis();

  // Button checking routine
  if (check_button_f) {
    check_button_f = 0;
    char startstop_s = digitalRead(startstop_pin);
    char mode_s = digitalRead(mode_pin);

    if (last_startstopb_s == HIGH && startstop_s == LOW)
      startstop_pressed = 1;
    if (last_modeb_s == HIGH && mode_s == LOW)
      mode_pressed = 1;

    last_startstopb_s = startstop_s;
    last_modeb_s = mode_s;
  }

  // Temp checking routine
  if (check_temp_f) {
    check_temp_f = 0;
    last_temp = check_print_temp(lcd);
  }

  // Main switch case for dealing with states
  char new_duty_cyc = 0;
  switch(the_state) {
  case MENU:
    new_duty_cyc = 0;
    if (startstop_pressed) {
      startstop_pressed = 0;
      the_state = RAMPUP1;
      millis_at_stage_start = millis();
      reprint_state(the_state);
    }
    if (mode_pressed) {
      mode_pressed = 0;
      reprint_state(the_state);
    }
    break;
  case RAMPUP1:
    new_duty_cyc = pb_profile[0]; // use PWM cycle from profile
    if (last_temp >= itotemp(pb_profile [1])) { // go to next stage at target temp
      the_state = SOAK;
      millis_at_stage_start = millis();
      reprint_state(the_state);
    }
    break;
  case SOAK:
    //try to hold this temp for the specified time
    if (last_temp < itotemp(pb_profile [1]))
      new_duty_cyc = 255;
    else
      new_duty_cyc = 0;
    if ((millis()-millis_at_stage_start > (pb_profile[2]*1000))) {
      the_state = RAMPUP2;
      millis_at_stage_start = millis();
      reprint_state(the_state);
    }
    break;
  case RAMPUP2:
    //like first rampup, get duty cycle from profile and cont. at targ. temp.
    new_duty_cyc = pb_profile[3];
    if (last_temp > itotemp(pb_profile [4])) {
      the_state = PEAK;
      millis_at_stage_start = millis();
      reprint_state(the_state);
    }
    break;
  case PEAK:
    // as in soak, keep temp
    if (last_temp < itotemp(pb_profile [4]))
      new_duty_cyc = 255;
    else
      new_duty_cyc = 0;
    if ((millis()-millis_at_stage_start > (pb_profile[5]*1000))) {
      the_state = COOLDOWN;
      millis_at_stage_start = millis();
      reprint_state(the_state);
    }
    break;
  case COOLDOWN:
    //turn off until reasonably cool
    new_duty_cyc = 0;
    if (last_temp < itotemp(60)) {
      the_state = MENU;
      millis_at_stage_start = millis();
      reprint_state(the_state);
    }
    break;
  default:
    new_duty_cyc = 0;
    the_state = MENU;
    reprint_state(the_state);
    break;
  } // end of switch

  // check startstop button, and turn off if flicked
  if (startstop_pressed) {
    startstop_pressed = 0;
    the_state = MENU;
    new_duty_cyc = 0;
    reprint_state(the_state);
  }

  if (update_pwm_f) { // update the pwm duty cycle, and draw state to screen
    static unsigned char screenupdate = 0;
    screenupdate++;
    update_pwm_f = 0;
    update_pwm_cycle(new_duty_cyc);
    if (screenupdate >= 30) { //fix the screen every 30 sec or so, just in case
      screenupdate = 0;
      reprint_state(the_state);
    }
  }
}
