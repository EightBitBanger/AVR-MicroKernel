//
// String functions

#define _STRING_SIZE__  20

/*
struct stack_string {
	
	uint32_t ptr;
	
	uint8_t length;
	
	char& operator[] (uint8_t index) {return memory_cache[ptr + index];}
	
	string(uint8_t string_size) {
		
		ptr = stack_alloc(string_size);
		
	}
	
	string(const char new_string[], uint8_t string_length) {
		
		ptr = stack_alloc(string_length);
		
		length = string_length;
		
		for (uint8_t i=0; i<string_length; i++)
		memory_cache[ptr + i] = new_string[i];
		
	}
	~string() {
		
		stack_free(length);
		
	}
	
	void clear(void) {
		for (uint8_t i=0; i<_STRING_SIZE__; i++) {
			memory_cache[ptr + i] = 0x20;
		}
	}
	
};
*/

struct string {
	
	char str[_STRING_SIZE__];
	
	uint8_t length;
	
	char& operator[] (uint8_t index) {return str[index];}
	
	string() {
		clear();
	}
	
	string(const char new_string[], uint8_t string_size) {
		
		length = string_size;
		
		for (uint8_t i=0; i<string_size; i++) str[i] = new_string[i];
		
	}
	
	void clear(void) {for (uint8_t i=0; i<_STRING_SIZE__; i++) str[i]=0x20; length=0;}
	
	// Find a sub-string within the current string
	uint8_t find(const char sub_string[], uint8_t sub_string_size) {
		
		for (uint8_t i=0; i < _STRING_SIZE__; i++) {
			
			uint8_t counter=0;
			
			for (uint8_t s=0; s <= sub_string_size; s++) {
				
				if (str[i + s] == sub_string[s]) {
					counter++;
					
					// Check full string match
					if (counter == sub_string_size) return 1;
					
					continue;
				}
				
				break;
			}
			
		}
		
		return 0;
	}
	
	void insert(const char source_string[], uint8_t string_size, uint16_t start_offset=0) {
		uint8_t strSz = (string_size-1);
		for (uint8_t i=0; i < strSz; i++) {str[start_offset+i] = source_string[i];}
		return;
	}
	
	uint32_t size(void) {return length;}
	
};











uint8_t string_compare(const char stringA[], const char stringB[], uint8_t string_size) {
	for (uint8_t i=0; i < string_size; i++) {if (stringA[i] != stringB[i]) return 0;}
	return 1;
}

void string_clear(char string[], uint8_t string_size) {for (uint8_t i=0; i<string_size; i++) string[i]=0x20;}

// Convert integer number to string of characters
uint8_t intToString(uint32_t number, char String[]) {
	
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
	
	// Place in the string
	if (place == 0) String[place] = ones;
	if (place > 0) String[place-1] = ones;
	if (place > 1) String[place-2] = tens;
	if (place > 2) String[place-3] = hnds;
	if (place > 3) String[place-4] = thou;
	if (place > 4) String[place-5] = ttho;
	if (place > 5) String[place-6] = htho;
	if (place > 6) String[place-7] = mill;
	
	return 0;
}

// Return the number of sub characters in the string
uint8_t string_char_count(char string[], unsigned int string_size, char sub_character) {
	
	uint8_t count=0;
	
	for (uint8_t i=0; i < string_size; i++) {
		if (string[i] == sub_character) count++;
	}
	
	return count;
}


