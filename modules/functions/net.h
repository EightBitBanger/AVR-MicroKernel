//
// Network application

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "net"

struct ModuleLoaderNet {
	
	uint8_t waitstate = 0x00;
	
	ModuleLoaderNet() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)net_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static netModuleLoader;


void task_net_ping(void) {
	
	uint8_t  flag       = 0xff;
	uint8_t  flagState  = 0xff;
	uint8_t  flagOld    = 0xff;
	uint8_t  byte       = 0x00;
	
	uint8_t  retry      = 3;
	uint32_t timeout    = 245000;
	uint32_t counter    = 0;
	
	Device networkDevice;
	
	if (get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), networkDevice) == 0) return;
	
	// Get the parameter from the keyboard string
	uint8_t param0 = console.keyboard_string[4];
	uint8_t param1 = console.keyboard_string[6];
	
	
	// Check transfer speed
	if (param0 == 't') {
		
		for (uint32_t i=0; i < 200000; i++) {
			
			call_extern(networkDevice, 0x01, flag);
			if (flag == 0) {
				
				flag = 0xff;
				
				// Write data to the TX frame buffer
				call_extern(networkDevice, 0x04, param1);
				call_extern(networkDevice, 0x00, flag);
				
				// Transfer waitstate
				for (uint8_t a=0; a < netModuleLoader.waitstate; a++) 
					_delay_ms(10);
				
			}
			
		}
		
		return;
	}
	
	
	// Set the transfer waitstate
	if (param0 == 'w') {
		netModuleLoader.waitstate = (param1 - 0x30);
		return;
	}
	
	// Send a byte
	if (param0 == 's') {
		
		// Send a frame
		call_extern(networkDevice, 0x01, flag);
		if (flag == 0) {
			
			flag = 0xff;
			
			// Write data to the TX frame buffer
			call_extern(networkDevice, 0x04, param1);
			call_extern(networkDevice, 0x00, flag);
			
		}
		
		return;
	}
	
	
	// Check the receiver
	if (param0 == 'r') {
		
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
		}
		
		return;
	}
	
	// Clear RX flag
	if (param0 == 'c') {
		flag = 0;
		call_extern(networkDevice, 0x02, flag);
		return;
	}
	
	// Set baud rate
	if (param0 == 'b') {
		
		WrappedPointer pointer;
		pointer.address = 0;
		
		// Baud rates
		if (param1 == '0') {pointer.address = 80;}
		if (param1 == '1') {pointer.address = 1041;} // Default
		if (param1 == '2') {pointer.address = 16000;}
		
		if (pointer.address == 0) return;
		
		// Print the baud rate
		console.print("baud ", sizeof("baud "));
		
		char addressString[8];
		for (uint8_t i=0; i < 8; i++) addressString[i] = 0x20;
		
		int_get_string(pointer.address, addressString);
		console.print(addressString, sizeof("xxxxxx"));
		console.printLn();
		
		// Set the baud rate
		call_extern(networkDevice, 0x07, pointer.byte_t[0]); // Low byte
		call_extern(networkDevice, 0x08, pointer.byte_t[1]); // High byte
		call_extern(networkDevice, 0x06, byte); // Set the baud rate
		
		return;
	}
	
	
	// Send a probing signal
	if (param0 == 'p') {
		
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
					
					// Verify the received data
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
	
	return;
}

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	//task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), task_net_ping, _TASK_PRIORITY_BACKGROUND__, _TASK_VOLATILE__);
	
	task_net_ping();
	
	return;
}

#undef __MODULE_NAME_

