//
// Kernel function modules

#ifndef __ARDUINO_BOARD_

#include "functions/cls.h"      // Clear the display
#include "functions/device.h"   // Device table configuration

#ifndef __BOOT_LIGHTWEIGHT_

// Testing
#include "functions/mem.h"      // Memory check
#include "functions/net.h"      // Network testing and configuration
#include "functions/format.h"   // Storage format
// Functionality
#include "functions/task.h"     // Task management
#include "functions/port.h"     // GPIO port configuration
#include "functions/list.h"     // Data listing
// Applications
#include "functions/app.h"      // Test application

#endif

#endif

