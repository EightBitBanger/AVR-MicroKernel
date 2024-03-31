#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/network.h>

#include <drivers/network/NIC/main.h>

struct Driver* networkDevice;

uint8_t ntPacketSend(struct NetworkPacket* packet) {
    
    return;
}


//
// Low level networking functions
//

void ntInitiate(void) {
    
    uint8_t nameNetwork[] = "network";
	networkDevice = (struct Driver*)GetDriverByName( nameNetwork, sizeof(nameNetwork) );
	
	return;
}


void ntSend(uint8_t* buffer, uint16_t bufferSize) {
    
    for (uint8_t i=0; i < bufferSize; i++) {
        
        // Write the buffer
        networkDevice->write( 0x0c, buffer[i]);
        
        // Set the flag to send the byte
        networkDevice->write( 0x0a, 0x01);
        
        // Check for the byte to complete being sent
        uint8_t checkByte = 1;
        networkDevice->read( 0x0a, &checkByte);
        
        if (checkByte == 0) 
            continue;
        
        // Check after a few cycles
        for (uint32_t t=0; t < 10000; t++) {
            
            _delay_us(1);
            
            networkDevice->read( 0x0a, &checkByte);
            
            if (checkByte == 0) 
                break;
            
        }
        
    }
    
    return;
}


uint8_t ntReceive(uint8_t* buffer, uint16_t bufferSize) {
    
    uint8_t receiveBufferSz = 0;
    
    networkDevice->read( 0x0b, &receiveBufferSz);
    
    if (receiveBufferSz > 0) {
        
        if (bufferSize > receiveBufferSz) 
            bufferSize = receiveBufferSz;
        
        for (uint8_t i=0; i < bufferSize; i++) {
            
            networkDevice->read( 0x10 + i, &buffer[i]);
            
        }
        
        return bufferSize;
    }
    
    return receiveBufferSz;
}

void ntReceiveClear(void) {
    
    networkDevice->write( 0x0b, 0x00);
    
    return;
}

void ntSetBaudRate(uint8_t baudRate) {
    
    networkDevice->write( 0x0e, baudRate);
    networkDevice->write( 0x0d, 0x01);
    
    return;
}

