#ifndef TEMP_H
#define TEMP_H

#include <LiquidCrystal.h>

unsigned int itotemp(unsigned int input);
void temptos(unsigned int input, char* output);
unsigned int check_print_temp(LiquidCrystal &lcd);

#endif
