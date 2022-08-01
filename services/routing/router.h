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
	uint8_t  count      = 0x00;
	
	uint8_t start_byte;
	uint8_t type_byte;
	uint8_t data_byte;
	uint8_t end_byte;
	
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
		call_extern(networkDevice, 0x03, count);
		if (count > 0) {
			
			// Check the RX buffer for valid packets
			for (uint8_t i=0; i <= count; i++) {
				
				// Extract the start byte
				call_extern(networkDevice, 0x0a, i, start_byte);
				
				// Check packet start byte
				if (start_byte == 0x55) {
					
					// Extract packet end byte
					address = i + 3;
					call_extern(networkDevice, 0x0a, address, end_byte);
					
					// Check packet end byte
					if (end_byte == 0xaa) {
						
						// Extract the packet type byte
						address = i + 1;
						call_extern(networkDevice, 0x0a, address, type_byte);
						
						// Extract the packet data
						address = i + 2;
						call_extern(networkDevice, 0x0a, address, data_byte);
						
						// Check packet type
						switch (type_byte) {
							
							// Data packet
							case 0x00: {
								// Print the received data
								console.printChar(data_byte);
								console.updateCursorPosition();
								
								// Clear packet
								data = 0x00;
								for (uint8_t a=i; a <= (i + 3); a++)
									call_extern(networkDevice, 0x09, a, data);
								
								// Zero the RX flag
								flag = 0;
								call_extern(networkDevice, 0x02, flag);
								
								break;
							}
							
							// Data packet
							case 0x55: {
								
								uint8_t ping_packet[] = {0x55, 0x55, 0x00, 0xaa};
								
								network_send(networkDevice, ping_packet);
								
								// Clear packet
								data = 0x00;
								for (uint8_t a=i; a <= (i + 3); a++)
									call_extern(networkDevice, 0x09, a, data);
								
								// Zero the RX flag
								flag = 0;
								call_extern(networkDevice, 0x02, flag);
								
								break;
							}
							
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

