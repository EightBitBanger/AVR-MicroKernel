//
// AVR kernel core

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

void initiate_kernel(void);

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


void initiate_kernel(void) {
	
	char skip=0;
	char byte=0;
	
	Device console_device;
	Device memory_device;
	Device speaker_device;
	
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), console_device) == 0) return;
	
	uint8_t character = '0';
	uint8_t line      = 0;
	uint8_t pos       = 0;
	
	call_extern(console_device, 0x0a, line, pos);
	call_extern(console_device, 0x00, character);
	
	// Allocate available external memory 0x00000 - 0x3ffff
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memory_device) == 1) {
		
		for (extendedMemoryDriver._STACK_END__=0x00000; extendedMemoryDriver._STACK_END__ < 0x40000; extendedMemoryDriver._STACK_END__++) {
			
			bus_write_byte(extendedMemoryDriver.device_bus, extendedMemoryDriver._STACK_END__, 0x55);
			bus_read_byte(extendedMemoryDriver.device_bus, extendedMemoryDriver._STACK_END__, byte);
			
			if (byte != 0x55) break;
			
			skip++;
			if (skip > 100) {skip=0;
				
				WrappedPointer pointer;
				pointer.address = extendedMemoryDriver._STACK_END__;
				
				call_extern(console_device, 0x0a, line, pos);
				call_extern(console_device, 0x04, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				
			}
			
		}
		
		// Zero kernel memory area
		uint8_t size = 0xff;
		WrappedPointer pointer;
		pointer.address = _KERNEL_BEGIN__;
		
		call_extern(memory_device, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(memory_device, 0x02, size);
		
	}
	// Place the cursor
	line = 1; pos = 0;
	
	call_extern(console_device, 0x0a, line, pos);
	call_extern(console_device, 0x02);
	
	// Speaker test - PASSED
	uint8_t length = 74;
	uint8_t tone   = 1;
	
	if (get_func_address(_INTERNAL_SPEAKER__, sizeof(_INTERNAL_SPEAKER__), speaker_device) == 1) {
		
		call_extern(speaker_device, 0x00, tone, length);
		_delay_ms(350);
		
	}
	
}


#endif


