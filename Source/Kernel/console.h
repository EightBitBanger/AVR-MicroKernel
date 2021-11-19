//
// AVR kernel command console

#define _PROMPT_CHARACTER__  '>'

// Console command functions
void device_list(void);
void clear_screen(void);
void port_output(void);
void device_disable(void);

void command_list_files(void);
void command_allocate(void);
void command_deallocate(void);
void command_mem_test(void);
void command_system_test(void);

struct CommandConsole {
	
	// Cursor
	char promptCharacter;
	uint8_t promptState;
	uint8_t cursorLine;
	uint8_t cursorPos;
	
	// Keyboard string
	uint8_t keyboard_string_length;
	char keyboard_string[24];
	
	CommandConsole() {
		
		promptCharacter = _PROMPT_CHARACTER__;
		promptState     = 1;
		cursorLine      = 0;
		cursorPos       = 0;
		
		keyboard_string_length = 0;
		clearKeyboardString();
		
	}
	
	// Clear the keyboard string
	void clearKeyboardString(void) {for (uint8_t i=0; i < 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
	// Shift the display up one line
	void shiftUp(void) {
		displayDriver.frameShiftUp();
		_delay_ms(100);
	}
	
	// Add new string to the console
	void addString(const char string[], uint8_t string_length, uint8_t position_offset=0) {
		
		// Check last line
		if (cursorLine < 3) {
			
			displayDriver.writeString(string, string_length, cursorLine, cursorPos + position_offset);
			
			addNewLine();
			
		} else {
			
			displayDriver.writeString(string, string_length, cursorLine, cursorPos + position_offset);
			
			addNewLine();
			
		}
		
		return;
	}
	
	// Add blank new line
	void addNewLine(void) {
		
		cursorPos = 0;
		
		// Check last line
		if (cursorLine < 3) {
			
			cursorLine++;
			
			} else {
			
			shiftUp();
			
		}
		return;
	}
	
	// Display a command prompt
	void setPrompt(void) {
		cursorPos = 1;
		displayDriver.writeChar(promptCharacter, cursorLine, 0);
		displayDriver.cursorSetPosition(cursorLine, 1);
	}
	
	
	//
	// Execute the keyboard string with command prompt
	void executeKeyboardString(void) {
		
		// Begin a prompt on a new line
		uint8_t currentKeyStringLength = keyboard_string_length;
		keyboard_string_length = 0;
		
		// Check first line
		if (cursorLine == 0) {
			
			// Prompt start state catch
			if (promptState == 0) {promptState++;} else {cursorLine++;}
			
			if (currentKeyStringLength > 0) execute_command(keyboard_string);
			
			// Clear the keyboard string
			clearKeyboardString();
			setPrompt();
			
			return;
		}
		
		// Shift up on last line
		if (cursorLine == 3) {
			
			if (currentKeyStringLength > 0) {shiftUp(); execute_command(keyboard_string);} else {shiftUp();}
			
			// Clear the keyboard string and set a prompt
			clearKeyboardString();
			setPrompt();
			
			return;
		}
		
		// Increment cursor
		cursorLine++;
		
		if (currentKeyStringLength > 0) execute_command(keyboard_string);
		
		// Clear the keyboard string
		clearKeyboardString();
		
		// Set a new prompt
		setPrompt();
		
		return;
	}
	
	
	//
	// Execute a command from a string
	void execute_command(char command_string[]) {
		
		if (string_compare(command_string, "cls",     3) == 1) {clear_screen(); return;}
		if (string_compare(command_string, "port",    4) == 1) {port_output(); return;}
		if (string_compare(command_string, "device",  6) == 1) {device_list(); return;}
		if (string_compare(command_string, "disable", 7) == 1) {device_disable(); return;}
		if (string_compare(command_string, "mem",     3) == 1) {command_mem_test(); return;}
		if (string_compare(command_string, "dir",     3) == 1) {command_list_files(); return;}
		
		if (string_compare(command_string, "dir",     3) == 1) {command_list_files(); return;}
		
		
		return;
	}
	
};
CommandConsole console;





//
// Display memory usage
void command_mem_test(void) {
	
	uint32_t stackSz = stack_count();
	uint32_t memoryFree = _HEAP_END__ - (0x00100 + stackSz);
	
	string memorySz(string_memory_allocator, sizeof(string_memory_allocator));
	
	intToString(memoryFree, memorySz.str);
	console.addString(memorySz.str, sizeof(string_memory_allocator));
	
	return;
}

void clear_screen(void) {
	
	// Clear the buffer and mask
	displayDriver.clearBuffer();
	displayDriver.clearMask();
	console.clearKeyboardString();
	
	// Reset the cursor
	console.cursorLine  = 0;
	console.cursorPos   = 1;
	
	_delay_ms(100);
	
	// Initiate the prompt
	console.setPrompt();
	
	console.promptState = 1;
	return;
}

void device_list(void) {
	
	for (uint8_t i=0; i<8; i++) {
		
		// Get the device driver IDs
		char deviceID=0xff;
		memory_read( (_KERNEL_BEGIN__ + i), deviceID);
		
		// Check valid device ID
		if (deviceID != 0x00) {
			
			string deviceName;
			deviceName[0] = (0x31 + i); // Device slot number
			
			// Display console
			if (deviceID == 0x10) {
				
				deviceName.insert("Display", 8, 2);
				
				console.addString(deviceName.str, sizeof(deviceName.str));
				continue;
			}
			
			
		}
		
	}
	
	return;
}

void device_disable(void) {
	
	console.addNewLine();
	
	switch (console.keyboard_string[8]) {
		case 0x31: {memory_write(_DEVICE_INDEX__,   0xff); break;}
		case 0x32: {memory_write(_DEVICE_INDEX__+1, 0xff); break;}
		case 0x33: {memory_write(_DEVICE_INDEX__+2, 0xff); break;}
		case 0x34: {memory_write(_DEVICE_INDEX__+3, 0xff); break;}
		case 0x35: {memory_write(_DEVICE_INDEX__+4, 0xff); break;}
		case 0x36: {memory_write(_DEVICE_INDEX__+5, 0xff); break;}
		case 0x37: {memory_write(_DEVICE_INDEX__+6, 0xff); break;}
		case 0x38: {memory_write(_DEVICE_INDEX__+7, 0xff); break;}
	}
	
	return;
}

void port_output(void) {
	
	uint8_t dataByte=0x00;
	
	// Flip bits
	for (uint8_t i=0; i<8; i++) if (console.keyboard_string[7+i] == 0x31) {dataByte |= (1 << i);}
	
	// Write to selected port
	if (console.keyboard_string[5] == 0x61) 
		memory_write(0xc0000, dataByte);
		memory_write(0xd0000, dataByte);
	
	return;
}

void command_list_files(void) {
	
	// Display number of files
	string strValue("    Files", 9);
	uint32_t numberOfFiles = partition.getNumberOfFiles();
	intToString(numberOfFiles, strValue.str);
	console.addString(strValue.str, strValue.size());
	
	// List files
	for (uint32_t i=0; i < numberOfFiles; i++) {
		
		
		
	}
	
	return;
}






