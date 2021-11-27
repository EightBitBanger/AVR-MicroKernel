//
// AVR Micro Kernel

// Kernel address space
#define _KERNEL_BEGIN__       0x00000
#define _KERNEL_END__         0x000ff

#define _DEVICE_INDEX__       (_KERNEL_BEGIN__ + 0x00000)

#define _KERNEL_FLAGS__       0x00010

#define  _ALLOCATOR_COUNTER_ADDRESS__  0x000fc // Memory address to the number of allocated chunks

// User address space
#define  _STACK_BEGIN__   _KERNEL_END__ + 1  // After kernel memory
uint32_t _STACK_END__  =  0xfffff;           // Total extended memory determined at boot

uint32_t _USER_BEGIN__ =  0x00000;
uint32_t _USER_END__   =  0xfffff;

// Logging
#define _BOOT_LOG__
#define _KERNEL_VERBOSE__

// Extended memory cache size
#define _CACHE_SIZE__  16

// Function tables
#define _DRIVER_TABLE_SIZE__         16  // Total number of elements
#define _COMMAND_TABLE_SIZE__        24
#define _DRIVER_TABLE_NAME_SIZE__    16  // Max name string length
#define _COMMAND_TABLE_NAME_SIZE__   16

// Hardware device address range
#define _DEVICE_ADDRESS_START__  0x40000
#define _DEVICE_ADDRESS_END__    0x80000

// System device names
const char _DISPLAY_CONSOLE__[]   = "display";
const char _KEYBOARD_INPUT__[]    = "keyboard";

#include "enums.h"
#include "flags.h"
#include "types.h"

#include "string_const.h"

#include "stack_allocator.h"

#include "driver_system\driver_table.h"

#include "console.h"

#include "scheduler.h"

// Command line function pointer table
#include "module_system\module_table.h"

struct Kernel {
	
	uint32_t current_address;
	char cache[_CACHE_SIZE__];
	char protectionOverflowBuffer[1];
	
	// External memory cache interface
	char& operator[] (uint32_t new_pointer) {
		
		// Check segmentation fault
		if ((new_pointer < _USER_BEGIN__) || (new_pointer >= _USER_END__)) {
			memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_SEG_FAULT__);
			return protectionOverflowBuffer[0];
		}
		
		// Check cache out of bounds
		if ((new_pointer < current_address) || (new_pointer >= (current_address + _CACHE_SIZE__))) {
			// Dump the cache back to memory
			for (uint8_t i=0; i<_CACHE_SIZE__; i++) {
				memory_write(current_address + i, cache[i]);
				memory_read(new_pointer + i, cache[i]);
			}
			current_address = new_pointer;
		}
		
		return cache[new_pointer - current_address];
	}
	
	Kernel() {
		current_address = _STACK_BEGIN__;
	}
	
	void initiate(void) {
		
		mem_zero(_ALLOCATOR_COUNTER_ADDRESS__, 4); // Number of external memory allocations
		mem_zero(_KERNEL_FLAGS__, 8);              // Kernel state flags
		
		// Initiate device drivers
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			
			if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) {
				
#ifdef _KERNEL_VERBOSE__
				// Display driver names during boot
				console.printChar('d');
				console.printSpace();
				
				for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++) 
					console.printChar(deviceDriverTable.deviceNameIndex[i][a]);
				
				console.printLn();
#endif
				
				deviceDriverTable.driver_entrypoint_table[i](_DRIVER_INITIATE__, NULL, NULL, NULL, NULL);
				
			}
			
			
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
		
		console.printPrompt();
		
		// User memory access
		_USER_BEGIN__ = _KERNEL_BEGIN__ + stack_size();
		_USER_END__   = _STACK_END__;
		
		bool isActive=1;
		while(isActive) {
			
			for (uint8_t index=0; index < _TASK_LIST_SIZE__; index++) {
				
				// Check task
				if (scheduler.taskPriority[index] == 0) continue;
				
				// Increment the task counter
				scheduler.taskCounters[index]++;
				
				// Check if the task should NOT be executed
				if (scheduler.taskCounters[index] < scheduler.taskPriority[index]) continue;
				
				// Reset the counter and call the task
				scheduler.taskCounters[index] = 0;
				scheduler.task_pointer_table[index]();
				
				// Kernel memory access
				_USER_BEGIN__ = _KERNEL_BEGIN__;
				_USER_END__   = _STACK_END__;
				
			}
			
		}
		
		return;
	}
	
	// Allocate available extended memory
	uint32_t allocateSystemMemory(void) {
		uint32_t total_system_memory = 0x00000;
		
		char readByte;
		for (uint32_t address=_STACK_BEGIN__; address < _DEVICE_ADDRESS_START__; address++) {
			
			memory_write(address, 0xff);
			memory_read(address, readByte);
			
			if (readByte != 0xff) break;
			total_system_memory++;
		}
		
		return total_system_memory;
	}
	
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
	
	// Initiate the device handler
	void initiateDeviceIndex(void) {
		
		mem_zero(_DEVICE_INDEX__, 0x0f);
		
		char identityByte=0xff;
		uint32_t address=_DEVICE_ADDRESS_START__;
		for (uint8_t i=0; i <= 0x0f; i++) {
			
			device_read(address, identityByte);
			
			if (identityByte != 0xff)
			memory_write((_DEVICE_INDEX__ + i), identityByte);
			
			if (address >= _DEVICE_ADDRESS_END__) return;
			address += 0x10000;
		}
		
		return;
	}
	
	// Get a device address by its identity byte
	uint32_t getDeviceAddress(uint8_t identity_byte) {
		
		uint32_t address=_DEVICE_ADDRESS_START__;
		
		for (uint8_t i=0; i <= 0x0f; i++) {
			
			char readIdentityByte=0x00;
			memory_read((_DEVICE_INDEX__ + i), readIdentityByte);
			
			if (identity_byte == readIdentityByte) return address;
			
			address += 0x10000;
			if (address > _DEVICE_ADDRESS_END__) break;
		}
			
		return 0x00000;
	}
	
	// Call an external function from a library function pointer
	uint8_t callExtern(EntryPtr& library_function, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
		
		// Check valid pointer
		if (library_function == (EntryPtr&)NULL_f) return 0;
		
		// Find pointer in the driver table
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			
			if (deviceDriverTable.driver_entrypoint_table[i] == library_function) {
				
				// Kernel memory access
				_USER_BEGIN__ = _KERNEL_BEGIN__;
				_USER_END__   = _STACK_END__;
				
				library_function(function_call, paramA, paramB, paramC, paramD);
				
				// User memory access
				_USER_BEGIN__ = _KERNEL_BEGIN__ + stack_size();
				_USER_END__   = _STACK_END__;
				
				return 1;
			}
			
		}
		
		return 1;
	}
	
};
Kernel kernel;

#include "strings.h"

// Load driver libraries
#include "driver_system\drivers.h"

// Load command line function modules
#include "module_system\modules.h"

#include "services\services.h"


// Load a device driver entry point function onto the driver function table
uint8_t loadLibrary(const char name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__-1) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length-1; i++)
		deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = driver_ptr;
	
	return 1;
}

// Get a library function address by its device name
EntryPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return (EntryPtr&)NULL_f;
	
	// Function look up
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
			
			if (count >= name_length) return deviceDriverTable.driver_entrypoint_table[i];
			
		}
		
	}
	
	return (EntryPtr&)NULL_f;
}

// Call an external function from a library function pointer
uint8_t callExtern(EntryPtr& library_function, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	 return kernel.callExtern(library_function, function_call, paramA, paramB, paramC, paramD);
}


