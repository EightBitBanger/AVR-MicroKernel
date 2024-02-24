#ifndef _STRING_CASE_FUNCTIONS__
#define _STRING_CASE_FUNCTIONS__

#include <avr/io.h>

int is_number(uint8_t* charPtr);
int is_letter(uint8_t* charPtr);

int is_uppercase(uint8_t* charPtr);
int is_lowercase(uint8_t* charPtr);

void make_uppercase(uint8_t* charPtr);
void make_lowercase(uint8_t* charPtr);


#endif
