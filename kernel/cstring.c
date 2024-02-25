#include <kernel/cstring.h>

int is_number(uint8_t* charPtr) {
    if ((*charPtr >= 0x30) & (*charPtr <= 0x39))
        return 1;
    return 0;
}

int is_letter(uint8_t* charPtr) {
    if (is_uppercase(charPtr)) return 1;
    if (is_lowercase(charPtr)) return 1;
    return 0;
}


int is_uppercase(uint8_t* charPtr) {
    if ((*charPtr >= 0x41) & (*charPtr <= 0x5a))
        return 1;
    return 0;
}

int is_lowercase(uint8_t* charPtr) {
    if ((*charPtr >= 0x61) & (*charPtr <= 0x7a))
        return 1;
    return 0;
}

void make_uppercase(uint8_t* charPtr) {
    if (is_lowercase(charPtr) == 1) 
        charPtr -= 0x20;
    return;
}

void make_lowercase(uint8_t* charPtr) {
    if (is_uppercase(charPtr) == 1) 
        charPtr += 0x20;
    return;
}



uint8_t int_to_string(uint32_t number, uint8_t* destination_string) {
	
	uint8_t ones = 0x30;
	uint8_t tens = 0x30;
	uint8_t hnds = 0x30;
	uint8_t thou = 0x30;
	uint8_t ttho = 0x30;
	uint8_t htho = 0x30;
	uint8_t mill = 0x30;
	
	// Find number of characters
	uint8_t place=0;
	if (number > 0)      place = 1;
	if (number > 9)      place = 2;
	if (number > 99)     place = 3;
	if (number > 999)    place = 4;
	if (number > 9999)   place = 5;
	if (number > 99999)  place = 6;
	if (number > 999999) place = 7;
	
	// Convert to character
	while (number > 9999999) {number -= 10000000;}
	while (number > 999999)  {number -= 1000000;   mill += 1;}
	while (number > 99999)   {number -= 100000;    htho += 1;}
	while (number > 9999)    {number -= 10000;     ttho += 1;}
	while (number > 999)     {number -= 1000;      thou += 1;}
	while (number > 99)      {number -= 100;       hnds += 1;}
	while (number > 9)       {number -= 10;        tens += 1;}
	while (number > 0)       {number -= 1;         ones += 1;}
	
	// Figure length of string
	if (place == 0) destination_string[place]   = ones;
	if (place > 0)  destination_string[place-1] = ones;
	if (place > 1)  destination_string[place-2] = tens;
	if (place > 2)  destination_string[place-3] = hnds;
	if (place > 3)  destination_string[place-4] = thou;
	if (place > 4)  destination_string[place-5] = ttho;
	if (place > 5)  destination_string[place-6] = htho;
	if (place > 6)  destination_string[place-7] = mill;
	
	return place;
}


void int_to_hex_string(uint32_t integer, uint8_t* string) {
	
	uint8_t hex[2] = {0, 0};
	
	while (integer >= 16) {
		integer -= 16;
		hex[0]++;
	}
	
	while (integer > 0) {
		integer -= 1;
		hex[1]++;
	}
	
	if (hex[0] > 9) {hex[0] += 'a' - 10;} else {hex[0] += '0';}
	if (hex[1] > 9) {hex[1] += 'a' - 10;} else {hex[1] += '0';}
	
	string[0] = hex[0];
	string[1] = hex[1];
	
	return;
}
