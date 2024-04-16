#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/network.h>

#include <drivers/network/NIC/main.h>

struct Driver* networkDevice;

uint8_t ntPacketSend(struct NetworkPacket* packet) {
    
    // Start byte to indicate the beginning of a packet
    ntSend(&packet->start, 1);
    
    // Send the destination and source address
    ntSend(&packet->addr_d[0], 2);
    ntSend(&packet->addr_s[0], 2);
    
    ntSend(&packet->data[0], 16);
    
    // Stop byte to indicate the packet is complete
    ntSend(&packet->stop, 1);
    
    return 1;
}

uint8_t ntPacketReceive(struct NetworkPacket* packet) {
    
    uint8_t buffer[64];
    
    uint8_t receiveBufferSz = 0;
    
    networkDevice->read( 0x0b, &receiveBufferSz);
    
    if (receiveBufferSz == 0) 
        return 0;
    
    if (receiveBufferSz > 64) 
        receiveBufferSz = 64;
    
    // Find a packet start byte
    for (uint8_t i=0; i < receiveBufferSz; i++) 
        networkDevice->read( 0x10 + i, &buffer[i]);
    
    for (uint8_t i=0; i < receiveBufferSz; i++) {
        
        if (buffer[i] == NETWORK_PACKET_START_BYTE) {
            
            if (buffer[i + 21] == NETWORK_PACKET_STOP_BYTE) {
                
                packet->start = buffer[i];
                
                packet->addr_d[0] = buffer[i + 1];
                packet->addr_d[1] = buffer[i + 2];
                packet->addr_s[0] = buffer[i + 3];
                packet->addr_s[1] = buffer[i + 4];
                
                for (uint8_t d=0; d < 16; d++) 
                    packet->data[d] = buffer[5 + i];
                
                packet->stop = buffer[i + 19];
                
                return 1;
            }
            
        }
        
    }
    
    return 0;
}

uint8_t ntPacketClean(void) {
    
    uint8_t buffer;
    
    uint8_t receiveBufferSz = 0;
    
    networkDevice->read( 0x0b, &receiveBufferSz);
    
    if (receiveBufferSz == 0) 
        return 0;
    
    if (receiveBufferSz > 64) 
        receiveBufferSz = 64;
    
    // Find a packet start byte
    for (uint8_t i=0; i < receiveBufferSz; i++) {
        
        networkDevice->read( 0x10 + i, &buffer);
        
        if (buffer == NETWORK_PACKET_START_BYTE) {
            
            networkDevice->read( 0x10 + i + 21, &buffer);
            
            if (buffer == NETWORK_PACKET_STOP_BYTE) {
                
                for (uint8_t i=0; i < receiveBufferSz; i++) 
                    networkDevice->write( 0x10 + i, 0x00);
                
                return 1;
            }
            
        }
        
    }
    
    return 0;
}



//
// Low level networking functions
//

void ntInitiate(void) {
    
    uint8_t nameNetwork[] = "network";
	networkDevice = (struct Driver*)GetDriverByName( nameNetwork, sizeof(nameNetwork) );
	
	return;
}


void ntSend(uint8_t* buffer, uint8_t bufferSize) {
    
    for (uint8_t i=0; i < bufferSize; i++) {
        
        // Write the buffer
        networkDevice->write( 0x0c, buffer[i]);
        
        // Set the flag to send the byte
        networkDevice->write( 0x0a, 0x01);
        
        // Check for the byte to finish being sent
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

uint8_t ntReceive(uint8_t* buffer, uint8_t bufferSize) {
    
    uint8_t receiveBufferSz = 0;
    
    networkDevice->read( 0x0b, &receiveBufferSz);
    
    if (receiveBufferSz == 0) 
        return 0;
    
    if (bufferSize > receiveBufferSz) 
        bufferSize = receiveBufferSz;
    
    for (uint8_t i=0; i < bufferSize; i++) 
        networkDevice->read( 0x10 + i, &buffer[i]);
    
    return bufferSize;
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

