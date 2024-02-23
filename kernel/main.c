#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>





int main(void) {
    
    // Zero the system bus
    bus_address_zero();
    bus_control_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1000);
	
	
	// Initiate drivers here
	
	initiateDisplayDriver();
	
	
	
	
	// Initiate the kernel
	
	InitiateDeviceTable();
    
    
    
	
	
	// Request the display driver from the kernel
	char nameString[] = "display";
	
	struct Driver* displayDriver = (struct Driver*)GetDriverByName( nameString, sizeof(nameString) );
	
	
	// Print off names of attached peripheral devices
	
	displayDriver->write( SET_CURSOR_BLINK_RATE, 0x00 );
	
	for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        struct Device* device = GetHardwareDeviceByIndex(d);
        
        if (is_letter(&device->device_name[1]) == 0) 
            continue;
        
        for (uint8_t i=0; i < DEVICE_NAME_LENGTH - 1; i++) 
            displayDriver->write( i + (d * 20), device->device_name[1 + i] );
        
    }
    
	_delay_ms( 800 );
    
    displayDriver->write( CLEAR_FRAME_BUFFER, 0x01 );
	
	_delay_ms( 300 );
    
	
    
    //bus_address_zero();
    bus_control_zero();
    
    
    
    PORTD = 0x00;
    
    //PORTD |= (1 << PD4);  // Red
    PORTD |= (1 << PD5);  // Green
    
    _delay_ms(100);
    
    
    
    while(1) {
        
        
        continue;
    }
    
    return 0;
}
