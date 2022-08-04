//
// Kernel function modules

#include "functions/cls.h"      // Clear the display
#include "functions/device.h"   // Device table configuration

#ifndef __BOOT_LIGHTWEIGHT_

// Testing
#include "functions/mem.h"      // Memory check
#include "functions/net.h"      // Network testing and configuration
// Functionality
#include "functions/task.h"     // Task management
#include "functions/port.h"     // GPIO port configuration
// Applications
#include "functions/app.h"      // Test application

#endif

