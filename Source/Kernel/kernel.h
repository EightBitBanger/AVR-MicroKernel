//
// AVR Micro Kernel

// Kernel address memory
#define _KERNEL_BEGIN__       0x00000
#define _KERNEL_END__         0x000ff

#define _KERNEL_FLAGS__       0x00010

// Flags
#define _KERNEL_STATE_NORMAL__          0x00
#define _KERNEL_STATE_OUT_OF_MEMORY__   0xa0
#define _KERNEL_STATE_SEG_FAULT__       0xff

// Counters
#define _KERNEL_TIMEOUT__    1000
#define _KEYBOARD_TIMEOUT__  10000

// Function tables
#define _COMMAND_TABLE_SIZE__  64
#define _DRIVER_TABLE_SIZE__   16

typedef void(*FunctionPtr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#include "enums.h"
#include "types.h"

#include "stack_allocator.h"
#include "device_index.h"

#include "string_const.h"
#include "strings.h"

//#include "drivers/file_system.h"
#include "drivers/keyboard.h"


// Prototypes
void updateKeyboard(void);

// Default message callback procedure
uint8_t defaultCallbackProcedure(uint8_t message);

#include "driver_table.h"

#include "console.h"

#include "memory_check.h"


// Command line function table
struct CommandFunctionTable {
	
	char functionNameIndex[_COMMAND_TABLE_SIZE__][8];
	void (*command_function_table[_COMMAND_TABLE_SIZE__])();
	
	CommandFunctionTable() {
		for (uint8_t i=0; i < _COMMAND_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a < 8; a++) functionNameIndex[i][a] = 0x20;
			command_function_table[i] = &nullfunction;
		}
	}
	
};
CommandFunctionTable moduleTable;

// Install a function pointer into the function table
uint8_t installModule(void(*function_ptr)(), const char name[], uint8_t name_length) {
	
	// Find a free slot
	uint8_t index;
	for (index=0; index < _COMMAND_TABLE_SIZE__; index++)
	if (moduleTable.command_function_table[index] == &nullfunction) break;
	
	// Set the name and pointer
	for (uint8_t i=0; i < name_length-1; i++)
	moduleTable.functionNameIndex[index][i] = name[i];
	
	moduleTable.command_function_table[index] = function_ptr;
	
	return 1;
}

// Load command modules
#include "modules.h"












struct Kernel {
	
	// Initiate kernel memory
	void initiate(void) {
		
		mem_zero(_ALLOCATOR_COUNTER_ADDRESS__, 4); // Number of external memory allocations
		mem_zero(_KERNEL_FLAGS__, 8);              // Kernel state flags
		
		// Initiate device drivers
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			
			if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20))
				deviceDriverTable.driver_entrypoint_table[i](_DRIVER_INITIATE__, NULL, NULL, NULL, NULL);
			
		}
		
	}
	
	void shutdown(void) {
		
		// Shutdown device drivers
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) 
				deviceDriverTable.driver_entrypoint_table[i](_DRIVER_SHUTDOWN__, NULL, NULL, NULL, NULL);
			
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
		console.printPrompt();
		
		bool isActive=1;
		while(isActive) {
			
			kernelCounter++;
			if (kernelCounter > kernelTimeOut) {kernelCounter=0;
				while(queue.peekMessage() != 0) queue.processMessage();
				//checkKernelState();
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
	
	// Check kernel error flags
	char checkKernelState(void) {
		
		char flag=0;
		memory_read(_KERNEL_FLAGS__, flag);
		
		switch (flag) {
			
			case _KERNEL_STATE_SEG_FAULT__: {
				console.printLn();
				console.print(message_error_seg_fault, sizeof(message_error_seg_fault));
				console.printLn();
				console.printPrompt();
				memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_NORMAL__);
				return _KERNEL_STATE_SEG_FAULT__;
			}
			
			default: break;
		}
		
		return 0x00;
	}
	
	
};
Kernel kernel;

// Keyboard event handling
#include "keyboard_events.h"

uint8_t defaultCallbackProcedure(uint8_t message) {
	
	switch (message) {
		
		
		default: moduleTable.command_function_table[message](); break;
	}
	
	return 0;
}


