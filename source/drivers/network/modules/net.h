//
// Network diagnostic tool

char msg_timed_out[]       = "Timed out...";
char msg_connecting[]      = "Connecting...";
char msg_receiver_empty[]  = "Receiver empty";
char msg_baud_rate[]       = "baud ";

struct ModuleLoaderNet {
	
	uint8_t baudrate;
	uint8_t byte;
	
	uint8_t addr_destination[2];
	uint8_t addr_source[2];
	
	ModuleLoaderNet() {
		
		baudrate  = 2; // Start at 2400 baud
		
		byte=0;
		
		addr_destination[0] = 0;
		addr_destination[1] = 0;
		
		addr_source[0] = 44;
		addr_source[1] = 15;
		
	}
}static netModuleLoader;

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device networkDevice = (Device)get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__));
	if (networkDevice == 0) return;
	
	// Get the parameters from the keyboard string
	uint8_t param0  = console.keyboard_string[sizeof("net")];
	uint8_t param1  = console.keyboard_string[sizeof("net") + 1];
	uint8_t param2  = console.keyboard_string[sizeof("net") + 3];
	uint8_t param3  = console.keyboard_string[sizeof("net") + 4];
	uint8_t param4  = console.keyboard_string[sizeof("net") + 6];
	uint8_t param5  = console.keyboard_string[sizeof("net") + 7];
	
	
	
	//
	// Connect to a host
	if ((param0 == '-') & (param1 == 'c')) {
		
		// Set the network address
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
			netModuleLoader.addr_destination[0] = string_get_hex_char(addr_first);
			
			char addr_second[2];
			addr_second[1] = param4;
			addr_second[0] = param5;
			netModuleLoader.addr_destination[1] = string_get_hex_char(addr_second);
			
		}
		
		// Check zero address
		if ((netModuleLoader.addr_destination[0] == 0x00) & (netModuleLoader.addr_destination[1] == 0x00)) {
			char msg_address_invalid[] = "Invalid address";
			console.print(msg_address_invalid, sizeof(msg_address_invalid));
			console.printLn();
			return;
		}
		
		console.print(msg_connecting, sizeof(msg_connecting));
		console.printLn();
		console.printSpace();
		console.printSpace();
		
		console.printHex(netModuleLoader.addr_destination[0]);
		console.printChar(':');
		console.printHex(netModuleLoader.addr_destination[1]);
		
		console.printLn();
		
		// Handshake as a client device
		if (network_send_handshake(networkDevice, 0x00, netModuleLoader.addr_source[0], netModuleLoader.addr_source[1]) == 0) {
			console.print(msg_timed_out, sizeof(msg_timed_out));
			console.printLn();
			return;
		}
		
		// Send a connection request to a server
		NetworkPacket packet;
		packet.start  = _PACKET_START_BYTE__;
		packet.addr_d[0] = netModuleLoader.addr_destination[0];
		packet.addr_d[1] = netModuleLoader.addr_destination[1];
		packet.addr_s[0] = netModuleLoader.addr_source[0];
		packet.addr_s[1] = netModuleLoader.addr_source[1];
		packet.data[0] = 0xcc; // Request to connect to a host
		packet.data[1] = 0x00;
		packet.data[2] = 0x00;
		packet.data[3] = 0x00;
		packet.data[4] = 0x00;
		packet.data[5] = 0x00;
		packet.data[6] = 0x00;
		packet.data[7] = 0x00;
		packet.data[8] = 0x00;
		packet.data[9] = 0x00;
		packet.data[10] = 0x00;
		packet.data[11] = 0x00;
		packet.data[12] = 0x00;
		packet.data[13] = 0x00;
		packet.data[14] = 0x00;
		packet.data[15] = 0x00;
		packet.stop   = _PACKET_STOP_BYTE__;
		
		network_send(networkDevice, packet);
		
		NetworkPacket r_packet;
		for (uint16_t i=0; i < 8000; i++) {
			
			if (network_receive(networkDevice, r_packet) == 1) {
				
				// Check packet address
				if ((r_packet.addr_d[0] == netModuleLoader.addr_source[0]) & 
				    (r_packet.addr_d[1] == netModuleLoader.addr_source[1])) {
					
					// Print packet return information
					for (uint8_t a=0; a < 16; a++)
						console.printChar( r_packet.data[a] );
					
					console.printLn();
				}
				
				if (param2 == 'x') {
					i=0;
					network_send(networkDevice, packet);
					continue;
				}
				
				return;
			}
			
			_delay_us(1000);
		}
		
		console.print(msg_timed_out, sizeof(msg_timed_out));
		console.printLn();
		
		return;
	}
	
	
	
	
	//
	// Request a service from a host
	if ((param0 == '-') & (param1 == 's')) {
		
		// Handshake as a client device
		if (network_send_handshake(networkDevice, 0x00, netModuleLoader.addr_source[0], netModuleLoader.addr_source[1]) == 0) {
			console.print(msg_timed_out, sizeof(msg_timed_out));
			console.printLn();
			return;
		}
		
		// Assemble the service request packet
		uint8_t name_str[16]; 
		uint8_t function[2];
		function[0] = console.keyboard_string[sizeof("net") + 3];
		function[1] = console.keyboard_string[sizeof("net") + 4];
		for (uint8_t a=0; a < 14; a++) 
			name_str[a] = console.keyboard_string[sizeof("net") + 6 + a];
		
		NetworkPacket request;
		request.start     = _PACKET_START_BYTE__;
		request.addr_d[0] = netModuleLoader.addr_destination[0];
		request.addr_d[1] = netModuleLoader.addr_destination[1];
		request.addr_s[0] = netModuleLoader.addr_source[0];
		request.addr_s[1] = netModuleLoader.addr_source[1];
		request.data[0]   = 0xcd;   // Service request
		request.data[1]   = 0x00;   // Function
		
		for (uint8_t a=0; a < 14; a++)    // Data string
			request.data[2]   = name_str[0];
		
		request.data[2]   = name_str[0];  // Data string
		request.data[3]   = name_str[1];
		request.data[4]   = name_str[2];
		request.data[5]   = name_str[3];
		request.data[6]   = name_str[4];
		request.data[7]   = name_str[5];
		request.data[8]   = name_str[6];
		request.data[9]   = name_str[7];
		request.data[10]   = name_str[8];
		request.data[11]   = name_str[9];
		request.data[12]   = name_str[10];
		request.data[13]   = name_str[11];
		request.data[14]   = name_str[12];
		request.data[15]   = name_str[13];
		request.stop      = _PACKET_STOP_BYTE__;
		
		// Setup packet function
		if ((function[0] == 'm') & (function[1] == 'k')) request.data[1] = 0xc0; // Create a file
		if ((function[0] == 'r') & (function[1] == 'm')) request.data[1] = 0xc1; // Delete a file
		
		network_send(networkDevice, request);
		
		for (uint16_t i=0; i < 1000; i++) {
			
			NetworkPacket rec_packet;
			if (network_receive(networkDevice, rec_packet) != 0) {
				if ((rec_packet.addr_d[0] == netModuleLoader.addr_source[0]) &
					(rec_packet.addr_d[1] == netModuleLoader.addr_source[1])) {
					
					for (uint8_t a=0; a < 16; a++) 
						console.printChar(rec_packet.data[a]);
					
					console.printLn();
				}
				break;
			}
			
			_delay_us(1000);
			
			if (i == 900)
				return;
			
			continue;
		}
		
		
		return;
	}
	
	
	//
	// Set baud rate
	if ((param0 == '-') & (param1 == 'b')) {
		
		console.print(msg_baud_rate, sizeof(msg_baud_rate));
		
		if ((param2 >= '0') & (param2 <= '9')) {
			
			netModuleLoader.baudrate = (param2 - '0');
			
		}
		
		switch (netModuleLoader.baudrate) {
			
			case 0: {console.printInt(900);  console.printLn(); break;}
			case 1: {console.printInt(1200); console.printLn(); break;}
			case 2: {console.printInt(2400); console.printLn(); break;}
			case 3: {console.printInt(4800); console.printLn(); break;}
			case 4: {console.printInt(9600); console.printLn(); break;}
			case 5: {console.printInt(28);   console.printChar('k'); console.printLn(); break;}
			case 6: {console.printInt(33);   console.printChar('k'); console.printLn(); break;}
			case 7: {console.printInt(56);   console.printChar('k'); console.printLn(); break;}
			case 8: {console.printInt(76);   console.printChar('k'); console.printLn(); break;}
			case 9: {console.printInt(125);  console.printChar('k'); console.printLn(); break;}
			
		}
		
		// Set the baud rate
		uint8_t byte = 0xff;
		call_extern(networkDevice, 0x07, netModuleLoader.baudrate);  // Set baud rate register
		call_extern(networkDevice, 0x06, byte);                      // Apply the baud rate flag
		
		return;
	}
	
	
	//
	// Send a handshake ping packet
	if ((param0 == '-') & (param1 == 'p')) {
		
		// Handshake as a client device
		if (network_send_handshake(networkDevice, 0x80, netModuleLoader.addr_destination[0], netModuleLoader.addr_destination[1]) == 0) {
			console.print(msg_timed_out, sizeof(msg_timed_out));
			console.printLn();
			return;
		}
		
		char msg_host_active[] = "Connection is active";
		console.print(msg_host_active, sizeof(msg_host_active));
		console.printLn();
		
		return;
	}
	
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		char help_line_a[] = "net -c  Connect";
		char help_line_b[] = "net -s  Send data";
		char help_line_c[] = "net -b  Set baud";
		
		console.print(help_line_a, sizeof(help_line_a));
		console.printLn();
		console.print(help_line_b, sizeof(help_line_b));
		console.printLn();
		console.print(help_line_c, sizeof(help_line_c));
		console.printLn();
		
		return;
	}
	
	return;
}


