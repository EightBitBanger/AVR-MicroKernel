//
// Kernel function modules

#ifndef __ARDUINO_BOARD_

#ifndef __BOOT_LIGHTWEIGHT_

// File system functions
#include "fs/dir.h"             // List files
#include "fs/mk.h"              // Make a new file
#include "fs/rm.h"              // Remove a file
#include "fs/cd.h"              // Change directory
#include "fs/copy.h"            // Copy a file to new file
#include "fs/format.h"          // Device format

#include "system/task.h"        // Task management
#include "system/port.h"        // GPIO port configuration
#include "system/list.h"        // Data listing
#include "system/cls.h"         // Clear the display
#include "system/device.h"      // Device table configuration
#include "system/mem.h"         // Memory check
#include "system/net.h"         // Network testing and configuration


// === TESTING ===
// Applications
//#include "functions/app.h"      // Test application

#endif

#endif

