#ifndef _STRING_CASE_FUNCTIONS__
#define _STRING_CASE_FUNCTIONS__

#include <avr/io.h>

// Checks

uint8_t is_number(uint8_t* charPtr);
uint8_t is_letter(uint8_t* charPtr);
uint8_t is_symbol(uint8_t* charPtr);
uint8_t is_hex(uint8_t* charPtr);

// Case

uint8_t is_uppercase(uint8_t* charPtr);
uint8_t is_lowercase(uint8_t* charPtr);

void uppercase(uint8_t* charPtr);
void lowercase(uint8_t* charPtr);

// Conversion

uint8_t int_to_string(uint32_t number, uint8_t* destination_string);
void int_to_hex_string(uint32_t integer, uint8_t* string);

uint8_t string_get_int(uint8_t* string);
uint32_t string_get_int_long(uint8_t* string);

uint8_t string_get_hex_char(uint8_t* string);

// Standard
uint8_t StringCompare(uint8_t* stringA, uint8_t lengthA, uint8_t* stringB, uint8_t lengthB);

#endif