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
uint32_t _STACK_END__  =  0;                 // Total extended memory determined at boot

uint32_t _USER_BEGIN__ =  0;
uint32_t _USER_END__   =  0;

// Extended memory cache size
#define _CACHE_SIZE__  16

// Counters
#define _KERNEL_TIMEOUT__    1000
#define _KEYBOARD_TIMEOUT__  20000

// Function tables
#define _DRIVER_TABLE_SIZE__         16
#define _DRIVER_TABLE_NAME_SIZE__    10
#define _COMMAND_TABLE_SIZE__        24
#define _COMMAND_TABLE_NAME_SIZE__   10


// Hardware device address range
#define _DEVICE_ADDRESS_START__  0x40000
#define _DEVICE_ADDRESS_END__    0x80000

// Device names
const char _DISPLAY_CONSOLE__[]   = "display";
const char _KEYBOARD_INPUT__[]    = "keyboard";

// Address pointer wrapper
union Pointer {
	
#ifdef _32_BIT_POINTERS__
	char byte[4];
	uint16_t word[2];
	uint32_t dword;
	uint32_t address;
#endif
#ifdef _64_BIT_POINTERS__
	char byte[8];
	uint16_t word[4];
	uint32_t dword[2];
	uint64_t address;
#endif
	
	Pointer() {address=0;}
	
#ifdef _32_BIT_POINTERS__
	Pointer(uint32_t newAddress) {address=newAddress;}
#endif
#ifdef _64_BIT_POINTERS__
	Pointer(uint64_t newAddress) {address=newAddress;}
#endif
	
	// Write the pointer to a memory location
	//void write(uint32_t address) {for (uint8_t i=0; i<4; i++) memory_write(address+i, byte[i]);}
	
	// Read the pointer from a memory location
	//void read(uint32_t address) {for (uint8_t i=0; i<4; i++) memory_read(address+i, byte[i]);}
	
};

#include "enums.h"
#include "types.h"

#include "string_const.h"

// Keyboard string update
void updateKeyboard(void);

// Default message callback procedure
uint8_t defaultCallbackProcedure(uint8_t message);


// Device driver entry pointer table
#include "driver_table.h"

#include "console.h"

// Command line function pointer table
#include "module_table.h"


struct Kernel {
	
	// External memory cache
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
		console.printPrompt();
		
		bool isActive=1;
		while(isActive) {
			
			kernelCounter++;
			if (kernelCounter > kernelTimeOut) {kernelCounter=0;
				
			}
			
			keyboardCounter++;
			if (keyboardCounter > keyboardTimeOut) {
				
				keyboardCounter=0; updateKeyboard();
			}
			
		}
		
		return;
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
	
	void updateMemoryAccess(void) {
		
		_USER_BEGIN__ = stack_size();
		_USER_END__   = _STACK_END__;
		
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
	
	
	
	// Allocate stack memory and return a pointer to the first byte of the allocation
	uint32_t stack_alloc(uint32_t size) {
		
		Pointer numberOfAllocations;
		for (uint8_t i=0; i<4; i++) memory_read(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
		
		//numberOfAllocations.read(_ALLOCATOR_COUNTER_ADDRESS__);
		
		// Check memory segmentation
		if (_STACK_BEGIN__ + (numberOfAllocations.address + size) >= _USER_END__) {
			memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_OUT_OF_MEMORY__);
			return nullptr;
		}
		
		// Get the beginning address pointing to the new allocation
		uint32_t new_pointer = _STACK_BEGIN__ + (numberOfAllocations.address);
		
		numberOfAllocations.address += size;
		for (uint8_t i=0; i<4; i++) memory_write(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
		
		//numberOfAllocations.write(_ALLOCATOR_COUNTER_ADDRESS__);
		
		return new_pointer;
	}
	
	// Free allocated memory of the given size
	void stack_free(uint32_t size) {
		
		Pointer numberOfAllocations;
		for (uint8_t i=0; i<4; i++) memory_read(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
		
		numberOfAllocations.address -= size;
		
		for (uint8_t i=0; i<4; i++) memory_write(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
		
		return;
	}
	
	// Zero a range of memory
	void mem_zero(uint32_t address_pointer, uint32_t size) {
		for (uint32_t i=0; i<size; i++) {memory_write(address_pointer+i, 0x00);}
	}
	
	// Return the number of bytes allocated in the stack
	uint32_t stack_size(void) {
		
		Pointer numberOfAllocations;
		for (uint8_t i=0; i<4; i++) memory_read(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
		
		return numberOfAllocations.address;
	}
	
	void memory_read(uint32_t address, char& buffer) {
		
		// Address the device
		_BUS_LOWER_DIR__ = 0xff;
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_LOWER_OUT__ = (address & 0xff);
		_BUS_MIDDLE_OUT__ = (address >> 8);
		_BUS_UPPER_OUT__ = (address >> 16);
		_CONTROL_OUT__ = 0b00110100;
		
		// Set data direction
		_BUS_LOWER_DIR__ = 0x00; // Data bus input
		_BUS_LOWER_OUT__ = 0xff; // Pull-up resistors
		
		_CONTROL_OUT__ = 0b00100100;
		
		// Wait states
#ifdef _MEMORY_READ_WAIT_STATE_1__
		nop;
#endif
#ifdef _MEMORY_READ_WAIT_STATE_2__
		nop; nop;
#endif
#ifdef _MEMORY_READ_WAIT_STATE_3__
		nop; nop; nop;
#endif
		
		// Read the data byte
		buffer = _BUS_LOWER_IN__;
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	void memory_write(uint32_t address, char byte) {
		
		// Address the device
		_BUS_LOWER_DIR__ = 0xff;
		
		_CONTROL_OUT__ = 0b00111111;
		//_CONTROL_OUT__ |= (1 << _CONTROL_ADDRESS_LATCH__);
		_BUS_LOWER_OUT__ = (address & 0xff);
		_BUS_MIDDLE_OUT__ = (address >> 8);
		_BUS_UPPER_OUT__ = (address >> 16);
		//_CONTROL_OUT__ &= ~(1 << _CONTROL_ADDRESS_LATCH__);
		_CONTROL_OUT__ = 0b00110101;
		
		// Cast the data byte
		_BUS_LOWER_OUT__ = byte;
		
		_CONTROL_OUT__ = 0b00010101;
		
		// Wait states
#ifdef _MEMORY_WRITE_WAIT_STATE_1__
		nop;
#endif
#ifdef _MEMORY_WRITE_WAIT_STATE_2__
		nop; nop;
#endif
#ifdef _MEMORY_WRITE_WAIT_STATE_3__
		nop; nop; nop;
#endif
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	
	void device_read(uint32_t address, char& byte) {
		
		// Check segmentation fault
		if ((address < _DEVICE_ADDRESS_START__) || (address >= _DEVICE_ADDRESS_END__)) 
			return;
		
		// Address the device
		_BUS_LOWER_DIR__ = 0xff;
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_LOWER_OUT__ = (address & 0xff);
		_BUS_MIDDLE_OUT__ = (address >> 8);
		_BUS_UPPER_OUT__ = (address >> 16);
		_CONTROL_OUT__ = 0b00110100;
		
		// Set data direction
		_BUS_LOWER_DIR__ = 0x00; // Data bus input
		_BUS_LOWER_OUT__ = 0xff; // Pull-up resistors
		
		// Read operation
		_CONTROL_OUT__ = 0b00100100;
		
		// Wait states
#ifdef _DEVICE_READ_WAIT_STATE_1__
		for (uint8_t i=0; i<10; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_READ_WAIT_STATE_2__
		for (uint8_t i=0; i<50; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_READ_WAIT_STATE_3__
		for (uint8_t i=0; i<100; i++) {nop;nop;nop;nop;}
#endif
		
		byte = _BUS_LOWER_IN__;
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	void device_write(uint32_t address, char byte) {
		
		// Check segmentation fault
		if ((address < _DEVICE_ADDRESS_START__) || (address >= _DEVICE_ADDRESS_END__))
		return;
		
		// Address the device
		_BUS_LOWER_DIR__ = 0xff;
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_LOWER_OUT__ = (address & 0xff);
		_BUS_MIDDLE_OUT__ = (address >> 8);
		_BUS_UPPER_OUT__ = (address >> 16);
		_CONTROL_OUT__ = 0b00110101;
		
		_BUS_LOWER_OUT__ = byte;
		
		// Write operation
		_CONTROL_OUT__ = 0b00010101;
		
		// Wait states
#ifdef _DEVICE_WRITE_WAIT_STATE_1__
		for (uint8_t i=0; i<10; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_WRITE_WAIT_STATE_2__
		for (uint8_t i=0; i<50; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_WRITE_WAIT_STATE_3__
		for (uint8_t i=0; i<100; i++) {nop;nop;nop;nop;}
#endif
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	
	private:
	uint8_t memoryProtectionLevel;
	
};
Kernel kernel;





#include "strings.h"

// Load driver libraries
#include "drivers.h"
// Load command modules
#include "modules.h"

// Total system memory allocator
#include "memory_check.h"

// Keyboard event handling
#include "keyboard_events.h"

uint8_t defaultCallbackProcedure(uint8_t message) {
	
	switch (message) {
		
		
		default: moduleTable.command_function_table[message](); break;
	}
	
	return 0;
}



