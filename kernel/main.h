#include <avr/io.h>
#include <stdlib.h>

#include <kernel/delay.h>

#include <kernel/board.h>

#include <kernel/kernel.h>

// Included console commands
#ifdef INCLUDE_KERNEL_APPLICATIONS
  
  #include <kernel/commands/list.h>
  #include <kernel/commands/device.h>
  #include <kernel/commands/cls.h>
  #include <kernel/commands/edit.h>
  #include <kernel/commands/asm.h>
  #include <kernel/commands/task.h>
  
#endif


#ifdef INCLUDE_NETWORK_APPLICATIONS
  
  #include <kernel/commands/net.h>
  
#endif


#ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
  
  #include <kernel/commands/fs/dir.h>
  #include <kernel/commands/fs/copy.h>
  #include <kernel/commands/fs/cd.h>
  #include <kernel/commands/fs/cap.h>
  #include <kernel/commands/fs/mk.h>
  #include <kernel/commands/fs/rm.h>
  #include <kernel/commands/fs/rn.h>
  
  #include <kernel/commands/fs/attrib.h>
  
  #include <kernel/commands/fs/repair.h>
  #include <kernel/commands/fs/format.h>
  
#endif



// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>
#include <drivers/network/NIC/main.h>
#include <drivers/audio/speaker/main.h>

