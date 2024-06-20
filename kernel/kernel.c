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
    
    fsClearWorkingDirectory();
    
    fsFormat(0, CAPACITY_8K);
    
    
    uint32_t fileAddressA = fsSectorAllocate(100);
    uint32_t fileAddressB = fsSectorAllocate(100);
    uint32_t fileAddressC = fsSectorAllocate(100);
    
    
    
    
    
    
    
    return;
    
    
    
    
    
    
    
    
    
    
    
    
    uint8_t kernelDirName[]  = "kernel";
    fsDirectoryCreate(kernelDirName, sizeof(kernelDirName)-1);
    
    uint8_t drvDirName[]  = "drivers";
    fsDirectoryCreate(drvDirName, sizeof(drvDirName)-1);
    
    uint8_t binDirName[]  = "bin";
    fsDirectoryCreate(binDirName, sizeof(binDirName)-1);
    
    uint8_t userDirName[]  = "user";
    fsDirectoryCreate(userDirName, sizeof(userDirName)-1);
    
    
    
    fsSetWorkingDirectory(kernelDirName, sizeof(kernelDirName)-1);
    
    uint8_t subsysDirName[]  = "subsys";
    fsDirectoryCreate(subsysDirName, sizeof(subsysDirName)-1);
    
    
    
    
    
    
    
    
    
    // Create version executable
    
    fsSetWorkingDirectory(kernelDirName, sizeof(kernelDirName)-1);
    
    uint8_t testFileName[]  = "ver";
    fsFileCreate(testFileName, sizeof(testFileName)-1, 80, ' ');
    
    struct FSAttribute attribTest;
    attribTest.executable = 'x';
    attribTest.readable   = 'r';
    attribTest.writeable  = 'w';
    attribTest.type       = ' ';
    
    fsSetFileAttributes(testFileName, sizeof(testFileName)-1, &attribTest);
    
    uint8_t index = fsFileOpen(testFileName, sizeof(testFileName)-1);
    uint8_t bufferTest[] = {0x89, 0x03, 'V', 0xcc, 0x10, 
                            0x89, 0x03, 'e', 0xcc, 0x10, 
                            0x89, 0x03, 'r', 0xcc, 0x10, 
                            0x89, 0x03, 's', 0xcc, 0x10, 
                            0x89, 0x03, 'i', 0xcc, 0x10, 
                            0x89, 0x03, 'o', 0xcc, 0x10, 
                            0x89, 0x03, 'n', 0xcc, 0x10, 
                            0x89, 0x03, ' ', 0xcc, 0x10, 
                            0x89, 0x03, '0', 0xcc, 0x10, 
                            0x89, 0x03, '.', 0xcc, 0x10, 
                            0x89, 0x03, '0', 0xcc, 0x10, 
                            0x89, 0x03, '.', 0xcc, 0x10, 
                            0x89, 0x03, '1', 0xcc, 0x10, 
                            
                            0xcc, 0x20};
    
    
    fsFileWrite(index, bufferTest, sizeof(bufferTest));
    fsFileClose(index);
    
    
    
    
    
    
    
    // Create test driver in drivers directory
    
    fsSetWorkingDirectory(drvDirName, sizeof(drvDirName)-1);
    
    
    uint8_t driverFileName[]  = "kmod";
    fsFileCreate(driverFileName, sizeof(driverFileName)-1, 40, ' ');
    
    index = fsFileOpen(driverFileName, sizeof(driverFileName)-1);
    uint8_t bufferDrv[] = "  kmod      $XXXXX";
    
    bufferDrv[0] = 'K';      // Marker bytes KD (kernel driver)
    bufferDrv[1] = 'D';
    
    // Next 10 bytes reserved for driver name
    
    bufferDrv[12] = '$';     // Marker byte '$'
    bufferDrv[13] = 0x13;    // Device ID
    bufferDrv[14] = 0x02;    // Bus read wait-state
    bufferDrv[15] = 0x01;    // Bus write wait-state
    bufferDrv[16] = 0x00;    // Bus interface type
    bufferDrv[17] = 0x00;    // Sub system registry type
    
    fsFileWrite(index, bufferDrv, sizeof(bufferDrv) - 1);
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

