//#define _CLOCK_SPEED_MHZ__  16.0
//#define _CLOCK_SPEED_MHZ__  20.0
#define _CLOCK_SPEED_MHZ__  24.0

#define F_CPU  _CLOCK_SPEED_MHZ__ * 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "kernel\kernel.h"

// Device drivers
#include "drivers\display.h"
#include "drivers\keyboard.h"
#include "drivers\exmem.h"
#include "drivers\console.h"

// System services
#include "services\keyboard_handler.h"

// Command function modules
#include "modules\kill.h"
#include "modules\mem.h"
#include "modules\cls.h"
#include "modules\device.h"
#include "modules\task.h"
#include "modules\app.h"


