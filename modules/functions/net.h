//
// Network diagnostic tool


//		Baud rates based on 20.0 mhz
//	
//	Bit Rate	UBRR (dec)	    UBRR (hex)		Actual  Bit Rate	   Error %
//	
//	300			4166			0x1046			299.98					  0%
//	600			2082			0x0822			600.1					  0%
//	900			1388			0x056C			899.93					  0%
//	1200		1041			0x0411			1199.62					  0%
//	1800		693				0x02B5			1801.15					0.1%
//	2400		520				0x0208			2399.23					  0%
//	3600		346				0x015A			3602.31					0.1%
//	4800		259				0x0103			4807.69					0.2%
//	9600		129				0x0081			9615.38					0.2%
//	14.4K		 86				0x0056			14.368 K			   -0.2%
//	19.2K		 64				0x0040			19.231 K				0.2%
//	28.8K		 42				0x002A			29.07 K					0.9%
//	33.6K		 36				0x0024			33.784 K				0.5%

#define _PACKET_START_BYTE__   0x55
#define _PACKET_TYPE_DATA__    0x00
#define _PACKET_STOP_BYTE__    0xaa

#define _NETWORK_WAITSTATE__  1200

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "net"

struct ModuleLoaderNet {
	
	uint32_t waitstate;
	uint8_t byte;
	
	ModuleLoaderNet() {
		
		waitstate = 40;
		byte      = 0;
	 	
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)net_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static netModuleLoader;


// Send a packet
uint8_t network_send_packet(Device networkDevice, uint8_t packet[]);


void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	uint8_t  flag       = 0xff;
	uint8_t  flagState  = 0xff;
	uint8_t  flagOld    = 0xff;
	uint8_t  byte       = 0x00;
	
	uint32_t timeout    = 245000;
	uint32_t counter    = 0;
	
	Device networkDevice;
	
	if (get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), networkDevice) == 0) return;
	
	// Get the parameter from the keyboard string
	uint8_t param0  = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1  = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	uint8_t param2  = console.keyboard_string[sizeof(__MODULE_NAME_) + 4];
	
	
	//
	// Data thrash test
	if (param0 == 't') {
		
		if ((param1 >= '0') & (param1 <= '9')) {
			
			counter = (param1 - '0') * 8;
			
			for (uint8_t i=0; i < counter; i++) {
				
				uint8_t packet[] = {_PACKET_START_BYTE__, _PACKET_TYPE_DATA__, netModuleLoader.byte, _PACKET_STOP_BYTE__};
				network_send_packet(networkDevice, packet);
				
			}
			
		}
		
		return;
	}
	
	
	//
	// Set the transfer waitstate
	if (param0 == 'w') {
		
		if ((param1 > '0') & (param1 <= '9')) {
			netModuleLoader.waitstate = (param1 - '0') * 2;
		} else if (param1 == '0') {
			netModuleLoader.waitstate = 1;
		}
		
		console.printInt( (netModuleLoader.waitstate * _NETWORK_WAITSTATE__) );
		
		char msg_string[] = "us";
		console.printSpace();
		console.print(msg_string, sizeof(msg_string));
		
		console.printLn();
		
		return;
	}
	
	
	//
	// Send a byte
	if (param0 == 's') {
		
		uint8_t packet[] = {_PACKET_START_BYTE__, _PACKET_TYPE_DATA__, param1, _PACKET_STOP_BYTE__};
		
		if (network_send_packet(networkDevice, packet) == 1) {
			char msg_string[] = "Sent";
			console.print(msg_string, sizeof(msg_string));
			console.printLn();
		}
		
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
		
		uint8_t baudrate = 0;
		
		if ((param1 >= '0') & (param1 <= '9')) {
			char msg_string[] = "baud ";
			console.print(msg_string, sizeof(msg_string));
		} else {
			return;
		}
		
		// Baud rates
		switch (param1) {
			
			case '0': {console.printInt(300);  console.printLn(); baudrate = 0; break;}
			case '1': {console.printInt(600);  console.printLn(); baudrate = 1; break;}
			case '2': {console.printInt(900);  console.printLn(); baudrate = 2; break;}
			case '3': {console.printInt(1200); console.printLn(); baudrate = 3; break;}
			case '4': {console.printInt(2400); console.printLn(); baudrate = 4; break;}
			case '5': {console.printInt(3600); console.printLn(); baudrate = 5; break;}
			case '6': {console.printInt(4800); console.printLn(); baudrate = 6; break;}
			case '7': {console.printInt(9600); console.printLn(); baudrate = 7; break;}
			
			case '8': {console.printInt(28); console.printChar('.'); console.printInt(8); console.printChar('k');
				console.printLn(); baudrate = 8; break;}
			
			case '9': {console.printInt(33); console.printChar('.'); console.printInt(6); console.printChar('k');
				console.printLn(); baudrate = 9; break;}
			
		}
		
		//
		// Set the new baud rate
		byte = 0xff;
		call_extern(networkDevice, 0x07, baudrate);  // Set baud rate register
		call_extern(networkDevice, 0x06, byte);      // Apply the baud rate
		
		return;
	}
	
	
	//
	// Send a probing signal
	if (param0 == 'p') {
		
		// Clear RX flag
		byte = 0x00;
		call_extern(networkDevice, 0x02, byte);
		
		uint8_t retry = 3;
		if ((param1 >= '0') & (param1 <= '9')) 
			retry = param1 - '0';
		
		for (uint8_t i=0; i < retry; i++) {
			
			// Write data to the TX frame buffer
			byte = 0x55;
			call_extern(networkDevice, 0x04, byte);
			
			// Send the frame
			flag = 0xff;
			call_extern(networkDevice, 0x00, flag);
			
			for (counter=0; counter < timeout; counter++) {
				
				// Check the receive flag
				call_extern(networkDevice, 0x03, flag);
				
				if (flag != 0) {
					
					// Read RX buffer data
					uint8_t address = 0x00;
					call_extern(networkDevice, 0x0a, address, byte);
					
					// Verify the received data is a return probing signal
					if (byte == 0x55) break;
				}
				
			}
			
			// Clear RX flag
			byte = 0x00;
			call_extern(networkDevice, 0x02, byte);
			
			if (counter == timeout) {
				console.print("Timed out", sizeof("Timed out"));
				console.printLn();
			} else {
				console.printInt(counter);
				console.printSpace();
				console.printChar('u');
				console.printChar('s');
				console.printLn();
				
				_delay_ms(300);
			}
			
		}
		
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

uint8_t network_send_packet(Device network_device, uint8_t packet[]) {
	
	uint8_t flag = 0;
	uint8_t byte = 0;
	
	for (uint8_t i=0; i <= 3;) {
		
		call_extern(network_device, 0x01, flag);
		if (flag == 0) {
			
			flag = 0xff;
			byte = packet[i];
			
			// Write data to the TX frame buffer
			call_extern(network_device, 0x04, byte);
			call_extern(network_device, 0x00, flag);
			
			// Transfer waitstate
			for (uint32_t t=0; t < netModuleLoader.waitstate; t++)
				_delay_us(_NETWORK_WAITSTATE__);
			
			i++;
			
		}
		
	}
	
	return 1;
}


