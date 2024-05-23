#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/network/network.h>

//#include <drivers/network/NIC/main.h>

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

uint8_t ntPacketReceive(struct NetworkPacket* packet, uint8_t clearOldPacket) {
    
    uint8_t buffer[200];
    
    for (uint8_t i=0; i < 200; i++) 
        networkDevice->read( 0x10 + i, &buffer[i]);
    
    // Find a packet start byte
    for (uint8_t i=0; i < 200; i++) {
        
        if (buffer[i] == NETWORK_PACKET_START_BYTE) {
            
            if (buffer[i + 21] == NETWORK_PACKET_STOP_BYTE) {
                
                packet->start = NETWORK_PACKET_START_BYTE;
                
                packet->addr_d[0] = buffer[i + 1];
                packet->addr_d[1] = buffer[i + 2];
                packet->addr_s[0] = buffer[i + 3];
                packet->addr_s[1] = buffer[i + 4];
                
                for (uint8_t d=0; d < NETWORK_PACKET_DATA_SIZE; d++) 
                    packet->data[d] = buffer[5 + i + d];
                
                packet->stop = NETWORK_PACKET_STOP_BYTE;
                
                // Clear the old packet
                if (clearOldPacket == 1) {
                    
                    for (uint8_t c=0; c <= NETWORK_PACKET_SIZE; c++) 
                        networkDevice->write( 0x10 + i + c, 0x00);
                    
                }
                
                return 1;
            }
            
        }
        
    }
    
    return 0;
}

uint8_t ntPacketClearAll(void) {
    
    uint8_t receiveBufferSz = 0;
    
    for (uint8_t i=0; i < 200; i++) 
        networkDevice->write( 0x10 + i, 0x00);
    
    networkDevice->write( 0x0b, receiveBufferSz);
    
    return 1;
}


//
// Low level networking functions
//

void ntInit(void) {
    
    uint8_t nameNetwork[] = "network";
	networkDevice = (struct Driver*)GetDriverByName( nameNetwork, sizeof(nameNetwork) );
	
	networkDevice->interface.read_waitstate  = 20;
	networkDevice->interface.write_waitstate = 20;
	
	// Initiate NIC baud rate
	ntSetBaudRate( NETWORK_BAUD_RATE );
	
	return;
}


void ntSend(uint8_t* buffer, uint8_t bufferSize) {
    
    for (uint8_t i=0; i < bufferSize; i++) {
        
        // Write the buffer
        networkDevice->write( 0x0c, buffer[i]);
        
        // Set the flag to send the byte
        networkDevice->write( 0x0a, 0x01);
        
        _delay_us(100);
        
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
    _delay_us(1);
    
    if (receiveBufferSz == 0) 
        return 0;
    
    if (bufferSize > receiveBufferSz) 
        bufferSize = receiveBufferSz;
    
    for (uint8_t i=0; i < bufferSize; i++) {
        networkDevice->read( 0x10 + i, &buffer[i]);
        _delay_us(1);
    }
    
    return bufferSize;
}

void ntReceiveClear(void) {
    
    networkDevice->write( 0x0b, 0x00);
    
    _delay_us(300);
    
    return;
}

void ntSetBaudRate(uint8_t baudRate) {
    
    networkDevice->write( 0x0e, baudRate);
    _delay_us(100);
    networkDevice->write( 0x0d, 0x01);
    _delay_us(100);
    
    return;
}

uint8_t ntGetBaudRate(void) {
    
    uint8_t baudrate;
    
    networkDevice->read( 0x0e, &baudrate);
    _delay_us(100);
    
    return baudrate;
}

void ntBindDevice(uint8_t deviceIndex) {
    
    networkDevice->device.hardware_address = PERIPHERAL_ADDRESS_BEGIN + (deviceIndex * 0x10000);
    
    return;
}

uint8_t ntCheckDevice(void) {
    
    uint8_t buffer[10];
    
    for (uint8_t i=0; i < 10; i++) {
        networkDevice->read( i, &buffer[i]);
        _delay_us(100);
    }
    
    uint8_t deviceName[] = "network";
    
    for (uint8_t i=0; i < sizeof(deviceName) - 1; i++) 
        if (deviceName[i] != buffer[i + 1]) 
            return 0;
    
    return 1;
}


