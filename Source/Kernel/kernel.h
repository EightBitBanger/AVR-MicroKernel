//
// AVR Micro Kernel

#define _KERNEL_BEGIN__       0x00000
#define _KERNEL_END__         0x00100

#define _KERNEL_FLAGS__       0x00010

// Flags
#define _KERNEL_STATE_NORMAL__          0x00
#define _KERNEL_STATE_OUT_OF_MEMORY__   0xff

#define _MESSAGE_QUEUE_LENGTH__         32

#include "stack_allocator.h"
#include "device_index.h"

#include "string_const.h"
#include "strings.h"

#include "drivers/display_driver.h"
#include "drivers/file_system.h"
#include "drivers/keyboard.h"

// Default message callback procedure
uint8_t defaultCallbackProcedure(uint8_t message);

struct Kernel {
	
	void command_clear_screen(void) {
		// Clear the buffer and mask
		displayDriver.clearBuffer();
		displayDriver.clearMask();
		clearKeyboardString();
		
		// Reset the cursor
		cursorLine  = 0;
		cursorPos   = 1;
		
		_delay_ms(100);
		
		// Initiate the prompt
		consoleNewPrompt();
		
		promptState = 1;
	}
	
	char promptCharacter;
	uint8_t promptState;
	uint8_t cursorLine;
	uint8_t cursorPos;
	
	uint8_t keyboard_string_length;
	char keyboard_string[24];
	uint8_t keyboardState;
	uint8_t lastChar;
	
	uint8_t messageQueue[_MESSAGE_QUEUE_LENGTH__];
	uint8_t queueCounter;
	
	uint8_t (*callbackPointer)(uint8_t);
	
	Kernel() {
		
		promptCharacter = '>';
		cursorLine      = 0;
		cursorPos       = 0;
		
		keyboard_string_length = 0;
		clearKeyboardString();
		
		keyboardState=0;
		lastChar=0;
		
		// Initiate message queue
		for (uint8_t i=0; i <= _MESSAGE_QUEUE_LENGTH__; i++) 
			messageQueue[i] = 0xff;
		
		queueCounter=0;
		
		// Set default message callback procedure
		callbackPointer = &defaultCallbackProcedure;
		
	}
	
	// Post a message to the kernel
	void postMessage(uint8_t message) {
		
		// Shift up the queue
		for (uint8_t i=_MESSAGE_QUEUE_LENGTH__; i > 0; i--)
			messageQueue[i] = messageQueue[1-i];
		
		// Add to queue
		messageQueue[0] = message;
		queueCounter++;
		
		return;
	}
	
	// Check the number of messages currently in the queue
	uint8_t peekMessage(void) {return queueCounter;}
	
	// Process a message from the message queue
	void processMessageQueue(void) {
		
		if (queueCounter == 0) return;
		
		// Grab the first message
		uint8_t message = messageQueue[0];
		
		// Shift down the queue
		for (uint8_t i=0; i < _MESSAGE_QUEUE_LENGTH__; i++)
			messageQueue[i] = messageQueue[i+1];
		
		// Decrement the queue
		queueCounter--;
		
		// Call the callback procedure function with the message
		(callbackPointer)( message );
		
	}
	
	// Initiate kernel memory
	void initiate(void) {
		
		mem_zero(_ALLOCATOR_COUNTER_ADDRESS__, 4); // Number of external memory allocations
		mem_zero(_KERNEL_FLAGS__, 8);              // Kernel state flags
		
		// Setup keyboard
		lastChar = keyboard.read();
		
	}
	
	//
	// Starts the kernel loop
	void run(void) {
		
		uint16_t kernelTimeOut   = 10;
		uint16_t keyboardTimeOut = 500;
		
		uint16_t kernelCounter   = 0;
		uint16_t keyboardCounter = 0;
		
		consoleNewPrompt();
		
		bool isActive=1;
		while(isActive) {
			
			kernelCounter++;
			if (kernelCounter < kernelTimeOut) {continue;} kernelCounter=0;
			
			keyboardCounter++;
			if (keyboardCounter > keyboardTimeOut) {keyboardCounter=0;checkKeyboardState();}
			
			while(peekMessage() != 0) processMessageQueue();
			
		}
		
		return;
	}
	
	//
	// Command console
	
	// Clear the keyboard string
	void clearKeyboardString(void) {for (uint8_t i=0; i < 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
	// Shift the display up one line
	void shiftUp(void) {displayDriver.frameShiftUp();_delay_ms(100);}
	
	// Add a const char string to the console
	void addString(const char charArray[], uint8_t string_length) {
		displayDriver.writeString(charArray, string_length, cursorLine, cursorPos);
		cursorPos+=string_length;
		return;
	}
	void addString(string& charString, uint8_t string_length) {
		displayDriver.writeString(charString, string_length, cursorLine, cursorPos);
		cursorPos+=string_length;
		return;
	}
	
	// Add new blank line
	void consoleNewLine(void) {cursorPos = 0; if (cursorLine < 3) {cursorLine++;} else {shiftUp();} return;}
	
	// Display a command prompt
	void consoleNewPrompt(void) {
		cursorPos = 1;
		displayDriver.writeChar(promptCharacter, cursorLine, 0);
		displayDriver.cursorSetPosition(cursorLine, 1);
	}
	
	// Check keyboard input
	void checkKeyboardState(void) {
		
		uint8_t scanCodeAccepted = 0;
		uint8_t currentChar=0x00;
		
		uint8_t readKeyCode = keyboard.read();
		
		// Decode the scan code
		if (keyboard_string_length < 19) {
			// Check current code
			currentChar = readKeyCode;
			if (currentChar > 0x1f) scanCodeAccepted=1;
		} else {
			// Allow backspace and enter to pass max string length
			if (readKeyCode < 0x03) currentChar = readKeyCode;
		}
		
		// Prevent wild key repeats
		if (lastChar == currentChar) {return;} lastChar = currentChar;
		
		// Backspace
		if (currentChar == 0x01) {
			if (keyboard_string_length > 0) {
				// Remove last char from display
				displayDriver.writeChar(0x20, cursorLine, keyboard_string_length);
				displayDriver.cursorSetPosition(cursorLine, keyboard_string_length);
				// and keyboard string
				keyboard_string_length--;
				keyboard_string[keyboard_string_length] = 0x20;
			}
		}
		
		// Enter
		if (currentChar == 0x02) {executeKeyboardString(); return;}
		
		// ASCII key character accepted
		if (scanCodeAccepted == 1) {
			keyboard_string[keyboard_string_length] = currentChar;
			keyboard_string_length++;
			// Update cursor
			displayDriver.cursorSetPosition(cursorLine, keyboard_string_length+1);
			// Display keyboard string
			displayDriver.writeString(keyboard_string, keyboard_string_length+1, cursorLine, cursorPos);
			displayDriver.writeChar(promptCharacter, cursorLine, 0);
		}
		
	}
	
	// Execute the keyboard string
	void executeKeyboardString(void) {
		
		uint8_t currentKeyStringLength = keyboard_string_length;
		keyboard_string_length = 0;
		
		if (cursorLine == 3) {shiftUp();}
		if (cursorLine == 2) cursorLine++;
		if (cursorLine == 1) cursorLine++;
		if (cursorLine == 0) {if (promptState == 0) {promptState++;} else {cursorLine++;}}
		
		if (currentKeyStringLength > 0) {cursorPos=0; execute_command(keyboard_string);}
		
		clearKeyboardString();
		consoleNewPrompt();
		
		return;
	}
	
	
	//
	// Execute a command from a string
	void execute_command(char command_string[]) {
		
		if (string_compare(command_string, "cls",     3) == 1) {postMessage(0x01);}
		//if (string_compare(command_string, "port",    4) == 1) {port_output();}
		//if (string_compare(command_string, "device",  6) == 1) {device_list();}
		//if (string_compare(command_string, "disable", 7) == 1) {device_disable();}
		//if (string_compare(command_string, "mem",     3) == 1) {command_mem_test();}
		//if (string_compare(command_string, "dir",     3) == 1) {command_list_files();}
		
		consoleNewPrompt();
		
		return;
	}
	
	
};
Kernel kernel;

uint8_t defaultCallbackProcedure(uint8_t message) {
	
	switch (message) {
		
		case 0x00: {
			char String[] = "Message callback";
			kernel.addString(String, sizeof(String));
			return 1;
		}
		
		case 0x01: {
			kernel.command_clear_screen();
			break;
		}
		
		default:
			break;
	}
	
	return 0;
}




// Allocate and display total available system memory
uint32_t allocate_system_memory(void) {
	
	displayDriver.cursorSetPosition(1, 0);
	kernel.cursorLine = 1;
	kernel.cursorPos  = 0;
	
	uint32_t updateTimer=0;
	
	displayDriver.writeString(string_memory_allocator, sizeof(string_memory_allocator));
	
	char memoryString[8];
	for (uint8_t i=0; i<8; i++) memoryString[i] = 0x20;
	
#ifdef _TEST_RAM_EXTENSIVELY__
	char readByteTest=0x00;
#endif
	
	char readByte=0x00;
	uint32_t total_system_memory;
	
	// Copy the kernel memory before wiping the ram
	uint8_t kernelBuffer[0x1f];
	uint8_t kernelBufferSize = 0x1f;
	for (uint8_t address=0; address < kernelBufferSize; address++) {
		memory_read(address, readByte);
		kernelBuffer[address] = readByte;
	}
	
	for (total_system_memory=0x00000; total_system_memory < 0x40000; ) {
		
		// Run some memory cycles
		memory_write(total_system_memory, 0x55);
		memory_read(total_system_memory, readByte);
		
		// Check if the byte(s) stuck
#ifdef _TEST_RAM_EXTENSIVELY__
		// Run some more memory cycles
		memory_write(total_system_memory, 0xaa);
		memory_read(total_system_memory, readByteTest);
		if ((readByte == 0x55) && (readByteTest == 0xaa)) {
#else
		if (readByte == 0x55) {
#endif
		
#ifdef _FAST_BOOT__
		total_system_memory += 0x8000;
#else
		total_system_memory++;
#endif
		} else {
			
			if (total_system_memory == 0x00000) {
				
				// Restore the kernel memory
				for (uint8_t address=0; address < kernelBufferSize; address++) memory_write(address, kernelBuffer[address]);
				
				displayDriver.writeString(error_exmem_not_installed, sizeof(error_exmem_not_installed));
				displayDriver.cursorSetPosition(2, 0);
				while(1) nop;
			}
			break;
		}
		
		// Restore the kernel memory
		if ((total_system_memory < 0x0001f) || (total_system_memory == 0x8000)) {
			for (uint8_t address=0; address < kernelBufferSize; address++) memory_write(address, kernelBuffer[address]);
		}
		
		// Update display
		if (updateTimer > 1024) {updateTimer=0;
			
			intToString(total_system_memory, memoryString);
			displayDriver.writeString(memoryString, 7);
			
		} else {updateTimer++;}
		
	}
	
	// Calculate free memory
	uint32_t memoryFree = (total_system_memory);
	
	// Display available memory
	intToString(memoryFree, memoryString);
	displayDriver.writeString(memoryString, 7);
	
	return total_system_memory;
}


