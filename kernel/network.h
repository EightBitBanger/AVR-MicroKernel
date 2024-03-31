#ifndef __NETWORK_INTERFACE_
#define __NETWORK_INTERFACE_

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

#include <kernel/cstring.h>

struct NetworkPacket {
	
	uint8_t start;       // Start byte
	
	uint8_t addr_d[2];   // Destination device address
	uint8_t addr_s[2];   // Source device address
	
	uint8_t data[16];    // Data block
	
	uint8_t stop;        // Stop byte
	
};

#define _PACKET_SIZE__        sizeof(struct NetworkPacket)

#define NETWORK_PACKET_START_BYTE   0x55
#define NETWORK_PACKET_TYPE_DATA    0x00

#define NETWORK_PACKET_STOP_BYTE    0xaa

uint8_t ntPacketSend(struct NetworkPacket* packet);


// TX/RX UART communication
void ntInitiate(void);

void ntSend(uint8_t* buffer, uint16_t bufferSize);

uint8_t ntReceive(uint8_t* buffer, uint16_t bufferSize);

void ntReceiveClear(void);

void ntSetBaudRate(uint8_t baudRate);

#endif
