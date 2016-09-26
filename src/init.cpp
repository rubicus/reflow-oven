#include "init.h"

// Some hardware objects
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// some constant pin definitions
const int startstop_pin = 8;
const int mode_pin      = 9;
const int relay_pin = 6;

//some constant speed info
const long CLK_SPEED = 16000000;
const unsigned int TIMER_DIV = 62500;
const int INTS_PER_SEC = CLK_SPEED/TIMER_DIV;
const float MS_PER_INT = 1000.0/INTS_PER_SEC;
const int MIN_MS_PERCYCLE = 30; // we don't want shorter cycles than 30ms
// minimum cycles to be turned on:
const int INTS_PER_MINCYCLE = ceil(MIN_MS_PERCYCLE/MS_PER_INT);

void init_lcd() {
  lcd.begin(16,2);
  lcd.print("Reflow Oven");
  lcd.setCursor(0,1);
  lcd.print("Rev 3");

  delay(1000);
  lcd.clear();
}

void setup() {
  Serial.begin(9600);
  init_lcd();

  // Setup pins correctly
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);

  pinMode(startstop_pin, INPUT);
  digitalWrite(startstop_pin, HIGH);

  pinMode(mode_pin, INPUT);
  digitalWrite(mode_pin, HIGH);

  // Setup timer interrupt correctly
  cli(); // make sure an interrupt doesn't disturb us during update...
  TCCR1A = 0; //CTC mode (count to OCR1A and reset)
  TCCR1B = _BV(WGM12) | _BV(CS10); //CTC, no prescaling
  OCR1A = TIMER_DIV; // With 16 MHz clock exactly 256 Hz of interrupts
  TIMSK1 = _BV(OCIE1A);
  sei();
}
