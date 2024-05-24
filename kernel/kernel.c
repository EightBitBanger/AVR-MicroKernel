#include <kernel/kernel.h>
#include <kernel/delay.h>

#define INT_VECT_0  254
#define INT_VECT_1  253
#define INT_VECT_2  251
#define INT_VECT_3  247
#define INT_VECT_4  239
#define INT_VECT_5  223
#define INT_VECT_6  191
#define INT_VECT_7  127

#define INTERRUPT_VECTOR_TABLE_SIZE  8
void defaultLandingFunction(uint8_t index) {}

void (*interrupt_vector_table[INTERRUPT_VECTOR_TABLE_SIZE])(uint8_t);

void kInit(void) {
    
    // Enumerate available hardware devices
    for (uint8_t d=1; d <= NUMBER_OF_PERIPHERALS; d++) {
        
        fsSetCurrentDevice( d );
        
        if (fsCheckDeviceReady() == 0) 
            continue;
        
        fsSetCurrentDevice( 0xff );
        
        // Set the root directory
        uint8_t filename[] = "storage0";
        filename[8] = '0' + d;
        fsFileCreate(filename, sizeof(filename), 20);
        
        continue;
    }
    
    fsSetCurrentDevice( 0xff );
    
    return;
}

void kernelVectorTableInit(void) {
    
    for (uint8_t i=0; i < INTERRUPT_VECTOR_TABLE_SIZE; i++) {
        
        interrupt_vector_table[i] = defaultLandingFunction;
        
    }
    
    return;
}

void _ISR_hardware_service_routine(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t iVect = 0;
    
    bus_read_io(&bus, 0x00000, &iVect);
    
    if (iVect == INT_VECT_0) {interrupt_vector_table[0](0);}
    if (iVect == INT_VECT_1) {interrupt_vector_table[1](1);}
    if (iVect == INT_VECT_2) {interrupt_vector_table[2](2);}
    if (iVect == INT_VECT_3) {interrupt_vector_table[3](3);}
    if (iVect == INT_VECT_4) {interrupt_vector_table[4](4);}
    if (iVect == INT_VECT_5) {interrupt_vector_table[5](5);}
    if (iVect == INT_VECT_6) {interrupt_vector_table[6](6);}
    if (iVect == INT_VECT_7) {interrupt_vector_table[7](7);}
    
    return;
}

