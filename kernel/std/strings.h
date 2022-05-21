//
// String structure

struct string {
	
	uint32_t ptr;
	uint8_t length;
	
	char& operator[] (uint8_t index) {return cache[ptr + index];}
	
	string() {
		ptr = stack_alloc(20);
		length = 20;
		clear();
	}
	
	string(uint8_t string_length) {
		ptr = stack_alloc(string_length);
		length = string_length;
		clear();
	}
	
	string(const char new_string[], uint8_t string_length) {
		ptr = stack_alloc(string_length);
		length = string_length;
		clear();
		
		for (uint8_t i=0; i<string_length; i++) cache[ptr + i] = new_string[i];
	}
	
	~string() {
		
		stack_free(length);
		
	}
	
	// Find a sub-string within the current string
	uint8_t find(const char sub_string[], uint8_t sub_string_size) {
		
		// Loop base string
		for (uint8_t i=0; i < length; i++) {
			
			uint8_t counter=0;
			
			// Loop/check sub string
			for (uint8_t s=0; s <= sub_string_size; s++) {
				
				char readByte = cache[ptr + i + s];
				
				if (readByte == sub_string[s]) {
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
				
				char readByte = cache[ptr + i + s];
				
				if (readByte == sub_string[s]) {
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
		for (uint8_t i=0; i < strSz; i++) cache[ptr + start_offset+i] = source_string[i];
		return;
	}
	void insert(string& source_string, uint16_t start_offset=0) {
		uint8_t strSz = (source_string.length - 1);
		for (uint8_t i=0; i < strSz; i++) cache[ptr + start_offset+i] = source_string[i];
		return;
	}
	
	// Clear the string to space characters
	void clear(void) {for (uint8_t i=0; i<=length+1; i++) cache[ptr + i] = 0x20;}
	
	// Return the number of characters the string contains
	uint8_t size(void) {return length;}
	
};

