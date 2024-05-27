#ifndef __KERNEL_CORE_
#define __KERNEL_CORE_

#include <avr/io.h>

#include <kernel/version.h>

#include <kernel/kalloc.h>

#include <kernel/pointers.h>

#include <kernel/interrupt.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

#include <kernel/cstring.h>

#include <kernel/fs/fs.h>

#include <kernel/command/cli.h>

#include <kernel/network/network.h>
#include <kernel/network/router.h>
#include <kernel/network/server.h>

#include <kernel/emulation/x4/x4.h>

#include <kernel/scheduler.h>

void kInit(void);

void kernelVectorTableInit(void);

void _ISR_hardware_service_routine(void);


#endif
