//
// Router program

//#define _DISPLAY_DEBUG_LOG__

struct PacketAddress {
	uint8_t addr_low;
	uint8_t addr_high;
};

struct RoutingTable {
	
	uint8_t device_state[5];
	uint8_t device_type[5];
	
	PacketAddress network_address[5];
	
	RoutingTable() {
		
		for (uint8_t i=0; i < 5; i++) {
			device_state[i] = 0x00;
			device_type[i] = 0x00;
			network_address[i].addr_high = 0x00;
			network_address[i].addr_low = 0x00;
		}
		
	}
	
}routing_table;


void router_entry_point(void);
void check_subnet_routes(Device networkDevice);
void get_available_network_hardware(void);


void router_entry_point(void) {
	
	Device networkDevice;
	networkDevice = (Device)get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__));
	
	if (networkDevice == 0) 
		return;
	
	console.setCursorPosition(0, 0);
	console.clearBuffer();
	
	// Check for hardware level network cards
	get_available_network_hardware();
	
	// Check the network cards for subnet connections
	check_subnet_routes(networkDevice);
	
	console.clearBuffer();
	console.setCursorPosition(0, 0);
	
	uint32_t current_network_card = 0;
	
	while (1) {
		
		// Check network cards
		current_network_card++;
		if (current_network_card > 4) 
			current_network_card = 0;
		
		// Skip other cards
		if (routing_table.device_state[current_network_card] == 0x00) 
			continue;
		networkInterfaceDriver.device_address = 0x40000 + (0x10000 * current_network_card);
		
		NetworkPacket packet;
		if (network_receive(networkDevice, packet) == 0) 
			continue;
		
#ifdef _DISPLAY_DEBUG_LOG__
		
		// Console print some details about the packet
		console.printInt(packet.addr_s[0]); console.printChar('.'); console.printInt(packet.addr_s[1]);
		console.printSpace(); console.printChar('>'); console.printSpace();
		console.printInt(packet.addr_d[0]); console.printChar('.'); console.printInt(packet.addr_d[1]);
		console.printLn();
		
#endif
		
		//
		// Router handshake packet
		if ((packet.addr_d[0] == 0xff) & (packet.addr_d[1] == 0xff)) {
			
			// Check if the source address already
			// exists in the routing table and remove
			// it if its found
			for (uint8_t i=0; i < 5; i++) {
				if ((routing_table.network_address[i].addr_low == packet.addr_s[0]) &
					(routing_table.network_address[i].addr_high == packet.addr_s[1])) {
					routing_table.network_address[i].addr_low = 0x00;
					routing_table.network_address[i].addr_high = 0x00;
				}
			}
			
			// Add the source address to the routing table
			routing_table.device_type[current_network_card] = packet.data[1];
			routing_table.network_address[current_network_card].addr_low  = packet.addr_s[0];
			routing_table.network_address[current_network_card].addr_high = packet.addr_s[1];
			
			// Return packet
			NetworkPacket r_packet;
			r_packet.start   = _PACKET_START_BYTE__;
			r_packet.addr_d[0]  = packet.addr_s[0];
			r_packet.addr_d[1]  = packet.addr_s[1];
			r_packet.addr_s[0]  = 0xff;
			r_packet.addr_s[1]  = 0xff;
			
			for (uint8_t i=0; i < 16; i++) 
				r_packet.data[i] = 0x55;
			
			r_packet.data[1] = 0xff; // Indicate we are a router
			r_packet.data[2] = 0x00; // And we acknowledge your connection
			
			r_packet.stop    = _PACKET_STOP_BYTE__;
			
			// Return the client handshake
			if ((packet.data[0] != 0x55) | 
				(packet.data[1] != 0x00)) {
				network_send(networkDevice, r_packet);
				continue;
			}
			// Return the server handshake
			if ((packet.data[0] != 0x55) |
				(packet.data[1] != 0x80)) {
				network_send(networkDevice, r_packet);
				continue;
			}
			// Return the subnet handshake
			if ((packet.data[0] != 0x55) | 
				(packet.data[1] != 0xff)) {
				network_send(networkDevice, r_packet);
				continue;
			}
			continue;
		}
		
		//
		// Check the routing table for the destination address
		uint8_t dest_found=0;
		for (uint8_t i=0; i < 5; i++) {
			
			if ((routing_table.network_address[i].addr_low  == packet.addr_d[0]) & 
			    (routing_table.network_address[i].addr_high == packet.addr_d[1])) {
				
				// Send the packet to the destination address
				networkInterfaceDriver.device_address = 0x40000 + (0x10000 * i);
				
				network_send(networkDevice, packet);
				
				dest_found = 1;
				break;
			}
			
			
		}
		
		// If the address is not on the routing table 
		// it will be sent to to the default gateway.
		// The default gateway will be the first
		// network card detected on the bus.
		if (dest_found == 1)
			continue;
		
		// Send the packet to the default gateway(s)
		for (uint8_t i=0; i < 5; i++) {
			
			if (routing_table.device_type[i] == 0xff) {
				
				// Dont send back to the sender
				if (i == current_network_card) 
					continue;
				
				// Send the packet to the destination address
				networkInterfaceDriver.device_address = 0x40000 + (0x10000 * i);
				network_send(networkDevice, packet);
				
			}
			
		}
		
		continue;
	}
	
	return;
}


void get_available_network_hardware(void) {
	
	HardwareInformation hInfo;
	
	Bus bus;
	bus.waitstate_read  = 20;
	bus.waitstate_write = 20;
	
	uint32_t device_address;
	
	for (uint8_t i=0; i < 5; i++) {
		
		device_address = 0x40000 + (0x10000 * i);
		get_hardware_info(device_address, bus, hInfo);
		
		// Network cards only
		if (hInfo.device_id != 0x14)
			continue;
		
		routing_table.device_state[i] = 0xff;
		
		console.printInt(i + 1);
		console.printSpace();
		console.printHex(hInfo.device_id);
		console.printSpace();
		console.print(hInfo.device_name, 10);
		console.printLn();
		
		// Set network device address
		networkInterfaceDriver.device_address = device_address;
		
		// Setup baud rates
		uint8_t baud = 0xff;
		uint8_t byte = 0xff;
		networkInterfaceDriver.writeBaudrate(2);          // 2400 baud
		networkInterfaceDriver.writeBaudrateFlag(0xff);
		
	}
	return;
}

void check_subnet_routes(Device networkDevice) {
	
	// Subnet request packet
	NetworkPacket packet;
	packet.start     = _PACKET_START_BYTE__;
	packet.addr_d[0]  = 0xff; // Send to the router
	packet.addr_d[1]  = 0xff;
	packet.addr_s[0]  = 0xff; // From a router
	packet.addr_s[1]  = 0xff;
	
	for (uint8_t i=0; i < 16; i++)
		packet.data[i] = 0x55;
	
	packet.data[1]   = 0xff; // Indicate we are a router
	packet.data[2]   = 0x00;
	
	packet.stop      = _PACKET_STOP_BYTE__;
	
	// Send out subnet request packets
	for (uint8_t i=0; i < 5; i++) {
		
		if (routing_table.device_state[i] != 0xff) 
			continue;
		
		networkInterfaceDriver.device_address = 0x40000 + (0x10000 * i);
		
		network_send(networkDevice, packet);
		
		continue;
	}
	
	// Check for a subnet response
	uint8_t count=0;
	for (uint16_t i=0; i < 800; i++) {
		
		if (count > 5) {count=0;} else {count++;}
		networkInterfaceDriver.device_address = 0x40000 + (0x10000 * count);
		
		NetworkPacket rec_packet;
		if (network_receive(networkDevice, rec_packet) != 0) {
			 
			if ((rec_packet.data[0] == 0x55) &
			(rec_packet.data[1] == 0xff) & // Return packet is another router
			(rec_packet.data[2] == 0x00)) {
				routing_table.device_state[count] = 0xff; // Add the router to the routing table
				routing_table.device_type[count] = 0xff;
				continue;
			}
			
		}
		
		_delay_us(100);
		continue;
	}
	
	return;
}

