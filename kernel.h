
// AVR kernel core configuration

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

#define __CORE_MAIN_                           // Include core kernel components
#define __CORE_SCHEDULER_                      // Include the task scheduler
#define __HARDWARE_AUTO_DETECT_                // Use hardware auto detection

//#define __BOOT_SAFEMODE_                       // Load only the minimal drivers required to boot
//#define __BOOT_LIGHTWEIGHT_                    // Dont load the command modules
#define __BOOT_FS_SUPPORT_                     // Include file system support console functions
#define __BOOT_NETWORK_SUPPORT_                // Include network support console functions
#define __BOOT_SYSTEM_FUNCTIONS_               // Include system support console functions
//#define __BOOT_ROUTER_PROGRAM_                 // Run the system as a packet router

#define _USE_VIRTUAL_STORAGE__                 // Enable and format a section of memory for storage
#define _VIRTUAL_STORAGE_ADDRESS__  0x10000    // Storage location in memory
#define _VIRTUAL_STORAGE_SIZE__     1024 * 2   // Total size of the memory storage


//#define __ARDUINO_BOARD_                       // Compile for an Arduino UNO board
#define __AVR_CUSTOM_BOARD_                    // Compile for a custom AVR board
//#define _REV_1_ADDRESS_MAP__


#define _32_BIT_POINTERS__
//#define _64_BIT_POINTERS__


// Kernel memory map
#define _KERNEL_BEGIN__                 0x00000
#define _KERNEL_END__                   0x000ff
#define _KERNEL_STACK_BEGIN__           0x00100
#define _KERNEL_STACK_COUNTER__         0x00000
#define _KERNEL_FLAGS__                 0x00004


// Kernel States
#define _KERNEL_STATE_NORMAL__          0x00
#define _KERNEL_STATE_OUT_OF_MEMORY__   0xa0
#define _KERNEL_STATE_SEG_FAULT__       0xff

// Version info
char msg_kernel_boot[]      = "AVR-Kernel";
char msg_kernel_version[]   = "1.0";

// Standard includes
#include "kernel/std/pointers.h"      // Pointer types
#include "kernel/std/cstring.h"       // C string functions

// Kernel systems
#include "kernel/device_table.h"      // Device resource manager
#include "kernel/scheduler.h"         // Task scheduler
#include "kernel/bus.h"               // System bus interface

// Hardware information detection
#include "kernel/hardware_detection.h"

// Load device drivers, function modules and system services
#include "drivers/config.h"
#include "modules/config.h"
#include "services/config.h"


void __kernel_initiate(void) {	
#ifdef __AVR_CUSTOM_BOARD_
	
	// Initiate the scheduler
	__scheduler_init_();
	
	char skip=0;
	char byte=0;
	
	Device memory_device;
	Device speaker_device;
	Device console_device;
	
	// External device bus wait state
	Bus device_bus;
	device_bus.waitstate_write = 0;
	device_bus.waitstate_read  = 2;
	
	// Initiate the console
	console_device = (Device)get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__));
	if (console_device == 0) 
		return;
	
	call_extern(console_device, DEVICE_CALL_INITIATE);
	
	uint8_t pos=0;
	uint8_t line=0;
	call_extern(console_device, 0x0a, line, pos);
	
	// Print kernel boot message
	for (uint8_t i=0; i < sizeof(msg_kernel_boot)-1; i++)
		call_extern(console_device, 0x00, (uint8_t&)msg_kernel_boot[i]);
	call_extern(console_device, 0x03); // Space
	
	// Print version information
	for (uint8_t i=0; i < sizeof(msg_kernel_version)-1; i++)
		call_extern(console_device, 0x00, (uint8_t&)msg_kernel_version[i]);
	call_extern(console_device, 0x01); // New line
	
	// Setup the command prompt
	uint8_t prompt_string[5] = "C>  ";
	uint8_t prompt_length = 0x02;
	call_extern(console_device, 0x0e, prompt_length);
	call_extern(console_device, 0x0f, prompt_string[0], prompt_string[1], prompt_string[2], prompt_string[3]);
	
	//
	// Allocate available external memory
	memory_device = (Device)get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__));
	if (memory_device != 0) {
		
		WrappedPointer total_memory;
		uint8_t test_byte = 0x55;
		
		for (total_memory.address=0x00000; total_memory.address <= 0x3ffff; total_memory.address++) {
			
			bus_write_byte(device_bus, total_memory.address, test_byte);
			bus_read_byte(device_bus, total_memory.address, byte);
			
			if (byte != test_byte) 
				break;
			
			test_byte++;
			
			// Print total memory
			if (skip > 245) {skip=0;
				
				// Reset cursor
				pos=0;
				line=1;
				call_extern(console_device, 0x0a, line, pos);
				
				call_extern(console_device, 0x04, total_memory.byte_t[0], total_memory.byte_t[1], total_memory.byte_t[2], total_memory.byte_t[3]);
				
			} else {skip++;}
		}
		
		if (total_memory.address > 0) {
			
			// Print final memory count
			line=1;
			pos=0;
			call_extern(console_device, 0x0a, line, pos);
			call_extern(console_device, 0x04, total_memory.byte_t[0], total_memory.byte_t[1], total_memory.byte_t[2], total_memory.byte_t[3]);
			
			// Print "bytes free"
			call_extern(console_device, 0x03); // Space
			char bytesfreemsg[] = "bytes free";
			for (uint8_t i=0; i < sizeof(bytesfreemsg)-1; i++) 
				call_extern(console_device, 0x00, (uint8_t&)bytesfreemsg[i]);
			
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
			
			call_extern(console_device, 0x01); // New line
		}
		
	}
	
#ifdef __BOOT_ROUTER_PROGRAM_
	router_entry_point();
#endif
	
#ifdef _USE_VIRTUAL_STORAGE__
	// Initiate file system
	fs_device_format(_VIRTUAL_STORAGE_ADDRESS__, _VIRTUAL_STORAGE_SIZE__);
	
	console.promptString[0] = '/';
	fs_set_device_scope();
	
	char file_data[] = {'H','H','H','H',};
	
	char filename[] = "io ";
	uint8_t fileattr[] = "xrw ";
	file_create(filename, sizeof(file_data), fileattr);
	
	if (file_open(filename) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		while(1){}
	}
	
	for (uint16_t i=0; i<sizeof(file_data); i++) 
		file_write_byte(i, file_data[i]);
	
	
#endif
	
	// Launch the command console
	task_create("console", sizeof("console"), keyboard_event_handler, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
	
#endif
	
	// Drop a command prompt
	call_extern(console_device, 0x02);
	
	return;
}


#endif


