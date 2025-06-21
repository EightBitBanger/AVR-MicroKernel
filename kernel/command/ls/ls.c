#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/ls/ls.h>

void functionLS(uint8_t* param, uint8_t param_length) {
    
    struct Partition part = fsDeviceOpen(0x00000000);
    DirectoryHandle rootDirectory = fsDeviceGetRootDirectory(part);
    
    vfsList(part, rootDirectory);
    
    return;
}

void registerCommandLS(void) {
    uint8_t lsCommandName[] = "ls";
    ConsoleRegisterCommand(lsCommandName, sizeof(lsCommandName), functionLS);
    return;
}
