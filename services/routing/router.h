//
// Router program

void router_entry_point(void) {
	
	uint8_t  flag       = 0xff;
	uint8_t  byte       = 0x00;
	uint8_t  current    = 0x00;
	
	uint8_t  retry      = 3;
	uint32_t timeout    = 245000;
	uint8_t  counter    = 0;
	uint8_t  address    = 0x00;
	uint8_t  data       = 0x00;
	uint8_t  count      = 0x00;
	
	uint8_t start_byte = 0;
	uint8_t type_byte  = 0;
	uint8_t data_byte  = 0;
	uint8_t end_byte   = 0;
	
	console.setCursorPosition(0, 0);
	console.clearBuffer();
	
	Device networkDevice;
	
	// Check available network devices
	uint8_t active_network_cards[5];
	for (uint8_t i=0; i < 5; i++) 
		active_network_cards[i] = 0;
	
	HardwareInformation hInfo;
	
	Bus bus;
	bus.waitstate_read  = 20;
	bus.waitstate_write = 20;
	
	
	uint32_t device_address = 0x40000;
	for (uint8_t i=0; i < 5; i++) {
		
		get_hardware_info(device_address, bus, hInfo);
		
		if (hInfo.device_id == 0x14) {
			
			active_network_cards[i] = 0xff;
			
			console.printInt(i);
			console.printSpace();
			console.printHex(hInfo.device_id);
			console.printSpace();
			console.print(hInfo.device_name, 10);
			console.printLn();
			
			_delay_ms(350);
		}
		
		device_address += 0x10000;
	}
	
	_delay_ms(3000);
	
	console.clearBuffer();
	console.setCursorPosition(0, 0);
	
	networkDevice = (Device)get_func_address(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__));
	if (networkDevice == 0) 
		console.print(error_driver_error, sizeof(error_driver_error));
	
	// Zero the RX flag
	flag = 0;
	call_extern(networkDevice, 0x02, flag);
	
	uint32_t current_network_card = 0;
	
	while (1) {
		
		current_network_card++;
		if (current_network_card > 4) current_network_card = 0;
		
		if (active_network_cards[current_network_card] == 0x00) continue;
		networkInterfaceDriver.device_address = 0x30000 + (0x10000 * (current_network_card + 1));
		
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
							
							// Print character
							case 0x00: {
								
								console.printChar(data_byte);
								console.updateCursorPosition();
								
								// Clear packet
								uint8_t data = 0x00;
								for (uint8_t a=address; a <= (address + 3); a++)
								call_extern(networkDevice, 0x09, a, data);
								
								// Zero the RX flag
								uint8_t flag = 0;
								call_extern(networkDevice, 0x02, flag);
								
								break;
							}
							
							// Ping packet
							case 0x55: {
								
								uint8_t ping_packet[] = {0x55, 0x55, 0x00, 0xaa};
								
								network_send(networkDevice, ping_packet);
								
								// Clear packet
								uint8_t data = 0x00;
								for (uint8_t a=address; a <= (address + 3); a++)
								call_extern(networkDevice, 0x09, a, data);
								
								// Zero the RX flag
								uint8_t flag = 0;
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


