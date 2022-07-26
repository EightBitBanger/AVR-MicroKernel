//
// Router program

void router_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "router"

struct ModuleLoaderRouter {
	ModuleLoaderRouter() {
		
		task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), (void(*)())router_entry_point, 10, _TASK_SERVICE__);
		
	}
}static routerModuleLoader;



void router_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	uint8_t  flag       = 0xff;
	uint8_t  byte       = 0x00;
	uint8_t  current    = 0x00;
	
	uint8_t  retry      = 3;
	uint32_t timeout    = 245000;
	uint8_t  counter    = 0;
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
			
			// Check the RX buffer for valid packets
			for (uint8_t i=0; i < 32; i++) {
				
				address = i;
				call_extern(networkDevice, 0x0a, address, byte);
				
				// Check packet start byte
				if (byte == 0x55) {
					
					address = i + 3;
					call_extern(networkDevice, 0x0a, address, byte);
					
					// Check packet end byte
					if (byte == 0xaa) {
						
						// Extract the packet type byte
						address = i + 1;
						call_extern(networkDevice, 0x0a, address, byte);
						
						// Data packet
						if (byte == 0x00) {
							
							// Extract the packet data
							address = i + 2;
							call_extern(networkDevice, 0x0a, address, byte);
							
							// Print the received data
							console.printChar(byte);
							console.updateCursorPosition();
							
							// Clear packet
							data = 0x00;
							for (uint8_t a=i; a < 32; a++)
								call_extern(networkDevice, 0x09, a, data);
							
							// Zero the RX flag
							flag = 0;
							call_extern(networkDevice, 0x02, flag);
							
							continue;
						}
						
						continue;
					}
					
				}
				
			}
			
		}
		
		continue;
	}
	
	return;
}

#undef __MODULE_NAME_

