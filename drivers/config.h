// Device driver configuration


// Safe mode device drivers

#include "display/display.h"     // Liquid Crystal display card
#include "keyboard/PS2.h"        // On-board PS2 keyboard

#ifndef __BOOT_SAFEMODE_

// Normal mode device drivers

#include "memory/exmem.h"       // On-board extended memory
#include "memory/eeprom.h"      // On-chip EEPROM storage

#endif

//
// Initiate at the end


#include "display/console.h"    // Command console library

//
// Hardware information detection
#include "hardware_detect.h"

