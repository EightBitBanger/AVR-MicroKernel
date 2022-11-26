//
// Network interface client card driver

#ifndef __NETWORK_INTERFACE_CARD__
#define __NETWORK_INTERFACE_CARD__

// 16 byte packet structure
//  2 byte address
// 10 byte data block

struct NetworkPacket {
	
	uint8_t start;       // Start byte
	
	uint8_t addr_d[2];   // Destination device address
	uint8_t addr_s[2];   // Source device address
	
	uint8_t data[16];    // Data block
	
	uint8_t stop;        // Stop byte
	
};

#define _PACKET_SIZE__        sizeof(NetworkPacket)

#define _PACKET_START_BYTE__   0x55
#define _PACKET_TYPE_DATA__    0x00
#define _PACKET_STOP_BYTE__    0xaa

#define _NETWORK_WAITSTATE__  4000

void network_packet_copy(NetworkPacket packet_a, NetworkPacket packet_b);

// Send a packet
uint8_t network_send(Device network_device, NetworkPacket& packet_buffer);
uint8_t network_receive(Device network_device, NetworkPacket& buffer);

// Connection
uint8_t network_send_handshake(Device network_device, uint8_t address_low, uint8_t address_high, uint8_t device_type);




struct NetworkInterfaceDriver {
	
	Bus device_bus;
	
	uint32_t device_address;
	
	NetworkInterfaceDriver() {
		
		device_address = 0x00000;
		
		device_bus.waitstate_read  = 10;
		device_bus.waitstate_write = 10;
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
	// TX flag
	void writeTXflag(uint8_t flag) {write(device_address + 0x0a, flag);}
	void readTXflag(uint8_t& flag) {read(device_address + 0x0a, (char&)flag);}
	// RX flag
	void writeRXflag(uint8_t flag) {write(device_address + 0x0b, flag);}
	void readRXflag(uint8_t& flag) {read(device_address + 0x0b, (char&)flag);}
	
	// TX frame buffer
	void writeTXbuffer(uint8_t frame_data) {write(device_address + 0x0c, frame_data);}
	void readTXbuffer(uint8_t& frame_data) {read(device_address + 0x0c, (char&)frame_data);}
	
	// Setup baud rate
	void writeBaudrateFlag(uint8_t flag) {write(device_address + 0x0d, flag);}
	void writeBaudrate(uint8_t baudrate)   {write(device_address + 0x0e, baudrate);}
	void readBaudrate(uint8_t& baudrate)   {read(device_address + 0x0e, (char&)baudrate);}
	
	// RX frame buffer array
	void writeRXbuffer(uint8_t index, uint8_t frame_data) {write(device_address + 0x10 + index, frame_data);}
	void readRXbuffer(uint8_t index, uint8_t& frame_data) {read(device_address + 0x10 + index, (char&)frame_data);}
	
}static networkInterfaceDriver;



//
// Associated command line modules

#include "modules/net.h"           // Network client application
#include "modules/server.h"        // Network server application
#include "modules/router.h"        // Network packet router



// Device Driver entry point
void NetworkInterfaceDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
#ifdef __BOOT_NETWORK_SUPPORT_
	
	if (functionCall == DEVICE_CALL_INITIATE) {
		
		load_device("net",  sizeof("net"), (Module)net_entry_point, DEVICE_TYPE_MODULE);
		load_device("router",  sizeof("router"), (Module)router_entry_point, DEVICE_TYPE_MODULE);
		load_device("server",  sizeof("server"), (Module)server_host_entry_point, DEVICE_TYPE_MODULE);
		
	}
	
#endif
	
	
	if (functionCall == DEVICE_CALL_ADDRESS) {
		WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
		networkInterfaceDriver.device_address = pointer.address;
		return;
	}
	
	if (functionCall == 0x00) {networkInterfaceDriver.writeTXflag(paramA); return;}
	if (functionCall == 0x01) {networkInterfaceDriver.readTXflag(paramA); return;}
	
	if (functionCall == 0x02) {networkInterfaceDriver.writeRXflag(paramA); return;}
	if (functionCall == 0x03) {networkInterfaceDriver.readRXflag(paramA); return;}
	
	if (functionCall == 0x04) {networkInterfaceDriver.writeTXbuffer(paramA); return;}
	if (functionCall == 0x05) {networkInterfaceDriver.readTXbuffer(paramA); return;}
	
	if (functionCall == 0x06) {networkInterfaceDriver.writeBaudrateFlag(paramA); return;}
	if (functionCall == 0x07) {networkInterfaceDriver.writeBaudrate(paramA); return;}
	
	if (functionCall == 0x09) {networkInterfaceDriver.writeRXbuffer(paramA, paramB); return;}
	if (functionCall == 0x0a) {networkInterfaceDriver.readRXbuffer(paramA, paramB); return;}
	
	return;
}



void network_packet_copy(NetworkPacket& packet_a, NetworkPacket& packet_b) {
	
	packet_b.start = _PACKET_START_BYTE__;
	packet_b.addr_d[0] = packet_a.addr_d[0];
	packet_b.addr_d[1] = packet_a.addr_d[1];
	packet_b.addr_s[0] = packet_a.addr_s[0];
	packet_b.addr_s[1] = packet_a.addr_s[1];
	packet_b.data[0]   = packet_a.data[0];
	packet_b.data[1]   = packet_a.data[1];
	packet_b.data[2]   = packet_a.data[2];
	packet_b.data[3]   = packet_a.data[3];
	packet_b.data[4]   = packet_a.data[4];
	packet_b.data[5]   = packet_a.data[5];
	packet_b.data[6]   = packet_a.data[6];
	packet_b.data[7]   = packet_a.data[7];
	packet_b.data[8]   = packet_a.data[8];
	packet_b.data[9]   = packet_a.data[9];
	packet_b.data[10]  = packet_a.data[10];
	packet_b.data[11]  = packet_a.data[11];
	packet_b.data[12]  = packet_a.data[12];
	packet_b.data[13]  = packet_a.data[13];
	packet_b.data[14]  = packet_a.data[14];
	packet_b.data[15]  = packet_a.data[15];
	packet_b.stop = _PACKET_STOP_BYTE__;
	
}



uint8_t network_send(Device network_device, NetworkPacket& packet_buffer) {
	
	uint8_t flag = 0;
	uint8_t byte = 0;
	
	for (uint8_t i=0; i <= _PACKET_SIZE__;) {
		
		networkInterfaceDriver.readTXflag(flag);
		
		if (flag == 0) {
			
			flag = 0xff;
			if (i ==  0) byte = _PACKET_START_BYTE__;
			if (i ==  1) byte = packet_buffer.addr_d[0];
			if (i ==  2) byte = packet_buffer.addr_d[1];
			if (i ==  3) byte = packet_buffer.addr_s[0];
			if (i ==  4) byte = packet_buffer.addr_s[1];
			if (i ==  5) byte = packet_buffer.data[0];
			if (i ==  6) byte = packet_buffer.data[1];
			if (i ==  7) byte = packet_buffer.data[2];
			if (i ==  8) byte = packet_buffer.data[3];
			if (i ==  9) byte = packet_buffer.data[4];
			if (i == 10) byte = packet_buffer.data[5];
			if (i == 11) byte = packet_buffer.data[6];
			if (i == 12) byte = packet_buffer.data[7];
			if (i == 13) byte = packet_buffer.data[8];
			if (i == 14) byte = packet_buffer.data[9];
			if (i == 15) byte = packet_buffer.data[10];
			if (i == 16) byte = packet_buffer.data[11];
			if (i == 17) byte = packet_buffer.data[12];
			if (i == 18) byte = packet_buffer.data[13];
			if (i == 19) byte = packet_buffer.data[14];
			if (i == 20) byte = packet_buffer.data[15];
			if (i == 21) byte = _PACKET_STOP_BYTE__;
			if (i  > 21) break;
			
			// Write data to the TX frame buffer
			networkInterfaceDriver.writeTXbuffer(byte);
			networkInterfaceDriver.writeTXflag(flag);
			
			i++;
			continue;
		}
		
		continue;
	}
	
	return 1;
}


uint8_t network_receive(Device network_device, NetworkPacket& buffer) {
	
	uint8_t flag=0;
	uint8_t count=0;
	
	networkInterfaceDriver.readRXflag(count);
	
	if (count == 0) 
		return 0;
	
	
	// Check network buffer for a complete packet
	for (uint8_t i=0; i < 255; i++) {
		
		uint8_t start_byte=0;
		uint8_t stop_byte=0;
		
		// Check start and stop bytes
		networkInterfaceDriver.readRXbuffer(i, start_byte);
		uint8_t address = i + _PACKET_SIZE__ - 1;
		networkInterfaceDriver.readRXbuffer(address, stop_byte);
		
		if ((start_byte != _PACKET_START_BYTE__) | (stop_byte != _PACKET_STOP_BYTE__)) 
			continue;
		
		buffer.start = _PACKET_START_BYTE__;
		
		// Set the packet addresses
		address = i + 1;  networkInterfaceDriver.readRXbuffer(address, buffer.addr_d[0]);
		address = i + 2;  networkInterfaceDriver.readRXbuffer(address, buffer.addr_d[1]);
		address = i + 3;  networkInterfaceDriver.readRXbuffer(address, buffer.addr_s[0]);
		address = i + 4;  networkInterfaceDriver.readRXbuffer(address, buffer.addr_s[1]);
		
		// Packet data
		for (uint8_t a=0; a < 16; a++) {
			networkInterfaceDriver.readRXbuffer(i + a + 5, buffer.data[a]);
		}
		
		buffer.stop = _PACKET_STOP_BYTE__;
		
		// Clear the old packet
		uint8_t clear_data=0x00;
		for (uint8_t a=i; a < i + _PACKET_SIZE__; a++)
			networkInterfaceDriver.writeRXbuffer(a, clear_data);
		
		// Decrement the RX flag
		networkInterfaceDriver.readRXflag(flag);
		flag -= _PACKET_SIZE__;
		networkInterfaceDriver.writeRXflag(flag);
		
		return 1;
	}
	
	return 0;
}

// Send a handshake to the router
uint8_t network_send_handshake(Device network_device, uint8_t device_type, uint8_t address_low, uint8_t address_high) {
	
	NetworkPacket handshake;
	handshake.start     = _PACKET_START_BYTE__;
	handshake.addr_d[0]  = 0xff; // Send to the router address
	handshake.addr_d[1]  = 0xff;
	handshake.addr_s[0]  = address_low;  // Our device address
	handshake.addr_s[1]  = address_high;
	
	for (uint8_t i=0; i < 16; i++) 
		handshake.data[i] = 0x55;
	
	handshake.data[1]   = device_type; // Indication of device type
	
	handshake.stop      = _PACKET_STOP_BYTE__;
	
	network_send(network_device, handshake);
	
	uint16_t timeout = 40;
	for (uint16_t i=0; i < timeout; i++) {
		
		NetworkPacket rec_packet;
		if (network_receive(network_device, rec_packet) != 0) {
			
			if ((rec_packet.addr_s[0] == 0xff) &
				(rec_packet.addr_s[1] == 0xff) &
				(rec_packet.data[0] == 0x55) &
				(rec_packet.data[1] == 0xff) &
				(rec_packet.data[2] == 0x00)) {
				return 1;
			}
			
		}
		
		_delay_ms(100);
		
		if (i == (timeout-1)) 
			return 0;
		
		continue;
	}
	
	return 1;
}

#endif


