#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>



uint8_t int_get_string(uint32_t number, uint8_t* destination_string) {
	
	uint8_t ones = 0x30;
	uint8_t tens = 0x30;
	uint8_t hnds = 0x30;
	uint8_t thou = 0x30;
	uint8_t ttho = 0x30;
	uint8_t htho = 0x30;
	uint8_t mill = 0x30;
	
	// Find number of characters
	uint8_t place=0;
	if (number > 0)      place = 1;
	if (number > 9)      place = 2;
	if (number > 99)     place = 3;
	if (number > 999)    place = 4;
	if (number > 9999)   place = 5;
	if (number > 99999)  place = 6;
	if (number > 999999) place = 7;
	
	// Convert to character
	while (number > 9999999) {number -= 10000000;}
	while (number > 999999)  {number -= 1000000;   mill += 1;}
	while (number > 99999)   {number -= 100000;    htho += 1;}
	while (number > 9999)    {number -= 10000;     ttho += 1;}
	while (number > 999)     {number -= 1000;      thou += 1;}
	while (number > 99)      {number -= 100;       hnds += 1;}
	while (number > 9)       {number -= 10;        tens += 1;}
	while (number > 0)       {number -= 1;         ones += 1;}
	
	// Figure length of string
	if (place == 0) destination_string[place]   = ones;
	if (place > 0)  destination_string[place-1] = ones;
	if (place > 1)  destination_string[place-2] = tens;
	if (place > 2)  destination_string[place-3] = hnds;
	if (place > 3)  destination_string[place-4] = thou;
	if (place > 4)  destination_string[place-5] = ttho;
	if (place > 5)  destination_string[place-6] = htho;
	if (place > 6)  destination_string[place-7] = mill;
	
	return place;
}


void get_hex_string(uint32_t integer, uint8_t* string) {
	
	uint8_t hex[2] = {0, 0};
	
	while (integer >= 16) {
		integer -= 16;
		hex[0]++;
	}
	
	while (integer > 0) {
		integer -= 1;
		hex[1]++;
	}
	
	if (hex[0] > 9) {hex[0] += 'a' - 10;} else {hex[0] += '0';}
	if (hex[1] > 9) {hex[1] += 'a' - 10;} else {hex[1] += '0';}
	
	string[0] = hex[0];
	string[1] = hex[1];
	
	return;
}









int main(void) {
    
    // Zero the system bus
    bus_address_zero();
    bus_control_zero();
    
    // Allow board some time to stabilize
    _delay_ms(700);
	
	
	// Initiate drivers here
	
	initiateDisplayDriver();
	initiatePS2Driver();
	
	
	
	// Initiate the kernel
	
	InitiateDeviceTable();
    
    
    
    
    
    
    
	uint8_t nameKeyboard[] = "PS2";
	
	struct Driver* keyboaDriver = (struct Driver*)GetDriverByName( nameKeyboard, sizeof(nameKeyboard) );
	
	
    
    
	
	
	// Request the display driver from the kernel
	uint8_t nameString[] = "display";
	
	struct Driver* displayDriver = (struct Driver*)GetDriverByName( nameString, sizeof(nameString) );
	
	/*
	
	// Print off names of attached peripheral devices
	
	displayDriver->write( SET_CURSOR_BLINK_RATE, 0x00 );
	
	for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        struct Device* device = GetHardwareDeviceByIndex(d);
        
        if (is_letter(&device->device_name[1]) == 0) 
            continue;
        
        for (uint8_t i=0; i < DEVICE_NAME_LENGTH - 1; i++) 
            displayDriver->write( i + (d * 20), device->device_name[1 + i] );
        
    }
    
	_delay_ms( 1000 );
    
    displayDriver->write( CLEAR_FRAME_BUFFER, 0x01 );
	
	_delay_ms( 300 );
    
    */
    
	
    
    //bus_address_zero();
    bus_control_zero();
    
    
    
    PORTD = 0x00;
    
    //PORTD |= (1 << PD4);  // Red
    PORTD |= (1 << PD5);  // Green
    
    _delay_ms(100);
    
    
    
    
    
    displayDriver->write( SET_CURSOR_BLINK_RATE, 0x00 );
	
    while(1) {
        
        uint8_t scanCode = 0;
        
        keyboaDriver->read( 0x00000, &scanCode );
        
        if (scanCode == 0x00) 
            continue;
        
        displayDriver->write( 0x00000, scanCode );
        
        continue;
    }
    
    return 0;
}
