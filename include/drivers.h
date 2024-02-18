#ifndef __DEVICE_DRIVER_INITIATION_
#define __DEVICE_DRIVER_INITIATION_

// List of device drivers to load into the kernel

#include <include/drivers/display/LiquidCrystalDisplayController/lcdc.h>



void initiateDeviceDrivers(void) {
    
    
    
    initiateDisplayDriver();
	
	displayDriver.device.hardware_address = 0x40000;
	
	displayDriver.interface.read_waitstate = 10;
	displayDriver.interface.write_waitstate = 10;
	
    
    
}


#endif
