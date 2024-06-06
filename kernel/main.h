#include <avr/io.h>
#include <stdlib.h>

#include <kernel/delay.h>

#include <kernel/configuration.h>

#include <kernel/kernel.h>

// Included console commands
#ifdef INCLUDE_KERNEL_APPLICATIONS
  
  // Text editor
  #include <kernel/command/edit/edit.h>
  // Assembler
  #include <kernel/command/asm/asm.h>
  
  #include <kernel/command/list/list.h>
  #include <kernel/command/device/device.h>
  #include <kernel/command/cls/cls.h>
  #include <kernel/command/task/task.h>
  
  #include <kernel/command/type/type.h>
  
#endif


#ifdef INCLUDE_NETWORK_APPLICATIONS
  
  #include <kernel/command/net/net.h>
  
#endif


#ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
  
  #include <kernel/command/ls/ls.h>
  #include <kernel/command/copy/copy.h>
  #include <kernel/command/cd/cd.h>
  #include <kernel/command/cap/cap.h>
  #include <kernel/command/mk/mk.h>
  #include <kernel/command/rm/rm.h>
  #include <kernel/command/rn/rn.h>
  
  #include <kernel/command/mkdir/mkdir.h>
  #include <kernel/command/rmdir/rmdir.h>
  
  #include <kernel/command/attrib/attrib.h>
  
  #include <kernel/command/repair/repair.h>
  #include <kernel/command/format/format.h>
  
#endif



// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>
#include <drivers/network/NIC/main.h>
#include <drivers/audio/speaker/main.h>

