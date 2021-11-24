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

uint8_t loadLibrary(const char[], uint8_t, void(*new_driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));
FunctionPtr& getFuncAddress(const char[], uint8_t);
uint8_t callExtern(FunctionPtr, uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Default message callback procedure
uint8_t defaultCallbackProcedure(uint8_t message);


//
// Device driver function table
struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][8];
	void (*driver_function_ptr[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a < 8; a++) deviceNameIndex[i][a] = 0x20;
			driver_function_ptr[i] = 0;
		}
	}
	
};
DeviceDriverTable deviceDriverTable;

// Load device drivers
#include "drivers.h"

// Command console interface
#include "console.h"


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
CommandFunctionTable functionTable;

// Install a function pointer into the function table
uint8_t installModule(void(*function_ptr)(), const char name[], uint8_t name_length) {
	
	// Find a free slot
	uint8_t index;
	for (index=0; index < _COMMAND_TABLE_SIZE__; index++)
	if (functionTable.command_function_table[index] == &nullfunction) break;
	
	// Set the name and pointer
	for (uint8_t i=0; i < name_length-1; i++)
	functionTable.functionNameIndex[index][i] = name[i];
	
	functionTable.command_function_table[index] = function_ptr;
	
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
			
			if ((deviceDriverTable.driver_function_ptr[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20))
				deviceDriverTable.driver_function_ptr[i](_DRIVER_INITIATE__, NULL, NULL, NULL, NULL);
			
		}
		
	}
	
	void shutdown(void) {
		
		// Shutdown device drivers
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			if ((deviceDriverTable.driver_function_ptr[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) 
				deviceDriverTable.driver_function_ptr[i](_DRIVER_SHUTDOWN__, NULL, NULL, NULL, NULL);
			
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
		
		
		default: functionTable.command_function_table[message](); break;
	}
	
	return 0;
}


// Load a device driver entry point function onto the driver function table
uint8_t loadLibrary(const char device_name[], uint8_t name_length, void(*new_driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > 7) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_function_ptr[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length; i++) deviceDriverTable.deviceNameIndex[index][i] = device_name[i];
	deviceDriverTable.driver_function_ptr[index] = new_driver_ptr;
	
	return 1;
}

// Get a library function address by its device name
FunctionPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > 7) return (FunctionPtr&)nullfunction;
	
	// Function look up
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.driver_function_ptr[i] == 0x00) continue;
		
		uint8_t count=0;
		for (count=0; count < name_length; count++)
			if (deviceDriverTable.deviceNameIndex[i][count] == device_name[count]) {continue;} else {count=0; break;}
		
		if (count == name_length) return deviceDriverTable.driver_function_ptr[i];
	}
	
	return (FunctionPtr&)nullfunction;
}

// Call an external function from a library function pointer
uint8_t callExtern(FunctionPtr library_function, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL) {
	
	if (library_function == (FunctionPtr&)nullfunction) return 0;
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.driver_function_ptr[i] == library_function) {
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
	
	displayDriver.writeString(string_memory_allocator_bytes, sizeof(string_memory_allocator_bytes), 0, 8);
	
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


