#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/repair.h>

void functionRepair(uint8_t* param, uint8_t param_length) {
    
    fsRepairDevice();
    
    return;
}

void registerCommandRepair(void) {
    
    uint8_t repairCommandName[] = "repair";
    
    ConsoleRegisterCommand(repairCommandName, sizeof(repairCommandName), functionRepair);
    
    return;
}
