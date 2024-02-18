#include <avr/io.h>
#include <util/delay.h>

#include <include/kernel/bus.h>
#include <include/kernel/device.h>
#include <include/kernel/kernel.h>

#include <include/drivers.h>


void Print(struct Device* driver, char* string, unsigned int size) {
    
    unsigned int numberOfChars = size - 1;
    
	//for (unsigned int i=0; i < numberOfChars; i++) 
    //    bus_write_byte( &driver->interface, 0x40000 + i, string[i] );
    
    return;
}



int main(void) {
    
    bus_control_zero();
    bus_address_zero();
    
    //kernel_initiate();
    
    // Allow board some time to stabilize
    _delay_ms(1000);
	
	initiateDeviceDrivers();
	
	
	
	
	
	
	
	// Test draw a string
	//struct Bus bus;
	
	//bus.read_waitstate  = 10;
	//bus.write_waitstate = 10;
	
	//char string[] = "          ";
	
	//for (unsigned int i=0; i < 8; i++) 
    //    bus_read_byte(&bus, 0x40000 + i, &string[i]);
	
	
	
	
	//for (unsigned int i=0; i < 10; i++) 
    //    displayDriver.write( i, device_table[0].device_name[i] );
	
	
	
	
	
	
	
	
	
    //device_write(&display, 0xa0, 0);     // Set cursor line
	//device_write(&display, 0xa1, 0);     // Set cursor position
	//device_write(&display, 0xa2, 'X');   // Set cursor char
    //device_write(&display, 0xa3, 40);    // Set blink rate
    //device_write(&display, 0xa4, 40);    // Set refresh rate
    //device_write(&display, 0xa5, 0x01);  // Clear frame buffer
    //device_write(&display, 0xa6, 0x01);  // Clear mask buffer
    //device_write(&display, 0xa7, 0);     // Clear a line
    //device_write(&display, 0xa8, 0x01);  // Shift the frame up
    //device_write(&display, 0xa9, 0x01);  // Shift the frame down
    
	_delay_ms(300);
	
    
    //for (unsigned int i=0; i < sizeof(stringOfText) - 1; i++) 
    //    device_write(&display, i, stringOfText[i]);
    
    //for (unsigned int i=0; i < sizeof(stringAcce/l) - 1; i++) 
    //    device_write(&display, i + 0x50 + 20, stringAccel[i]);
    
    
    
    
    
    
    
    
    
    bus_control_zero();
    bus_address_zero();
    
    while(1) {
        
        
        
        continue;
    }
    
    return 0;
}
