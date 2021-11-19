//
// String functions

struct string {
	
	uint32_t ptr;
	uint8_t length;
	
	char& operator[] (uint8_t index) {return memory_cache[ptr + index];}
	
	string(uint8_t string_length) {
		ptr = stack_alloc(string_length);
		length = string_length;
		clear();
	}
	
	string(const char new_string[], uint8_t string_length) {
		
		ptr = stack_alloc(string_length);
		length = string_length;
		
		for (uint8_t i=0; i<string_length; i++) memory_cache[ptr + i] = new_string[i];
	}
	~string() {stack_free(length);}
	
	// Find a sub-string within the current string
	uint8_t find(const char sub_string[], uint8_t sub_string_size) {
		
		// Loop base string
		for (uint8_t i=0; i < length; i++) {
			
			uint8_t counter=0;
			
			// Loop/check sub string
			for (uint8_t s=0; s <= sub_string_size; s++) {
				
				if (memory_cache[ptr + i + s] == sub_string[s]) {
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
	uint8_t find(string& sub_string) {
		
		// Loop base string
		for (uint8_t i=0; i < length; i++) {
			
			uint8_t counter=0;
			
			// Loop/check sub string
			for (uint8_t s=0; s <= sub_string.length; s++) {
				
				if (memory_cache[ptr + i + s] == sub_string[s]) {
					counter++;
					
					// Check full string match
					if (counter == sub_string.length) return 1;
					
					continue;
				}
				
				break;
			}
			
		}
		
		return 0;
	}
	
	// Insert a string into the current string starting at the offset in the current string
	void insert(const char source_string[], uint8_t string_size, uint16_t start_offset=0) {
		uint8_t strSz = (string_size - 1);
		for (uint8_t i=0; i < strSz; i++) memory_cache[ptr + start_offset+i] = source_string[i];
		return;
	}
	void insert(string& source_string, uint16_t start_offset=0) {
		uint8_t strSz = (source_string.length - 1);
		for (uint8_t i=0; i < strSz; i++) memory_cache[ptr + start_offset+i] = source_string[i];
		return;
	}
	
	// Clear the string to space characters
	void clear(void) {for (uint8_t i=0; i<=length+1; i++) memory_cache[ptr + i] = 0x20;}
	
	// Return the number of characters the string contains
	uint8_t size(void) {return length;}
	
};






uint8_t string_compare(const char stringA[], const char stringB[], uint8_t string_size) {
	for (uint8_t i=0; i < string_size; i++) {if (stringA[i] != stringB[i]) return 0;}
	return 1;
}

// Convert integer number to string of characters
uint8_t intToString(uint32_t number, string& destination_string) {
	
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

uint8_t intToString(uint32_t number, char destination_string[]) {
	
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

// Return the number of sub characters in the string
uint8_t string_char_count(char string[], unsigned int string_size, char sub_character) {
	
	uint8_t count=0;
	
	for (uint8_t i=0; i < string_size; i++) {
		if (string[i] == sub_character) count++;
	}
	
	return count;
}


