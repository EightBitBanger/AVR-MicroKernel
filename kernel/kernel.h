#ifndef __KERNEL_CORE_
#define __KERNEL_CORE_

#include <avr/io.h>

#include <kernel/kconfig.h>
#include <kernel/version.h>

#include <kernel/kref.h>
#include <kernel/kalloc.h>
#include <kernel/pointers.h>
#include <kernel/throw.h>

#include <kernel/bitwise.h>

#include <kernel/interrupt.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

#include <kernel/drivers.h>

#include <kernel/cstring.h>

#include <kernel/fs/fs.h>

#include <kernel/command/cli.h>

#include <kernel/network/network.h>
#include <kernel/network/router.h>
#include <kernel/network/server.h>

#include <kernel/emulation/x4/x4.h>

#include <kernel/scheduler.h>

#include <kernel/virtual/virtual.h>

// System subroutine calls
#include <kernel/syscalls/alloc/new.h>
#include <kernel/syscalls/print/print.h>
#include <kernel/syscalls/beep/beep.h>
#include <kernel/syscalls/switch/switch.h>

#define nullptr  0x00000000


void kInit(void);

void KernelVectorTableInit(void);

void SetInterruptVector(uint8_t index, void(*servicePtr)());

void _ISR_hardware_service_routine(void);


#endif
