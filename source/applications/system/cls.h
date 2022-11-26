//
// Clear screen command

void command_clear_screen(uint8_t function, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	// Clear the buffer and mask
	console.clearBuffer();
	console.clearMask();
	console.clearKeyboardString();
	
	// Reset the cursor
	console.cursorLine  = 0;
	console.cursorPos   = 1;
	
	// Initiate the prompt
	console.promptState = 1;
	
	return;
}


