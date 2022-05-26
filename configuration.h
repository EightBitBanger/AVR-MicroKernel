//
// Device driver configuration


//
// Safe mode device drivers

#include "drivers\display.h"     // Liquid Crystal display controller
#include "drivers\keyboard.h"    // On-board keyboard
#include "drivers\console.h"     // Command console library


//
// Normal mode device drivers

#ifndef __BOOT_SAFEMODE_

#include "drivers\exmem.h"       // On-board extended memory
#include "drivers\eeprom.h"      // On-chip EEPROM storage

#endif

#ifndef __BOOT_LIGHTWEIGHT_

//
// Kernel function modules

#include "modules\app.h"
#include "modules\cls.h"
#include "modules\device.h"
#include "modules\format.h"
#include "modules\kill.h"
#include "modules\list.h"
#include "modules\mem.h"
#include "modules\task.h"

//
// System services

#include "services\command_console.h"

#endif

