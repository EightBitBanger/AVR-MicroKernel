//
// Banner message 

void banner_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void banner_task(void);

#define __MODULE_NAME_  "banner"

struct ModuleLoaderBanner {
	
	ModuleLoaderBanner() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)banner_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static bannerModuleLoader;



void banner_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	char message[] = "Happy 4th";
	uint8_t shift = 0;
	
	while (1) {
		
		console.setCursorPosition(1, shift);
		
		for (uint8_t i=0; i < sizeof(message)-1; i++) {
			
			console.printChar(message[i]);
			if (console.cursorPos > 20) console.cursorPos = 0;
		}
		
		console.setCursorPosition(0,0);
		
		shift++;
		if (shift > 20) shift = 0;
		
		_delay_ms(800);
		console.clearBuffer();
		
	}
	
	return;
}

#undef __MODULE_NAME_

