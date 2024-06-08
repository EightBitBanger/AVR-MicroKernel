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
    
    uint8_t deviceLetter = 'X';
    
    fsSetDeviceLetter(deviceLetter);
    fsSetRootDirectory(deviceLetter);
    
    uint8_t binDirName[]  = "bin";
    fsDirectoryCreate(binDirName, sizeof(binDirName)-1);
    
    uint8_t drvDirName[]  = "drivers";
    fsDirectoryCreate(drvDirName, sizeof(drvDirName)-1);
    
    fsSetWorkingDirectory(drvDirName, sizeof(drvDirName)-1);
    
    uint8_t driverFileName[]  = "sys";
    fsFileCreate(driverFileName, sizeof(driverFileName)-1, 40, ' ');
    
    uint8_t index = fsFileOpen(driverFileName, sizeof(driverFileName)-1);
    
    uint8_t buffer[] = "kmod      xxxx";
    
    fsFileWrite(index, buffer, sizeof(buffer) - 1);
    
    fsFileClose(index);
    
    fsClearWorkingDirectory();
    
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
    
    switch(iVect) {
        
        case INT_VECT_0: interrupt_vector_table[0](0); break;
        case INT_VECT_1: interrupt_vector_table[1](1); break;
        case INT_VECT_2: interrupt_vector_table[2](2); break;
        case INT_VECT_3: interrupt_vector_table[3](3); break;
        case INT_VECT_4: interrupt_vector_table[4](4); break;
        case INT_VECT_5: interrupt_vector_table[5](5); break;
        case INT_VECT_6: interrupt_vector_table[6](6); break;
        case INT_VECT_7: interrupt_vector_table[7](7); break;
        
        default:
            break;
        
    }
    
    return;
}

