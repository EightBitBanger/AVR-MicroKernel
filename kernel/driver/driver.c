#include <kernel/driver/driver.h>

void* driver_registry[DEVICE_REGISTRY_SIZE];

// Number of drivers registered in the kernel
uint8_t number_of_drivers = 0;


struct Driver* GetDriverByName(char* nameString, uint8_t stringSize) {
	
	for (uint8_t d=0; d < number_of_drivers; d++) {
        
        struct Driver* driverPtr = GetDriverByIndex(d);
        
        uint8_t isWrongName = 0;
        
        for (uint8_t i=0; i < stringSize - 1; i++) {
            
            if (driverPtr->device.device_name[i] != nameString[i]) {
                isWrongName = 1;
                break;
            }
            
        }
        
        if (isWrongName == 0) 
            return driverPtr;
        
        continue;
	}
	
	return nullptr;
}

struct Driver* GetDriverByIndex(uint8_t index) {
    return driver_registry[ index ];
}


uint8_t RegisterDriver(void* deviceDriverPtr) {
    
    uint8_t current_number_of_drivers = number_of_drivers;
    
    driver_registry[ number_of_drivers ] = (void*)deviceDriverPtr;
    
    number_of_drivers++;
    
    return current_number_of_drivers;
}

uint8_t GetNumberOfDrivers(void) {
    return number_of_drivers;
}
