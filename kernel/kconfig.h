//
// Hardware configuration

// --------------------------------------------
// Target board
//

#define  BOARD_RETROBOARD_REV2
//#define  BOARD_RETRO_AVR_X4_REV1


// --------------------------------------------
// Boot options

//#define _BOOT_SAFEMODE__
#define _BOOT_DETAILS__

#define _KERNEL_ALLOCATE_EXTERNAL_MEMORY__
#define _KERNEL_PRINT_VERSION_INFORMATION__


// --------------------------------------------
// Kernel console application function sets

//#define DONT_INCLUDE_CONSOLE_COMMANDS
#define INCLUDE_KERNEL_APPLICATIONS
#define INCLUDE_NETWORK_APPLICATIONS
#define INCLUDE_FILE_SYSTEM_APPLICATIONS


// --------------------------------------------
// Networking

//#define NETWORK_APPLICATION_PACKET_ROUTER
//#define NETWORK_APPLICATION_SERVER




// --------------------------------------------
// INTERNAL


// --------------------------------------------
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

