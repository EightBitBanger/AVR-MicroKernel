#ifndef __NETWORK_INTERFACE_
#define __NETWORK_INTERFACE_

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

#include <kernel/cstring.h>


#define NETWORK_BAUD_RATE           9

#define NETWORK_PACKET_SIZE         sizeof(struct NetworkPacket)

#define NETWORK_PACKET_DATA_SIZE    16

#define NETWORK_PACKET_START_BYTE   0x55
#define NETWORK_PACKET_TYPE_DATA    0x00

#define NETWORK_PACKET_STOP_BYTE    0xaa


struct NetworkPacket {
	
	uint8_t start;       // Start byte
	
	uint8_t addr_d[2];   // Destination device address
	uint8_t addr_s[2];   // Source device address
	
	// Data block
	uint8_t data[NETWORK_PACKET_DATA_SIZE];
	
	uint8_t stop;        // Stop byte
	
};

uint8_t ntPacketSend(struct NetworkPacket* packet);

uint8_t ntPacketReceive(struct NetworkPacket* packet, uint8_t clearOldPacket);

uint8_t ntPacketClearAll(void);


void ntInitiate(void);

void ntSend(uint8_t* buffer, uint8_t bufferSize);

uint8_t ntReceive(uint8_t* buffer, uint8_t bufferSize);

void ntReceiveClear(void);

void ntSetBaudRate(uint8_t baudRate);

uint8_t ntGetBaudRate(void);

void ntBindDevice(uint8_t deviceIndex);

uint8_t ntCheckDevice(void);

#endif
