//
// Device driver configuration

// Safe mode device drivers

#include "display/display.h"     // Liquid Crystal display card
#include "keyboard/keyboard.h"    // On-board keyboard
#include "display/console.h"     // Command console library

#ifndef __BOOT_SAFEMODE_


// Normal mode device drivers

#include "memory/exmem.h"       // On-board extended memory
#include "memory/eeprom.h"      // On-chip EEPROM storage

#endif



