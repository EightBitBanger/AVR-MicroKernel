#ifndef __C_STRING____
#define __C_STRING____


// Return the number of given sub characters in the string
uint8_t strsub(char* string, unsigned int string_size, char sub_character);
// Compare a string to another string of the length specified
uint8_t strcmp(char* string_a, char* string_b, uint8_t string_length);

// Convert an integer to a string representation of the given number
uint8_t int_get_string(uint32_t number, char* destination_string);
// Returns the value from the given hex chars. String should contain two hex chars ex: "3f"
uint8_t string_get_hex_char(char* string);



uint8_t strsub(char* string, unsigned int string_size, char sub_character) {
	
	uint8_t count=0;
	
	for (uint8_t i=0; i < string_size; i++) {
		if (string[i] == sub_character) count++;
	}
	
	return count;
}


uint8_t strcmp(char* string_a, char* string_b, uint8_t string_length) {
	for (uint8_t i=0; i < string_length-1; i++) {if (string_a[i] != string_b[i]) return 0;}
	return 1;
}


uint8_t int_get_string(uint32_t number, char* destination_string) {
	
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

uint8_t string_get_hex_char(char* string) {
	
	uint32_t value_a = 0;
	uint32_t value_b = 0;
	uint8_t hex;
	
	// First digit
	hex = string[1];
	if ((hex > 0x30) && (hex < 0x40)) value_b += hex - 0x30;
	if ((hex > 0x60) && (hex < 0x67)) value_b += 9 + (hex - 0x60);
	
	// Second digit
	hex = string[0];
	if ((hex > 0x30) && (hex < 0x40)) value_a += hex - 0x30;
	if ((hex > 0x60) && (hex < 0x67)) value_a += 9 + (hex - 0x60);
	
	return value_a + (value_b * 16);
}

#endif


