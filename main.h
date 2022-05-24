#define _CLOCK_SPEED_MHZ__  24.0
#define F_CPU  _CLOCK_SPEED_MHZ__ * 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "kernel.h"

// Device drivers
#ifndef __BOOT_SAFEMODE_
  
  //
  // Device drivers
  
  #include "drivers\display.h"     // Liquid Crystal display controller
  #include "drivers\keyboard.h"    // On-board keyboard
  #include "drivers\exmem.h"       // On-board extended memory
  #include "drivers\eeprom.h"      // On-chip EEPROM storage
  #include "drivers\console.h"     // Command console library
  
#else
  
  //
  // Safe mode device drivers
  
  #include "drivers\display.h"     // Liquid Crystal display controller
  #include "drivers\keyboard.h"    // On-board keyboard
  #include "drivers\console.h"     // Command console library
  
#endif

#ifndef __CORE_LIGHTWEIGHT_
  
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
  
#else
  
  //
  // Lightweight kernel function modules
  
  //#include "modules\device.h"
  //#include "modules\kill.h"
  //#include "modules\list.h"
  //#include "modules\mem.h"
  //#include "modules\task.h"
  
#endif


