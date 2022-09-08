//
// Device driver configuration

// Safe mode device drivers
#include "display/console.h"     // Command console library
#include "display/display.h"     // 20x4 display driver
#include "keyboard/PS2.h"        // On-board PS2 keyboard driver
#include "storage/storage.h"     // Basic file system and storage

// Normal mode device drivers
#include "network/network.h"     // Network interface client
//#include "memory/eeprom.h"     // On-chip EEPROM storage
#include "memory/exmem.h"        // On-board extended memory
#include "audio/speaker.h"       // External speaker
#include "gpio/gpio_card.h"      // GPIO card

#ifdef __ARDUINO_UNO_BOARD_

// Arduino boards
#include "arduino/uno.h"         // Arduino UNO device driver

#endif

struct __DriverLoader__ {
	
	__DriverLoader__() {
		
#ifdef __ARDUINO_UNO_BOARD_
		
		load_device("uno", sizeof("uno"), (Module)arduinoUNODeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
#endif
		
#ifdef __AVR_CUSTOM_BOARD_
		
		load_device(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), (Driver)ConsoleLibraryEntryPoint, DEVICE_TYPE_LIBRARY);
		load_device(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), (Driver)DisplayDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), (Driver)keyboardDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_MASS_STORAGE__, sizeof(_MASS_STORAGE__), (Driver)storageDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
#ifndef __BOOT_SAFEMODE_
		
		load_device(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), (Driver)NetworkInterfaceDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), (Driver)ExtendedMemoryDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_INTERNAL_SPEAKER__, sizeof(_INTERNAL_SPEAKER__), (Driver)SpeakerDriverDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		load_device(_GPIO_INTERFACE__, sizeof(_GPIO_INTERFACE__), (Driver)GPIOCardDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
#endif
#endif
		
	}
	
} static __driverLoader__;

