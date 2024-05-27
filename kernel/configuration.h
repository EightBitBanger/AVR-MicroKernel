//
// Hardware configuration

// 
// Kernel console application functions

//#define DONT_INCLUDE_CONSOLE_COMMANDS

#define INCLUDE_KERNEL_APPLICATIONS
#define INCLUDE_NETWORK_APPLICATIONS
#define INCLUDE_FILE_SYSTEM_APPLICATIONS



// 
// Networking

//#define NETWORK_APPLICATION_PACKET_ROUTER
//#define NETWORK_APPLICATION_SERVER





//
// Hardware maps

#ifdef BOARD_RETROBOARD_REV2
    
    #define EXTERNAL_MEMORY_BEGIN     0x00000
    
    #define PERIPHERAL_ADDRESS_BEGIN  0x40000
    #define PERIPHERAL_STRIDE         0x10000
    
    #define NUMBER_OF_PERIPHERALS  5
    
#endif

#ifdef BOARD_RETRO_AVR_X4_REV1
    
    #define EXTERNAL_MEMORY_BEGIN     0x00000
    
    #define PERIPHERAL_ADDRESS_BEGIN  0x10000
    #define PERIPHERAL_STRIDE         0x10000
    
    #define NUMBER_OF_PERIPHERALS  5
    
#endif

