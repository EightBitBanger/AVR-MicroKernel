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

uint32_t _USER_BEGIN__ =  0;
uint32_t _USER_END__   =  0;

// Extended memory cache size
#define _CACHE_SIZE__  16

// Counters
#define _KERNEL_TIMEOUT__    16
#define _KEYBOARD_TIMEOUT__  20000

// Function tables
#define _DRIVER_TABLE_SIZE__         10
#define _DRIVER_TABLE_NAME_SIZE__    10
#define _COMMAND_TABLE_SIZE__        32
#define _COMMAND_TABLE_NAME_SIZE__   10

// Hardware device address range
#define _DEVICE_ADDRESS_START__  0x40000
#define _DEVICE_ADDRESS_END__    0x80000

// Device names
const char _DISPLAY_CONSOLE__[]   = "display";
const char _KEYBOARD_INPUT__[]    = "keyboard";

#include "enums.h"
#include "flags.h"
#include "types.h"

#include "string_const.h"

#include "stack_allocator.h"

void keyboard_event_handler(void);

// Task scheduler system
#include "scheduler.h"

// Device driver entry pointer table
#include "driver_table.h"

// Command console
#include "console.h"

// Command line function pointer table
#include "module_table.h"

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
		
		accessModeKernel();
		
		console.printPrompt();
		
		bool isActive=1;
		while(isActive) {
			
			kernelCounter++;
			if (kernelCounter > kernelTimeOut) {kernelCounter=0;
				accessModeUser();
				scheduler.processTasks();
				accessModeKernel();
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
	
	void accessModeUser(void) {
		_USER_BEGIN__ = _STACK_BEGIN__;
		_USER_END__   = _STACK_END__;
	}
	
	void accessModeKernel(void) {
		_USER_BEGIN__ = _KERNEL_BEGIN__;
		_USER_END__   = _STACK_END__;
	}
	
};
Kernel kernel;

#include "strings.h"

// Load driver libraries
#include "drivers.h"

// Load command line function modules
#include "modules.h"

// Keyboard console event handling
#include "keyboard_events.h"


