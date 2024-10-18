#include <kernel/drivers.h>

void initBakedDrivers(void) {
    
    initiateDisplayDriver();      // Display
	initiatePS2Driver();          // PS/2 Port
	
    return;
}
