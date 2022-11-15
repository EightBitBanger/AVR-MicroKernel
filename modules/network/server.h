#ifndef _SERVER_HOST_APPLICATION__
#define _SERVER_HOST_APPLICATION__

#define _SERVER_ADDRESS_LOW__   200
#define _SERVER_ADDRESS_HIGH__   80

uint8_t addr_destination[2] = {0xc8, 0x14};

void host_service_call(uint8_t* packet_data);

void server_host_entry_point(void) {
	
	Device networkDevice = (Device)get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__));
	if (networkDevice == 0) {
		console.print(error_driver_error, sizeof(error_driver_error));
		return;
	}
	
	// Get the parameters from the keyboard string
	uint8_t param0  = console.keyboard_string[sizeof("server")];
	uint8_t param1  = console.keyboard_string[sizeof("server") + 1];
	uint8_t param2  = console.keyboard_string[sizeof("server") + 3];
	uint8_t param3  = console.keyboard_string[sizeof("server") + 4];
	uint8_t param4  = console.keyboard_string[sizeof("server") + 6];
	uint8_t param5  = console.keyboard_string[sizeof("server") + 7];
	
	
	//
	// Enter server mode
	if ((param0 != '-') | (param1 != 'c'))
		return;
	
	if (((param2 >= '0') & (param2 <= '9')) |
		((param2 >= 'a') & (param2 <= 'f')) |
		((param3 >= '0') & (param3 <= '9')) |
		((param3 >= 'a') & (param3 <= 'f')) |
		((param4 >= '0') & (param4 <= '9')) |
		((param4 >= 'a') & (param4 <= 'f')) |
		((param5 >= '0') & (param5 <= '9')) |
		((param5 >= 'a') & (param5 <= 'f'))) {
		
		char addr_first[2];
		addr_first[1] = param2;
		addr_first[0] = param3;
		addr_destination[0] = string_get_hex_char(addr_first);
		
		char addr_second[2];
		addr_second[1] = param4;
		addr_second[0] = param5;
		addr_destination[1] = string_get_hex_char(addr_second);
		
	}
	
	console.print(msg_connecting, sizeof(msg_connecting));
	console.printLn();
	
	// Handshake as a server device
	if (network_send_handshake(networkDevice, 0x80, addr_destination[0], addr_destination[1]) == 0) {
		console.print(msg_timed_out, sizeof(msg_timed_out));
		console.printLn();
		return;
	}
	
	console.clearBuffer();
	console.setCursorBlinkRate(0);
	console.setCursorPosition(0, 0);
	
	char msg_host[] = "Host address";
	console.print(msg_host, sizeof(msg_host));
	console.printLn();
	
	console.printSpace();
	
	console.printInt(addr_destination[0]);
	console.printChar('.');
	console.printInt(addr_destination[1]);
	
	console.printSpace();
	console.printSpace();
	console.printSpace();
	
	console.printHex(addr_destination[0]);
	console.printChar(':');
	console.printHex(addr_destination[1]);
	console.printSpace();
	
	console.setCursorPosition(3, 0);
	
	uint8_t is_active = 1;
	while(is_active) {
		
		NetworkPacket packet;
		if (network_receive(networkDevice, packet) == 0)
			continue;
		
		// Check packet address
		if ((packet.addr_d[0] != addr_destination[0]) | (packet.addr_d[1] != addr_destination[1]))
			continue;
		
		// Return packet
		NetworkPacket r_packet;
		r_packet.start = _PACKET_START_BYTE__;
		r_packet.addr_d[0] = packet.addr_s[0];
		r_packet.addr_d[1] = packet.addr_s[1];
		r_packet.addr_s[0] = packet.addr_d[0];
		r_packet.addr_s[1] = packet.addr_d[1];
		r_packet.stop = _PACKET_STOP_BYTE__;
		
		
		//
		// Host connection request
		if (packet.data[0] == 0xcc) {
			
			char blank_char=' ';
			for (uint8_t i=0; i < 19; i++)
				console.printChar(blank_char);
			console.setCursorPosition(3, 0);
			
			console.printHex(packet.addr_s[0]);
			console.printChar(':');
			console.printHex(packet.addr_s[1]);
			console.printSpace();
			console.printSpace();
			
			char msg_connection_request[] = "conn_rqst";
			console.print(msg_connection_request, sizeof(msg_connection_request));
			
			// Send back a confirm packet
			for (uint8_t i=0; i < 16; i++)
				r_packet.data[i] = 0x20;
			
			char data_name[] = "srv.host.active";
			for (uint8_t i=0; i < sizeof(data_name)-1; i++)
				r_packet.data[i] = data_name[i];
			
			network_send(networkDevice, r_packet);
		}
		
		
		//
		// Host service request
		if (packet.data[0] == 0xcd) {
			
			char blank_char=' ';
			for (uint8_t i=0; i < 19; i++)
				console.printChar(blank_char);
			console.setCursorPosition(3, 0);
			
			console.printHex(packet.addr_s[0]);
			console.printChar(':');
			console.printHex(packet.addr_s[1]);
			console.printSpace();
			console.printSpace();
			
			char msg_service_request[] = "srvc_rqst_";
			
			console.print(msg_service_request, sizeof(msg_service_request));
			console.printHex(packet.data[1]);
			
			host_service_call(packet.data);
			
			for (uint8_t i=0; i < 16; i++) 
				r_packet.data[i] = packet.data[i];
			
			network_send(networkDevice, r_packet);
		}
		
		console.setCursorPosition(3, 0);
		
		continue;
	}
	
	return;
}

//
// Service routines

void host_service_call(uint8_t* packet_data) {
	
	// Create file service
	if (packet_data[1] == 0xc0) {
		
		// Get the requested filename
		char name_string[16];
		for (uint8_t i=0; i < 16 - 2; i++) 
			name_string[i] = packet_data[i + 2];
		
		for (uint8_t i=0; i < 16; i++)
			packet_data[i] = 0x20;
		
		// Return data
		char data_name[] = "srv.mk";
		for (uint8_t i=0; i < sizeof(data_name)-1; i++) 
			packet_data[i] = data_name[i];
		
		uint8_t attribute[]={' ','r','w',' '};
		
		if (file_open(name_string) == 1) {
			char data_error[] = ".error";
			for (uint8_t a=0; a < sizeof(data_error)-1; a++)
				packet_data[ sizeof(data_name) - 1 + a ] = data_error[a];
			return;
		} else {
			char data_success[] = ".success";
			for (uint8_t a=0; a < sizeof(data_success)-1; a++)
				packet_data[ sizeof(data_name) - 1 + a ] = data_success[a];
		}
		
		//
		// Execute requested function
		if (file_create(name_string, 128, attribute) == 1) 
			return;
		
	}
	
	// Delete file service
	if (packet_data[1] == 0xc1) {
		
		// Get the requested filename
		char name_string[16];
		for (uint8_t i=0; i < 16 - 2; i++)
			name_string[i] = packet_data[i + 2];
		
		for (uint8_t i=0; i < 16; i++)
			packet_data[i] = 0x20;
		
		// Return data
		char data_name[] = "srv.rm";
		for (uint8_t i=0; i < sizeof(data_name)-1; i++)
			packet_data[i] = data_name[i];
		
		if (file_open(name_string) == 0) {
			char data_error[] = ".error";
			for (uint8_t a=0; a < sizeof(data_error)-1; a++)
				packet_data[ sizeof(data_name) - 1 + a ] = data_error[a];
			return;
		} else {
			char data_success[] = ".success";
			for (uint8_t a=0; a < sizeof(data_success)-1; a++)
				packet_data[ sizeof(data_name) - 1 + a ] = data_success[a];
		}
		
		//
		// Execute requested function
		if (file_delete(name_string) == 1)
			return;
		
	}
	
	// Return unknown service request
	for (uint8_t i=0; i < 16; i++)
		packet_data[i] = 0x20;
	
	char data_name[] = "srv.unknown";
	for (uint8_t i=0; i < sizeof(data_name)-1; i++)
		packet_data[i] = data_name[i];
	
	return;
}










#endif


