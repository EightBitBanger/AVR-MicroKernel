#ifndef _STRING_CASE_FUNCTIONS__
#define _STRING_CASE_FUNCTIONS__

#include <avr/io.h>

// Case

int is_number(uint8_t* charPtr);
int is_letter(uint8_t* charPtr);

int is_uppercase(uint8_t* charPtr);
int is_lowercase(uint8_t* charPtr);

void make_uppercase(uint8_t* charPtr);
void make_lowercase(uint8_t* charPtr);

// Conversion

uint8_t int_to_string(uint32_t number, uint8_t* destination_string);
void int_to_hex_string(uint32_t integer, uint8_t* string);

#endif
