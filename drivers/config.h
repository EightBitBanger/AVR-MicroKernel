//
// Device driver configuration

// Safe mode device drivers
#include "display/console.h"       // Command console library
#include "display/display.h"       // 20x4 display driver
#include "keyboard/PS2.h"          // On-board PS2 keyboard driver
#include "fs/fs.h"                 // File system

// Normal mode device drivers
#include "network/network.h"       // Network interface client
#include "memory/exmem.h"          // On-board extended memory

// Emulation
#include "emulation/emulation.h"   // Software emulation

#ifdef __ARDUINO_UNO_BOARD_

// Arduino boards
#include "arduino/uno.h"           // Arduino UNO device driver

#endif

struct __DriverLoader__ {
	
	__DriverLoader__() {
		
		__extern_initiate();
		
#ifdef __ARDUINO_UNO_BOARD_
		
		load_device("uno", sizeof("uno"), (Driver)arduinoUNODeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
#endif
		
#ifdef __BOOT_FS_SUPPORT_
		load_device(_MASS_STORAGE__, sizeof(_MASS_STORAGE__), (Driver)storageDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
#endif
		
#ifndef __BOOT_SAFEMODE_
		
#ifdef __BOOT_NETWORK_SUPPORT_
		load_device(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), (Driver)NetworkInterfaceDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
#endif
		
		load_device(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), (Driver)ConsoleLibraryEntryPoint, DEVICE_TYPE_LIBRARY);
		load_device(_DISPLAY_DEVICE__,  sizeof(_DISPLAY_DEVICE__),  (Driver)DisplayDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_KEYBOARD_INPUT__,  sizeof(_KEYBOARD_INPUT__),  (Driver)keyboardDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), (Driver)ExtendedMemoryDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
#endif
		
	}
	
} static __driverLoader__;

