//
// AVR kernel core

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

#include "kernel/config.h"

// Standard includes
#include "kernel/std/pointers.h"      // Pointer types
#include "kernel/std/string_const.h"  // System message strings
#include "kernel/std/cstring.h"       // C string functions

// Kernel function tables
#include "kernel/driver_table.h"      // Device drivers
#include "kernel/module_table.h"      // Function modules
#include "kernel/bus.h"               // System bus interface
#include "kernel/scheduler.h"         // Task scheduler


// Kernel function table
uint8_t kernel_load_module(uint8_t index, Module entry_pointer);
void    kernel_call_extern(uint8_t index);


// Kernel table initiator
struct __INITIATE_KERNEL_TABLES_ {
	__INITIATE_KERNEL_TABLES_() {
		
		__extern_initiate();  // Initiate device driver system
		
		__module_init_();     // Initiate function module system
		
		__scheduler_init_();  // Initiate scheduler
		
	}
} static __initiate_kernel_tables_;

// Load device drivers, function modules and system services
#include "drivers/config.h"
#include "modules/config.h"
#include "services/config.h"

// Hardware information detection
#include "drivers/hardware_detect.h"


union FunctionPointer {
	
	FunctionPointer() {
		byte[0] = 0x00;
		byte[1] = 0x00;
	}
	
	void*(pointer)();
	
	char    byte[2];
	uint8_t byte_t[2];
	
	void(*device)();
	
};




void __kernel_initiate(void) {
	
	char skip=0;
	char byte=0;
	
	Device console_device;
	Device memory_device;
	Device speaker_device;
	
	
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), console_device) == 0) return;
	
	uint8_t character = '0';
	uint8_t line      = 0;
	uint8_t pos       = 0;
	
	// Initiate the cursor
	call_extern(console_device, 0x0a, line, pos);
	call_extern(console_device, 0x00, character);
	
	WrappedPointer total_memory;
	Bus device_bus;
	
	// External bus wait states
	device_bus.waitstate_write = 0;
	device_bus.waitstate_read  = 2;
	
	// Allocate available external memory
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memory_device) == 1) {
		
		for (total_memory.address=0x00000; total_memory.address < 0x80000; total_memory.address++) {
			
			bus_write_byte(device_bus, total_memory.address, 0x55);
			bus_read_byte(device_bus, total_memory.address, byte);
			
			if (byte != 0x55) break;
			
			skip++;
			if (skip > 100) {skip=0;
				
				call_extern(console_device, 0x0a, line, pos);
				call_extern(console_device, 0x04, total_memory.byte_t[0], total_memory.byte_t[1], total_memory.byte_t[2], total_memory.byte_t[3]);
				
			}
			
		}
		
		// Zero kernel memory area
		uint8_t size = 0xff;
		for (uint8_t i=0; i < 10; i++) 
			call_extern(memory_device, 0x02, size);
		
		// Write memory total size to kernel memory
		call_extern(memory_device, 0x0a, total_memory.byte_t[0], total_memory.byte_t[1], total_memory.byte_t[2], total_memory.byte_t[3]);
		call_extern(memory_device, 0x05);
		
	} else {
		
		// ERROR - memory not installed
		for (uint8_t i=0; i<sizeof(error_exmem_not_installed); i++) 
			call_extern(console_device, 0x00, (uint8_t&)error_exmem_not_installed[i]);
		call_extern(console_device, 0x01);
		
		// Error beep code 
		uint8_t length   = 74;
		uint8_t tone     = 1;
		uint8_t beepcode = 5;
		
		if (get_func_address(_INTERNAL_SPEAKER__, sizeof(_INTERNAL_SPEAKER__), speaker_device) == 1) {
			
			for (uint8_t i=0; i<beepcode; i++) {
				call_extern(speaker_device, 0x00, tone, length);
				_delay_ms(350);
			}
			
		}
		
		while(1) asm("nop");
		
	}
	
	// Place the cursor
	line = 1; pos = 0;
	
	call_extern(console_device, 0x0a, line, pos);
	call_extern(console_device, 0x02);
	
	// Speaker test - PASSED \ single beep
	uint8_t length = 74;
	uint8_t tone   = 1;
	
	if (get_func_address(_INTERNAL_SPEAKER__, sizeof(_INTERNAL_SPEAKER__), speaker_device) == 1) {
		
		call_extern(speaker_device, 0x00, tone, length);
		_delay_ms(350);
		
	}
	
}


// Load a kernel module onto the function table
uint8_t kernel_load_module(uint8_t index, void(*entry_pointer)() ) {
	
	FunctionPointer function;
	function.device = entry_pointer;
	
	uint32_t function_address = _KERNEL_FUNCTION_TABLE__ + (index * 2);
	
	extendedMemoryDriver.write(function_address,   function.byte[0]);
	extendedMemoryDriver.write(function_address+1, function.byte[1]);
	
	return 1;
}

// Call an external function pointer
void kernel_call_extern(uint8_t index) {
	
	FunctionPointer function;
	uint32_t function_address = _KERNEL_FUNCTION_TABLE__ + (index * 2);
	
	extendedMemoryDriver.read(function_address,   function.byte[0]);
	extendedMemoryDriver.read(function_address+1, function.byte[1]);
	
	function.device();
	
	return;
}























#endif


