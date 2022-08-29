//
// Device driver configuration

#ifndef __ARDUINO_BOARD_

// Safe mode device drivers
#include "display/display.h"     // 20x4 display driver
#include "display/console.h"     // Command console library
#include "keyboard/PS2.h"        // On-board PS2 keyboard driver
#include "storage/storage.h"     // Mass storage driver


#ifndef __BOOT_SAFEMODE_

// Normal mode device drivers
//#include "network/network.h"     // Network interface client
//#include "memory/eeprom.h"       // On-chip EEPROM storage
//#include "memory/exmem.h"        // On-board extended memory
//#include "audio/speaker.h"       // External speaker
//#include "gpio/gpio_card.h"      // GPIO card

#endif

#endif



#ifdef __ARDUINO_BOARD_

// Arduino boards
#include "arduino/uno.h"         // Arduino UNO device driver


#endif


