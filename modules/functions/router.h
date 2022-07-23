//
// Router program

void router_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "router"

struct ModuleLoaderRouter {
	ModuleLoaderRouter() {
		
		task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), (void(*)())router_entry_point, _TASK_PRIORITY_NORMAL__, _TASK_SERVICE__);
		
	}
}static routerModuleLoader;



void router_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	uint8_t  flag       = 0xff;
	uint8_t  byte       = 0x00;
	uint8_t  current    = 0x00;
	
	uint8_t  retry      = 3;
	uint32_t timeout    = 245000;
	uint32_t counter    = 0;
	uint8_t  address    = 0x00;
	uint8_t  data       = 0x00;
	
	console.setCursorPosition(0, 0);
	console.clearBuffer();
	
	Device networkDevice;
	
	if (get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), networkDevice) == 0) {
		console.print(error_driver_error, sizeof(error_driver_error));
	}
	
	// Zero the RX flag
	flag = 0;
	call_extern(networkDevice, 0x02, flag);
	
	
	while (1) {
		
		// Check the receive flag
		call_extern(networkDevice, 0x03, flag);
		if (flag > 0) {
			
			for (uint8_t i=0; i < flag; i++) {
				
				// Read RX buffer data
				call_extern(networkDevice, 0x0a, i, byte);
				
				// Ping test byte
				if (byte == 0x55) {
					
					flag = 0xff;
					byte = 0x55;
					
					// Write data to the TX frame buffer
					call_extern(networkDevice, 0x04, byte);
					call_extern(networkDevice, 0x00, flag);
					
					// Zero the RX flag
					flag = 0;
					call_extern(networkDevice, 0x02, flag);
					
					continue;
				}
				
				// Print the received data
				console.printChar(byte);
				console.updateCursorPosition();
				
			}
			
			// Zero the RX flag
			flag = 0;
			call_extern(networkDevice, 0x02, flag);
			
		}
		
		/*
		
		// Check the receive flag
		call_extern(networkDevice, 0x03, flag);
		if (flag == 0) continue;
		
		// Find packet start byte
		uint8_t begin;
		for (begin=0; begin < flag; begin++) {
			
			call_extern(networkDevice, 0x0a, begin, byte);
			
			// Packet start byte
			if (byte == 0xaa) {
				
				// Check packet end byte
				address = begin + 20;
				call_extern(networkDevice, 0x0a, address, byte);
				if (byte != 0xff) continue; // Packet hasnt loaded yet
				
				
				// Read data byte
				address = begin + 1;
				call_extern(networkDevice, 0x0a, address, byte);
				
				console.printChar(byte);
				
				// Clear the packet from the buffer
				data = 0x00;
				for (uint8_t i=begin; i < (begin + 20); i++) 
					call_extern(networkDevice, 0x09, i, data);
				
			}
			
		}
		*/
		
		continue;
	}
	
	return;
}

#undef __MODULE_NAME_

