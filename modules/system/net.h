//
// Network diagnostic tool

#define _PACKET_START_BYTE__   0x55
#define _PACKET_TYPE_DATA__    0x00
#define _PACKET_STOP_BYTE__    0xaa

#define _NETWORK_WAITSTATE__  4000

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "net"

struct ModuleLoaderNet {
	
	uint8_t waitstate;
	uint8_t baudrate;
	uint8_t byte;
	
	ModuleLoaderNet() {
		
		waitstate = 0;
		baudrate  = 7; // Start at 56k baud
		byte      = 0;
	 	
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (void(*)())net_entry_point, DEVICE_TYPE_MODULE);
	}
}static netModuleLoader;


// Send a packet
uint8_t network_send(Device networkDevice, uint8_t packet[]);


void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	uint8_t  flag       = 0xff;
	uint8_t  flagState  = 0xff;
	uint8_t  flagOld    = 0xff;
	uint8_t  byte       = 0x00;
	
	uint32_t timeout    = 230000;
	uint32_t counter    = 0;
	
	Device networkDevice;
	
	networkDevice = (Device)get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__));
	if (networkDevice == 0) return;
	
	// Get the parameters from the keyboard string
	uint8_t param0  = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1  = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	uint8_t param2  = console.keyboard_string[sizeof(__MODULE_NAME_) + 4];
	
	
	//
	// Data thrash test
	if (param0 == 't') {
		
		if ((param1 >= '0') & (param1 <= '9') | (param1 == 'x')) {
			
			counter = (param1 - '0') * 8;
			
			uint8_t packet[] = {_PACKET_START_BYTE__, _PACKET_TYPE_DATA__, netModuleLoader.byte, _PACKET_STOP_BYTE__};
			
			for (uint8_t i=0; i < counter; i++) {
				
				network_send(networkDevice, packet);
				
				if (param1 == 'x') i = 0;
			}
			
		}
		
		return;
	}
	
	
	//
	// Set the transfer waitstate
	if (param0 == 'w') {
		
		if ((param1 >= '0') & (param1 <= '9')) {
			netModuleLoader.waitstate = (param1 - '0');
		}
		
		
		char msg_string[] = "wait-state";
		console.print(msg_string, sizeof(msg_string));
		console.printSpace();
		
		console.printInt( netModuleLoader.waitstate );
		
		console.printLn();
		
		return;
	}
	
	
	//
	// Send a byte
	if (param0 == 's') {
		
		uint8_t packet[] = {_PACKET_START_BYTE__, _PACKET_TYPE_DATA__, param1, _PACKET_STOP_BYTE__};
		
		network_send(networkDevice, packet);
		
		return;
	}
	
	
	//
	// Check the receiver
	if (param0 == 'r') {
		
		// Check continuous receiver mode
		if (param1 == '1') {
			
			console.clearBuffer();
			console.setCursorPosition(0, 0);
			console.cursorLine = 0;
			console.cursorPos = 0;
			
			
			while(1) {
				
				// Check the receive flag
				call_extern(networkDevice, 0x03, flag);
				if (flag > 0) {
					
					for (uint8_t i=0; i < flag; i++) {
						
						// Read RX buffer data
						call_extern(networkDevice, 0x0a, i, byte);
						
						// Print the received data
						console.printChar(byte);
						console.updateCursorPosition();
						
					}
					
					// Zero the RX flag
					flag = 0;
					call_extern(networkDevice, 0x02, flag);
					
				}
				
			}
			
		} else {
			
			// Check the receive flag
			call_extern(networkDevice, 0x03, flag);
			if (flag > 0) {
				
				for (uint8_t i=0; i < flag; i++) {
					
					// Read RX buffer data
					call_extern(networkDevice, 0x0a, i, byte);
					
					// Print the received data
					console.printChar(byte);
					
				}
				
				console.printLn();
			} else {
				
				char msg_string[] = "Receiver empty";
				console.print(msg_string, sizeof(msg_string));
				console.printLn();
				
			}
			
		}
		
		return;
	}
	
	
	//
	// Clear RX flag
	if (param0 == 'c') {
		flag = 0;
		call_extern(networkDevice, 0x02, flag);
		
		char msg_string[] = "Receiver cleared";
		console.print(msg_string, sizeof(msg_string));
		console.printLn();
		
		return;
	}
	
	
	//
	// Set baud rate
	if (param0 == 'b') {
		
		char msg_string[] = "baud ";
		console.print(msg_string, sizeof(msg_string));
		
		if ((param1 >= '0') & (param1 <= '9')) {
			
			netModuleLoader.baudrate = param1 - '0';
			
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
		byte = 0xff;
		call_extern(networkDevice, 0x07, netModuleLoader.baudrate);  // Set baud rate register
		call_extern(networkDevice, 0x06, byte);                      // Apply the baud rate flag
		
		return;
	}
	
	
	//
	// Send a probing signal
	if (param0 == 'p') {
		
		// Clear RX flag
		byte = 0x00;
		call_extern(networkDevice, 0x02, byte);
		
		// Clear packet
		uint8_t data = 0x00;
		for (uint8_t a=0; a <= 10; a++)
			call_extern(networkDevice, 0x09, a, data);
		
		// Send ping packet
		uint8_t ping_packet[] = {0x55, 0x55, 0x00, 0xaa};
		network_send(networkDevice, ping_packet);
		
		// Check return packets
		for (counter=0; counter < timeout; counter++) {
			
			_delay_us(1);
			
			// Check the receive flag
			uint8_t RXflag;
			call_extern(networkDevice, 0x03, RXflag);
			
			if (RXflag > 0) {
				
				// Read RX buffer data
				uint8_t address = 0x00;
				call_extern(networkDevice, 0x0a, address, byte);
				
				// Check return packets
				for (uint8_t a=0; a <= RXflag; a++) {
					
					// Verify the received data is a return probing signal
					if (byte == 0x55) {
						
						// Clear RX flag
						byte = 0x00;
						call_extern(networkDevice, 0x02, byte);
						
						// Clear packet
						uint8_t data = 0x00;
						for (uint8_t b=a; b <= 3; b++)
							call_extern(networkDevice, 0x09, b, data);
						
						console.printInt(counter);
						console.printSpace();
						console.printChar('u');
						console.printChar('s');
						console.printLn();
						
						return;
					}
					
				}
				
			}
			
		}
		
		console.print("Timed out", sizeof("Timed out"));
		console.printLn();
		
		return;
	}
	
	//
	// Set the data testing byte
	if (param0 == 'd') {
		
		if ((param1 >= '0') & (param1 <= '9')) netModuleLoader.byte = param1;
		if ((param1 >= 'a') & (param1 <= 'z')) netModuleLoader.byte = param1;
		if (param1 == 0x20) netModuleLoader.byte = 0x20;
		
		char msg_string[] = "Data ";
		console.print(msg_string, sizeof(msg_string));
		console.printSpace();
		console.printChar(netModuleLoader.byte);
		console.printLn();
		
		return;
	}
	
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		char help_line_a[] = "net s b - Send";
		char help_line_b[] = "net r   - Receive";
		char help_line_c[] = "net c   - Clear";
		
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

#undef __MODULE_NAME_


uint8_t network_send(Device network_device, uint8_t packet_buffer[]) {
	
	uint8_t flag = 0;
	uint8_t byte = 0;
	
	for (uint8_t i=0; i <= 3;) {
		
		call_extern(network_device, 0x01, flag);
		if (flag == 0) {
			
			flag = 0xff;
			byte = packet_buffer[i];
			
			// Write data to the TX frame buffer
			call_extern(network_device, 0x04, byte);
			call_extern(network_device, 0x00, flag);
			
			i++;
			
		}
		
		// Transfer waitstate
		for (uint32_t t=0; t < netModuleLoader.waitstate; t++)
			_delay_us(_NETWORK_WAITSTATE__);
		
	}
	
	return 1;
}


uint8_t network_receive(Device network_device, uint8_t packet_buffer[]) {
	
	uint8_t flag=0;
	uint8_t byte=0;
	uint8_t count=0;
	
	uint8_t start_byte=0;
	uint8_t type_byte=0;
	uint8_t data_byte=0;
	uint8_t end_byte=0;
	
	uint8_t address=0;
	
	call_extern(network_device, 0x03, count);
	if (count > 0) {
		
		for (uint8_t i=0; i < count; i++) {
			
			// Extract and check the start byte
			call_extern(network_device, 0x0a, i, start_byte);
			
			if (start_byte == 0x55) {
				
				// Extract and check the end byte
				address = i + 3;
				call_extern(network_device, 0x0a, address, end_byte);
				
				if (end_byte == 0xaa) {
					
					// Extract the packet type
					address = i + 1;
					call_extern(network_device, 0x0a, address, type_byte);
					
					// Extract the packet data
					address = i + 2;
					call_extern(network_device, 0x0a, address, data_byte);
					
					// Reassemble the packet
					packet_buffer[0] = start_byte;
					packet_buffer[1] = type_byte;
					packet_buffer[2] = data_byte;
					packet_buffer[3] = end_byte;
					
					// Clear the packet
					byte = 0x00;
					for (uint8_t a=i; a <= (i + 3); a++)
						call_extern(network_device, 0x09, a, byte);
					
					// Zero the RX flag
					call_extern(network_device, 0x03, flag);
					if (flag >= 200) {
						flag = 0;
						call_extern(network_device, 0x02, flag);
					}
					
					return 1;
				}
				
			}
			
		}
		
	}
	
	return 0;
}

