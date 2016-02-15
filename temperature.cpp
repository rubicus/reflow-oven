// Written by Oskar Södergren
// contains functions for dealing with integers as temperatures
// assuming 16 bit integers, with six fractional bits

#include <string.h>
#include <stdlib.h>
#include "temperature.h"

unsigned int itotemp(unsigned int input) {
  return input << 6;
}

// this function turns a temperature into a string
// the pointer given here must point towards at least 6 bytes of mem
void temptos(unsigned int input, char* output) {
  char fraction = input % 64;
  int int_part = input / 64;
  char int_string[4];
  char frac_string[2];

  //find the fraction digit
  char frac_digit = (fraction*2)/13;
  if (frac_digit % 13 > 6) // rounding
    ++frac_digit;
  if (frac_digit == 10) {
    frac_digit = 0;
    int_part++;
  }

  // Fix allignnment with spaces
  if (int_part < 10)
    strcpy(output, "  ");
  else if(int_part < 100)
    strcpy(output, " ");
  else
    strcpy(output, "");

  //check valid range and convert integer part
  if (int_part > 999)
    strcpy(int_string, "NaN");
  else
    itoa(int_part, int_string, 10); // convert to string
  strncat(output, int_string, 3);
  strcat(output, ".");

  // fix the fraction part
  itoa(frac_digit, frac_string, 10);
  strncat(output, frac_string, 1);

  return;
}
