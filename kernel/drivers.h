#ifndef _DRIVER_INITIATION__
#define _DRIVER_INITIATION__

//
// Defined baked drivers to be compiled into
// the kernel and available by default.

#include <avr/io.h>

#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>

void initBakedDrivers(void);

#endif
