//
// Network application

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "net"

struct ModuleLoaderNet {
	
	ModuleLoaderNet() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)net_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static netModuleLoader;


void task_net_ping(void) {
	
	uint8_t  flag    = 0xff;
	uint8_t  byte    = 0x00;
	
	uint8_t  retry   = 3;
	uint32_t timeout = 245000;
	uint32_t counter = 0;
	
	Device networkDevice;
	
	if (get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), networkDevice) == 0) return;
	
	// Clear RX flag
	byte = 0x00;
	call_extern(networkDevice, 0x02, byte);
	
	// Send probing signal and await a return
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
				call_extern(networkDevice, 0x07, address, byte);
				
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

void net_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	//task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), task_net_ping, _TASK_PRIORITY_BACKGROUND__, _TASK_VOLATILE__);
	
	task_net_ping();
	
	return;
}

#undef __MODULE_NAME_

