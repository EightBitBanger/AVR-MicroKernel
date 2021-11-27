//
// Commonly used strings

const char string_memory_allocator_bytes[]  = "bytes";
const char string_memory_allocator_free[]   = "free";

const char message_device_disabled[]        = "Device disabled.";
const char message_task_not_found[]         = "Task not found.";

const char error_exmem_not_installed[]      = "Extended memory not installed.";
const char error_stack_error[]              = "Stack error.";

// Memory
const char sting_error_out_of_memory[]      = "Out of memory!";
const char message_error_seg_fault[]        = "Segmentation-fault";







uint8_t string_compare(const char stringA[], const char stringB[], uint8_t string_size) {
	for (uint8_t i=0; i < string_size; i++) {if (stringA[i] != stringB[i]) return 0;}
	return 1;
}

uint8_t string_compare(char stringA[], char stringB[], uint8_t string_size) {
	for (uint8_t i=0; i < string_size; i++) {if (stringA[i] != stringB[i]) return 0;}
	return 1;
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


