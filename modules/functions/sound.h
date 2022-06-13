//
// Music command

void command_music(void);

#define __MODULE_NAME_  "sound"

struct ModuleLoaderSound {
	
	ModuleLoaderSound() {
		
		load_module(__MODULE_NAME_, sizeof(__MODULE_NAME_), command_music);
		
	}
}static moduleLoaderSound;
#undef __MODULE_NAME_


void command_music(void) {
	
	uint8_t pulse  = 2;
	
	for (uint8_t i=0; i < 2; i++) {
		
		for (uint8_t a=0; a < 5; a++) {
			
			speakerDriver.beep(pulse, 1);
			speakerDriver.beep(pulse, 3);
			speakerDriver.beep(pulse, 5);
			
			speakerDriver.beep(pulse, 2);
			speakerDriver.beep(pulse, 4);
			speakerDriver.beep(pulse, 6);
			
			speakerDriver.beep(pulse, 3);
			speakerDriver.beep(pulse, 5);
			speakerDriver.beep(pulse, 7);
			
		}
		
		pulse--;
		
	}
	
	return;
}

