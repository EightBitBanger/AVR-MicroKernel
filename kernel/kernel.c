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
    
    // Initiate console prompt
    uint8_t prompt[] = " >";
    prompt[0] = fsGetDeviceRoot();
    
    ConsoleSetPrompt(prompt, sizeof(prompt));
    
    // Initiate temporary storage
    fsFormat(0, FORMAT_CAPACITY_8K);
    
    
    
    //
    // Create bin directory
    
    uint8_t directoryName[] = "bin";
    fsDirectoryCreate(directoryName, sizeof(directoryName));
    
    fsChangeWorkingDirectory(directoryName, sizeof(directoryName));
    
    
    
    //
    // Executable example
    
    uint8_t versionFileName[]  = "ver";
    uint32_t versionFileAddress = fsFileCreate(versionFileName, sizeof(versionFileName), 80);
    
    struct FSAttribute attribTest;
    attribTest.executable = 'x';
    attribTest.readable   = 'r';
    attribTest.writeable  = 'w';
    attribTest.type       = ' ';
    
    fsFileSetAttributes(versionFileAddress, &attribTest);
    
    uint8_t versionMajorStr[4];
    uint8_t versionMinorStr[4];
    uint8_t versionPatchStr[4];
    
    uint8_t majLen = int_to_string( _KERNEL_VERSION_MAJOR__, versionMajorStr );
    uint8_t minLen = int_to_string( _KERNEL_VERSION_MINOR__, versionMinorStr );
    uint8_t patLen = int_to_string( _KERNEL_VERSION_PATCH__, versionPatchStr );
    
    fsFileOpen(versionFileAddress);
    uint8_t bufferTest[] = {0x89, 0x00,  9,   // Call to print a string to the display
                            0x89, 0x01, 20,   // Number of characters to print
                            0x89, 0x02,  0,   // High byte offset to data start address
                            0x89, 0x03, 16,   // Low byte offset to data start address
                            0xcc, 0x10,       // Run display routine
                            0xcc, 0x20,       // Return to the shell
                            
                            'V', 'e', 'r', 's', 'i', 'o', 'n', ' ',
                            
                            ' ',' ',' ',' ',' ',' ',' ',' ',' '
                            
                            };
    
    uint8_t currentChar = 0;
    
    if (majLen == 2) {
        
        bufferTest[24] = versionMajorStr[0];
        bufferTest[25] = versionMajorStr[1];
        bufferTest[26] = '.';
        currentChar = 27;
    } else {
        bufferTest[24] = versionMajorStr[0];
        bufferTest[25] = '.';
        currentChar = 26;
    }
    
    if (minLen == 2) {
        
        bufferTest[currentChar]   = versionMinorStr[0];
        bufferTest[currentChar+1] = versionMinorStr[1];
        bufferTest[currentChar+2] = '.';
        currentChar += 3;
    } else {
        bufferTest[currentChar] = versionMinorStr[0];
        bufferTest[currentChar+1] = '.';
        currentChar += 2;
    }
    
    if (patLen == 2) {
        
        bufferTest[currentChar]   = versionPatchStr[0];
        bufferTest[currentChar+1] = versionPatchStr[1];
    } else {
        bufferTest[currentChar] = versionPatchStr[0];
    }
    
    fsFileWrite(bufferTest, sizeof(bufferTest));
    fsFileClose();
    
    
    
    //
    // Create drivers directory
    
    fsClearWorkingDirectory();
    
    uint8_t directorySubName[] = "drivers";
    fsDirectoryCreate(directorySubName, sizeof(directorySubName));
    
    fsChangeWorkingDirectory(directorySubName, sizeof(directorySubName));
    
    
    
    
    
    //
    // Driver example
    
    uint8_t driverFileName[]  = "kmod";
    uint32_t kmodFileAddress = fsFileCreate(driverFileName, sizeof(driverFileName), 40);
    
    fsFileOpen(kmodFileAddress);
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
    
    fsFileWrite(bufferDrv, sizeof(bufferDrv));
    fsFileClose();
    
    fsClearWorkingDirectory();
    
    
    //
    // Document example
    
    uint8_t docFileName[]  = "doc";
    uint32_t docFileAddress = fsFileCreate(docFileName, sizeof(docFileName), 80);
    
    struct FSAttribute attribDoc;
    attribDoc.executable = ' ';
    attribDoc.readable   = 'r';
    attribDoc.writeable  = 'w';
    attribDoc.type       = ' ';
    
    fsFileSetAttributes(docFileAddress, &attribDoc);
    
    fsFileOpen(docFileAddress);
    uint8_t bufferDoc[] = "Line A\nNext Line\nLine C\nLast Line\n\0";
    
    fsFileWrite(bufferDoc, sizeof(bufferDoc));
    fsFileClose();
    
    
    
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

