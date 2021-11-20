//
// AVR Micro Kernel

#define _KERNEL_BEGIN__       0x00000
#define _KERNEL_END__         0x00100

#define _KERNEL_FLAGS__       0x00010

// Flags
#define _KERNEL_STATE_NORMAL__          0x00
#define _KERNEL_STATE_OUT_OF_MEMORY__   0xff

#define _KERNEL_TIMEOUT__    40
#define _KEYBOARD_TIMEOUT__  10000

#define _COMMAND_TABLE_SIZE__  128

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
	
	uint8_t messageQueue[8];
	uint8_t queueCounter;
	
	// Message queue callback
	uint8_t (*callbackPointer)(uint8_t);
	
	Kernel() {
		
		// Initiate message queue
		for (uint8_t i=0; i < 8; i++) messageQueue[i] = 0xff;
		queueCounter=0;
		
		// Set default message callback procedure
		callbackPointer = &defaultCallbackProcedure;
		
	}
	
	void callCommandFunction(void) {
		
		// Function look up
		for (uint8_t i=0; i<_COMMAND_TABLE_SIZE__; i++) {
			
			if (function.functionState[i] == 0x00) continue;
			
			uint8_t count=1;
			for (uint8_t a=0; a<8; a++) {
				// Compare to keyboard string
				if (function.functionNameIndex[i][a] == console.keyboard_string[a]) count++;
				// Execute the command
				if (count == 8) {function.command_function_ptr[i](); return;}
			}
			
		}
		
	}
	
	
	// Post a message to the kernel
	void postMessage(uint8_t message) {
		
		// Shift down the queue
		messageQueue[7] = messageQueue[6];
		messageQueue[6] = messageQueue[5];
		messageQueue[5] = messageQueue[4];
		messageQueue[4] = messageQueue[3];
		messageQueue[3] = messageQueue[2];
		messageQueue[2] = messageQueue[1];
		messageQueue[1] = messageQueue[0];
		
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
		
		// Shift up the queue
		messageQueue[0] = messageQueue[1];
		messageQueue[1] = messageQueue[2];
		messageQueue[2] = messageQueue[3];
		messageQueue[3] = messageQueue[4];
		messageQueue[4] = messageQueue[5];
		messageQueue[5] = messageQueue[6];
		messageQueue[6] = messageQueue[7];
		
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
		console.lastChar = keyboard.read();
		
	}
	// Starts the kernel loop
	void run(void) {
		
		uint16_t kernelTimeOut   = _KERNEL_TIMEOUT__;
		uint16_t keyboardTimeOut = _KEYBOARD_TIMEOUT__;
		
		uint16_t kernelCounter   = 0;
		uint16_t keyboardCounter = 0;
		
		console.consoleNewPrompt();
		
		bool isActive=1;
		while(isActive) {
			
			kernelCounter++;
			if (kernelCounter > kernelTimeOut) {kernelCounter=0;
				while(peekMessage() != 0) processMessageQueue();
			}
			
			keyboardCounter++;
			if (keyboardCounter > keyboardTimeOut) {
				keyboardCounter=0; updateKeyboard();
			}
			
		}
		
		return;
	}
	
	// Update keyboard state
	void updateKeyboard(void) {
		
		uint8_t scanCodeAccepted = 0;
		uint8_t currentChar=0x00;
		
		uint8_t readKeyCode = keyboard.read();
		
		// Decode the scan code
		if (console.keyboard_string_length < 19) {
			// ASCII char
			currentChar = readKeyCode;
			if (readKeyCode > 0x1f) {scanCodeAccepted=1;}
			} else {
			// Allow backspace and enter past max string length
			if (readKeyCode < 0x03) currentChar = readKeyCode;
		}
		
		// Prevent wild key repeats
		if (console.lastChar == currentChar) {console.lastChar == currentChar; return;} console.lastChar = currentChar;
		
		// Backspace
		if (currentChar == 0x01) {
			if (console.keyboard_string_length > 0) {
				// Remove last char from display
				displayDriver.writeChar(0x20, console.cursorLine, console.keyboard_string_length);
				displayDriver.cursorSetPosition(console.cursorLine, console.keyboard_string_length);
				// and keyboard string
				console.keyboard_string_length--;
				console.keyboard_string[console.keyboard_string_length] = 0x20;
			}
		}
		
		// Enter
		if (currentChar == 0x02) {
			
			uint8_t currentKeyStringLength = console.keyboard_string_length;
			console.keyboard_string_length = 0;
			
			if (console.cursorLine == 3) {console.consoleShiftUp();}
			if (console.cursorLine == 2) console.cursorLine++;
			if (console.cursorLine == 1) console.cursorLine++;
			if (console.cursorLine == 0) {if (console.promptState == 0) {console.promptState++;} else {console.cursorLine++;}}
			
			if (currentKeyStringLength > 0) {console.cursorPos=0; callCommandFunction();}
			
			console.consoleClearString();
			console.consoleNewPrompt();
		}
		
		// ASCII key character accepted
		if (scanCodeAccepted == 1) {
			console.keyboard_string[console.keyboard_string_length] = currentChar;
			console.keyboard_string_length++;
			// Update cursor
			displayDriver.cursorSetPosition(console.cursorLine, console.keyboard_string_length+1);
			// Display keyboard string
			displayDriver.writeString(console.keyboard_string, console.keyboard_string_length+1, console.cursorLine, console.cursorPos);
			displayDriver.writeChar(console.promptCharacter, console.cursorLine, 0);
		}
		
	}
	
	struct CommandFunctionIndex {
		
		// Function type index (0 = no function)
		uint8_t functionState[_COMMAND_TABLE_SIZE__];
		// Function pointer index
		void (*command_function_ptr[_COMMAND_TABLE_SIZE__])();
		// Function name index
		char functionNameIndex[_COMMAND_TABLE_SIZE__][8];
		
		CommandFunctionIndex() {
			for (uint8_t i=0; i < 16; i++) {
				functionState[i] = 0x00;
				command_function_ptr[i] = nullfunction;
				for (uint8_t a=0; a < 8; a++) functionNameIndex[i][a] = 0x20;
			}
		}
		
		// Return a free command function slot index
		uint8_t getFreeIndex(void) {
			for (uint8_t i=0; i < _COMMAND_TABLE_SIZE__; i++) {if (functionState[i] == 0) return i+1;} return 0;
		}
		// Call a command function
		
		// Install a function pointer into the command function table
		void install(void(*function_ptr)(), const char name[], uint8_t name_length) {
			
			uint8_t index = getFreeIndex();
			if (index == 0) return; else index--;
			
			functionState[index] = 0xff;
			command_function_ptr[index] = function_ptr;
			for (uint8_t i=0; i < name_length; i++) functionNameIndex[index][i] = name[i];
			
			return;
		}
		
	};
	CommandFunctionIndex function;
	
	struct CommandConsole {
		
		char  promptCharacter;
		uint8_t promptState;
		uint8_t cursorLine;
		uint8_t cursorPos;
		
		uint8_t keyboard_string_length;
		char  keyboard_string[24];
		uint8_t keyboardState;
		uint8_t lastChar;
		
		CommandConsole () {
			
			promptCharacter = '>';
			cursorLine      = 0;
			cursorPos       = 0;
			
			keyboard_string_length = 0;
			consoleClearString();
			
			keyboardState=0;
			lastChar=0;
			
		}
		
		// Shift the display up one line
		void consoleShiftUp(void) {displayDriver.frameShiftUp();_delay_ms(100);}
		
		// Add a const char string to the console
		void consoleAddString(const char charArray[], uint8_t string_length) {
			displayDriver.writeString(charArray, string_length, cursorLine, cursorPos);
			cursorPos+=string_length;
			return;
		}
		void consoleAddString(string& charString, uint8_t string_length) {
			displayDriver.writeString(charString, string_length, cursorLine, cursorPos);
			cursorPos+=string_length;
			return;
		}
		
		// Add new blank line
		void consoleNewLine(void) {cursorPos = 0; if (cursorLine < 3) {cursorLine++;} else {consoleShiftUp();} return;}
		
		// Display a command prompt
		void consoleNewPrompt(void) {
			cursorPos = 1;
			displayDriver.writeChar(promptCharacter, cursorLine, 0);
			displayDriver.cursorSetPosition(cursorLine, 1);
		}
		
		// Clear the keyboard string
		void consoleClearString(void) {for (uint8_t i=0; i < 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
		
	};
	CommandConsole console;
	
};
Kernel kernel;

// Load kernel modules
#include "modules.h"

uint8_t defaultCallbackProcedure(uint8_t message) {
	
	switch (message) {
		default: kernel.function.command_function_ptr[message](); break;
	}
	
	return 0;
}



// Allocate and display total available system memory
uint32_t allocate_system_memory(void) {
	
	displayDriver.cursorSetPosition(1, 0);
	kernel.console.cursorLine = 1;
	kernel.console.cursorPos  = 0;
	
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


