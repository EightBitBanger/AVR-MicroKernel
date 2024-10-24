#include <avr/io.h>

extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

#include <kernel/array.h>
#include <kernel/string.h>

void functionTest(uint8_t* param, uint8_t param_length) {
    
    string str;
    uint8_t msgString[] = "String of text\0";
    
    str = msgString;
    
    
    uint8_t msgSrc[sizeof(msgString)];
    str.data(msgSrc, sizeof(msgSrc));
    
    print(msgSrc, sizeof(msgSrc));
    printLn();
    
    
    array<uint32_t> arr();
    
    arr.push_back( 32768 );
    arr.push_back( 888777);
    arr.push_back( 512256 );
    
    
    //string test;
    //test = (string)arr[0];
    
    //uint8_t msgSrc[] = "                ";
    //test.data(msgSrc, sizeof(msgSrc));
    
    //print(msgSrc, sizeof(msgSrc));
    //printLn();
    
    return;
}

void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
