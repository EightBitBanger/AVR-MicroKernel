<<<<<<< HEAD
#ifndef NETWORK_SERVER_APPLICATION
#define NETWORK_SERVER_APPLICATION

#include <kernel/network/network.h>

struct ServerTable {
	
	uint8_t device_state[5];
	uint8_t device_type[5];
	
	uint8_t address_low[5];
	uint8_t address_high[5];
    
};

void InitiateServer(void);

#endif
=======
#ifndef NETWORK_SERVER_APPLICATION
#define NETWORK_SERVER_APPLICATION

#include <kernel/network/network.h>

struct ServerTable {
	
	uint8_t device_state[5];
	uint8_t device_type[5];
	
	uint8_t address_low[5];
	uint8_t address_high[5];
    
};

void InitiateServer(void);

#endif
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
