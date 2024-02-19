#include <avr/io.h>

#include <kernel/kernel.h>

struct Device device_table[32];


void kernel_initiate(void) {
    
    struct Bus bus;
    
    bus.read_waitstate  = 20;
    bus.write_waitstate = 20;
    
    // Check peripheral slots
    for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        device_table[d].hardware_address = PERIPHERAL_ADDRESS_BEGIN + (0x10000 * d);
        
        char nameChar=0x00;
        
        // Copy device name
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) {
            
            bus_read_byte(&bus, device_table[d].hardware_address + i, &nameChar);
            
            device_table[d].device_name[i] = nameChar;
        }
        
        continue;
    }
    
    return;
}


struct Device* GetDevice(unsigned int deviceIndex) {
    return &device_table[ deviceIndex ];
}






int is_number(char* charPtr) {
    if ((*charPtr >= 0x30) & (*charPtr <= 0x39))
        return 1;
    return 0;
}

int is_letter(char* charPtr) {
    if (is_uppercase(charPtr)) return 1;
    if (is_lowercase(charPtr)) return 1;
    return 0;
}


int is_uppercase(char* charPtr) {
    if ((*charPtr >= 0x41) & (*charPtr <= 0x5a))
        return 1;
    return 0;
}

int is_lowercase(char* charPtr) {
    if ((*charPtr >= 0x61) & (*charPtr <= 0x7a))
        return 1;
    return 0;
}

void make_uppercase(char* charPtr) {
    if (is_lowercase(charPtr) == 1) 
        charPtr -= 0x20;
    return;
}

void make_lowercase(char* charPtr) {
    if (is_uppercase(charPtr) == 1) 
        charPtr += 0x20;
    return;
}


