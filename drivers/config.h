// Device driver configuration


// Safe mode device drivers

#include "display/display.h"     // Liquid Crystal display card
#include "keyboard/PS2.h"        // On-board PS2 keyboard

// Normal mode device drivers
#ifndef __BOOT_SAFEMODE_

#include "memory/exmem.h"        // On-board extended memory
#include "memory/eeprom.h"       // On-chip EEPROM storage
#include "gpio/gpio_card.h"      // GPIO card rev 1

#endif

//
// Initiate at the end

#include "audio/speaker.h"       // On-board speaker
#include "display/console.h"     // Command console library

//
// Hardware information detection
#include "hardware_detect.h"

