//
// AVR kernel core

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

//
// Kernel configuration

#define __CORE_MAIN_             // Include the device management system
#define __CORE_SCHEDULER_        // Include the task scheduler
#define __HARDWARE_AUTO_DETECT_  // Include hardware auto detection

//#define __BOOT_SAFEMODE_      // Load only the devices required to boot
//#define __BOOT_LIGHTWEIGHT_   // Load minimal device modules required to boot
//#define __BOOT_NOSERVICES_    // Load only the system services required to boot

#define _32_BIT_POINTERS__
//#define _64_BIT_POINTERS__


//
// Kernel memory map
#define _KERNEL_BEGIN__                    0x00000
#define _KERNEL_END__                      0x007ff

#define _KERNEL_STACK_BEGIN__              0x00100

#define _KERNEL_FUNCTION_TABLE_BEGIN__     0x00000
#define _KERNEL_FUNCTION_TABLE_SIZE__      0x000ff

#define _KERNEL_STACK_COUNTER__            0x00100
#define _KERNEL_FLAGS__                    0x00104


// Kernel State flags
#define _KERNEL_STATE_NORMAL__          0x00
#define _KERNEL_STATE_OUT_OF_MEMORY__   0xa0
#define _KERNEL_STATE_SEG_FAULT__       0xff

// Message constants
char msg_kernel_version[]    = "AVR-Kernel";


// Standard includes
#include "kernel/std/pointers.h"      // Pointer types
#include "kernel/std/cstring.h"       // C string functions

// Kernel systems
#include "kernel/device_table.h"      // Device resource manager
#include "kernel/scheduler.h"         // Task scheduler
#include "kernel/bus.h"               // System bus interface


uint8_t kernel_load_device(uint8_t index, void(*entry_pointer)() );
void    kernel_call_extern(uint8_t index);


// Kernel table initiator
struct __INITIATE_KERNEL_TABLES_ {
	__INITIATE_KERNEL_TABLES_() {
		
		__extern_initiate();
		
		__scheduler_init_();
		
	}
} static __initiate_kernel_tables_;

// Hardware information detection
#include "kernel/hardware_detect.h"

// Load device drivers, function modules and system services
#include "drivers/config.h"
#include "modules/config.h"
#include "services/config.h"




void __kernel_initiate(void) {
	
	char skip=0;
	char byte=0;
	
	Device memory_device  = 0;
	Device speaker_device = 0;
	
	// Beep code
	uint8_t length   = 74;
	uint8_t tone     = 1;
	uint8_t beepcode = 1;
	
	WrappedPointer total_memory;
	Bus device_bus;
	uint8_t test_byte=0x55;
	
	// External bus wait state
	device_bus.waitstate_write = 0;
	device_bus.waitstate_read  = 2;
	
	get_func_address(_INTERNAL_SPEAKER__, sizeof(_INTERNAL_SPEAKER__), speaker_device);
	get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memory_device);
	
	// Initiate the console
	console.initiate();
	
	// Allow peripheral devices time to initiate
	_delay_ms(1000);
	
	//
	// Kernel version information
	console.setCursorPosition(0, 0);
	
	for (uint8_t i=0; i < sizeof(msg_kernel_version)-1; i++)
		console.printChar( msg_kernel_version[i] );
	console.printLn();
	
	// Allocate available external memory
	if (memory_device != nullptr) {
		
		// Zero memory allocated
		console.printChar('0');
		
		// Begin allocating memory
		for (total_memory.address=0x00000; total_memory.address < 0x40000; total_memory.address++) {
			
			bus_write_byte(device_bus, total_memory.address, test_byte);
			bus_read_byte(device_bus, total_memory.address, byte);
			
			if (byte != test_byte) break;
			test_byte++;
			
			// Display total memory
			if (skip > 100) {skip=0;
				// Reset cursor
				console.setCursorPosition(console.cursorLine, 0);
				
				console.printInt(total_memory.address);
				
			} else {skip++;}
		}
		
		// Display final memory count
		console.cursorPos = 0;
		console.setCursorPosition(console.cursorLine, console.cursorPos);
		console.printInt(total_memory.address);
		
		// Message "bytes"
		console.printSpace();
		for (uint8_t i=0; i < sizeof(msg_bytes_free)-1; i++) 
			console.printChar( (uint8_t&)msg_bytes_free[i] );
		
		uint8_t size = 0xff;
		WrappedPointer start_address;
		start_address.address = 0x00000;
		
		// Zero the kernel memory range
		call_extern(memory_device, 0x0a, start_address.byte_t[0], start_address.byte_t[1], start_address.byte_t[2], start_address.byte_t[3]);
		for (uint8_t i=0; i < 10; i++) 
			call_extern(memory_device, 0x02, size);
		
		// Write total memory size to kernel memory
		call_extern(memory_device, 0x0a, total_memory.byte_t[0], total_memory.byte_t[1], total_memory.byte_t[2], total_memory.byte_t[3]);
		call_extern(memory_device, 0x05);
		
	}
	
	// Setup the command prompt
	console.promptString[0] = 'C'; // Scope select
	console.promptString[1] = '>';
	console.promptStringLength = 2;
	
	// Drop a command prompt
	console.printLn();
	console.printPrompt();
	
	
	// Speaker beep code test
	for (uint8_t i=0; i < beepcode; i++) {
		call_extern(speaker_device, 0x00, tone, length);
		_delay_ms(350);
	}
	
	//if (beepcode > 2) while(1) asm("nop");
	
	return;
}



union FunctionPointer {
	
	FunctionPointer() {
		byte[0] = 0x00;
		byte[1] = 0x00;
	}
	
	char    byte[2];
	uint8_t byte_t[2];
	
	void(*pointer)();
	void(*device)();
	
};

// Load a device onto the external function table
uint8_t kernel_load_device(uint8_t index, void(*entry_pointer)() ) {
	
	FunctionPointer function;
	function.device = entry_pointer;
	
	uint32_t function_address = _KERNEL_FUNCTION_TABLE_BEGIN__ + (index * 2);
	
	//extendedMemoryDriver.write(function_address,   function.byte[0]);
	//extendedMemoryDriver.write(function_address+1, function.byte[1]);
	
	return 1;
}

// Call an external function pointer
void kernel_call_extern(uint8_t index) {
	
	FunctionPointer function;
	uint32_t function_address = _KERNEL_FUNCTION_TABLE_BEGIN__ + (index * 2);
	
	//extendedMemoryDriver.read(function_address,   function.byte[0]);
	//extendedMemoryDriver.read(function_address+1, function.byte[1]);
	
	function.device();
	
	return;
}






















#endif


