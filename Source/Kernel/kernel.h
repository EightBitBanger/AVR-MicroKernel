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

// Function tables
#define _COMMAND_TABLE_SIZE__  64
#define _DRIVER_TABLE_SIZE__   16

typedef void(*FunctionPtr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#include "types.h"
#include "enums.h"

#include "stack_allocator.h"
#include "device_index.h"

#include "string_const.h"
#include "strings.h"

//#include "drivers/display_driver.h"
//#include "drivers/file_system.h"
#include "drivers/keyboard.h"


uint8_t loadLibrary(const char[], uint8_t, void(*new_driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));
FunctionPtr& getFuncAddress(const char[], uint8_t);
uint8_t callExtern(FunctionPtr, uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Default message callback procedure
uint8_t defaultCallbackProcedure(uint8_t message);


//
// Device driver function table
struct DeviceDriverIndex {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][8];
	void (*driver_function_ptr[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverIndex() {
		for (uint8_t i=0; i <= _DRIVER_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a <= 8; a++) deviceNameIndex[i][a] = 0x20;
			driver_function_ptr[i] = 0;
		}
	}
	
};
DeviceDriverIndex deviceDriverIndex;

// Load device drivers
#include "drivers.h"


// Command console interface
struct CommandConsole {
	
	char  promptCharacter;
	uint8_t promptState;
	uint8_t cursorLine;
	uint8_t cursorPos;
	
	uint8_t keyboard_string_length;
	char  keyboard_string[24];
	uint8_t keyboardState;
	uint8_t lastChar;
	
	CommandConsole() {
		
		promptCharacter = '>';
		promptState     = 0;
		cursorLine      = 0;
		cursorPos       = 0;
		
		keyboard_string_length = 0;
		clearString();
		
		keyboardState=0;
		lastChar=0;
		
	}
	
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
	void newLine(void) {cursorPos = 0; if (cursorLine < 3) {cursorLine++;} else {shiftUp();} return;}
	
	// Display a command prompt
	void newPrompt(void) {
		cursorPos = 1;
		displayDriver.writeChar(promptCharacter, cursorLine, 0);
		displayDriver.cursorSetPosition(cursorLine, 1);
	}
	
	// Clear the keyboard string
	void clearString(void) {for (uint8_t i=0; i <= 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
};
CommandConsole console;










struct Kernel {
	
	// Initiate kernel memory
	void initiate(void) {
		
		mem_zero(_ALLOCATOR_COUNTER_ADDRESS__, 4); // Number of external memory allocations
		mem_zero(_KERNEL_FLAGS__, 8);              // Kernel state flags
		
		// Initiate device drivers
		for (uint8_t i=0; i <= _DRIVER_TABLE_SIZE__; i++) {
			
			if ((deviceDriverIndex.driver_function_ptr[i] != 0) && (deviceDriverIndex.deviceNameIndex[i][0] != 0x20))
				deviceDriverIndex.driver_function_ptr[i](_DRIVER_INITIATE__, NULL, NULL, NULL, NULL);
			
		}
		
	}
	
	void shutdown(void) {
		
		// Initiate device drivers
		for (uint8_t i=0; i <= _DRIVER_TABLE_SIZE__; i++) {
			if ((deviceDriverIndex.driver_function_ptr[i] != 0) && (deviceDriverIndex.deviceNameIndex[i][0] != 0x20)) 
				deviceDriverIndex.driver_function_ptr[i](_DRIVER_SHUTDOWN__, NULL, NULL, NULL, NULL);
			
		}
		
	}
	
	// Starts the kernel loop
	void run(void) {
		
		uint16_t kernelTimeOut   = _KERNEL_TIMEOUT__;
		uint16_t keyboardTimeOut = _KEYBOARD_TIMEOUT__;
		
		uint16_t kernelCounter   = 0;
		uint16_t keyboardCounter = 0;
		
		console.lastChar = keyboard.read();
		console.promptCharacter = '>';
		console.newPrompt();
		
		bool isActive=1;
		while(isActive) {
			
			kernelCounter++;
			if (kernelCounter > kernelTimeOut) {kernelCounter=0;
				while(queue.peekMessage() != 0) queue.processMessage();
			}
			
			keyboardCounter++;
			if (keyboardCounter > keyboardTimeOut) {
				keyboardCounter=0; updateKeyboard();
			}
			
		}
		
		return;
	}
	
	
	
	// System message queue
	struct MessageQueue {
		
		uint8_t queue[8];
		uint8_t counter;
		
		// Message callback
		uint8_t (*callbackPointer)(uint8_t);
		
		MessageQueue() {
			
			// Initiate message queue
			for (uint8_t i=0; i < 8; i++) queue[i] = 0xff;
			counter=0;
			
			// Set default message callback procedure
			callbackPointer = &defaultCallbackProcedure;
			
		}
		
		// Post a message to the kernel
		void postMessage(uint8_t message) {
			
			// Shift down the queue
			queue[7] = queue[6];
			queue[6] = queue[5];
			queue[5] = queue[4];
			queue[4] = queue[3];
			queue[3] = queue[2];
			queue[2] = queue[1];
			queue[1] = queue[0];
			
			// Add to queue
			queue[0] = message;
			counter++;
			
			return;
		}
		// Check the number of messages currently in the queue
		uint8_t peekMessage(void) {return counter;}
		// Process a message from the message queue
		void processMessage(void) {
			
			if (counter == 0) return;
			
			// Grab the first message
			uint8_t message = queue[0];
			
			// Shift up the queue
			queue[0] = queue[1];
			queue[1] = queue[2];
			queue[2] = queue[3];
			queue[3] = queue[4];
			queue[4] = queue[5];
			queue[5] = queue[6];
			queue[6] = queue[7];
			
			// Decrement the queue
			counter--;
			
			// Call the callback procedure function with the message
			(callbackPointer)( message );
			
		}
		
	};
	MessageQueue queue;
	
	// Command line function table
	struct CommandFunctionIndex {
		
		char functionNameIndex[_COMMAND_TABLE_SIZE__][8];
		void (*command_function_ptr[_COMMAND_TABLE_SIZE__])();
		
		CommandFunctionIndex() {
			for (uint8_t i=0; i <= _COMMAND_TABLE_SIZE__; i++) {
				for (uint8_t a=0; a <= 8; a++) functionNameIndex[i][a] = 0x20;
				command_function_ptr[i] = nullfunction;
			}
		}
		
		// Install a function pointer into the command function table
		uint8_t install(void(*function_ptr)(), const char name[], uint8_t name_length) {
			
			// Find a free slot
			uint8_t index;
			for (index=0; index <= _COMMAND_TABLE_SIZE__; index++) {
				if (command_function_ptr[index] == nullfunction) break; else continue;
			}
			
			
			// Set the pointer
			for (uint8_t i=0; i < name_length; i++) functionNameIndex[index][i] = name[i];
			command_function_ptr[index] = function_ptr;
			
			return 1;
		}
		
	};
	CommandFunctionIndex function;
	
	
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
			
			if (console.cursorLine == 3) {console.shiftUp();}
			if (console.cursorLine == 2) console.cursorLine++;
			if (console.cursorLine == 1) console.cursorLine++;
			if (console.cursorLine == 0) {if (console.promptState == 0) {console.promptState++;} else {console.cursorLine++;}}
			
			// Execute the function
			if (currentKeyStringLength > 0) {console.cursorPos=0;
				
				// Function look up
				for (uint8_t i=0; i<_COMMAND_TABLE_SIZE__; i++) {
					
					if (function.command_function_ptr[i] == 0x00) continue;
					
					uint8_t count=1;
					for (uint8_t a=0; a<8; a++) {
						// Compare to keyboard string
						if (function.functionNameIndex[i][a] == console.keyboard_string[a]) count++;
						// Execute the command
						if (count == 8) {function.command_function_ptr[i](); return;}
					}
					
				}
				
			}
			
			console.clearString();
			console.newPrompt();
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
	
	
	
};
Kernel kernel;

// Load command modules
#include "modules.h"

uint8_t defaultCallbackProcedure(uint8_t message) {
	
	switch (message) {
		default: kernel.function.command_function_ptr[message](); break;
	}
	
	return 0;
}


// Load a device driver entry point function onto the driver function table
uint8_t loadLibrary(const char device_name[], uint8_t name_length, void(*new_driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > 8) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index <= _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverIndex.driver_function_ptr[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length; i++) deviceDriverIndex.deviceNameIndex[index][i] = device_name[i];
	deviceDriverIndex.driver_function_ptr[index] = new_driver_ptr;
	
	return 1;
}

// Get a library function address by its device name
FunctionPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > 8) return (FunctionPtr&)nullfunction;
	
	// Function look up
	for (uint8_t i=0; i <= _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverIndex.driver_function_ptr[i] == 0x00) continue;
		
		uint8_t count=0;
		for (count=0; count < name_length; count++)
		if (deviceDriverIndex.deviceNameIndex[i][count] == device_name[count]) {continue;} else {count=0; break;}
		
		if (count == name_length) return deviceDriverIndex.driver_function_ptr[i];
	}
	
	return (FunctionPtr&)nullfunction;
}

// Call an external function from a library function pointer
uint8_t callExtern(FunctionPtr library_function, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL) {
	
	for (uint8_t i=0; i <= _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverIndex.driver_function_ptr[i] == library_function) {
			library_function(function_call, paramA, paramB, paramC, paramD);
			return 1;
		}
		
	}
	
	return 1;
}


// Allocate and display total available system memory
uint32_t allocate_system_memory(void) {
	
	displayDriver.cursorSetPosition(1, 0);
	console.cursorLine = 1;
	console.cursorPos  = 0;
	
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


