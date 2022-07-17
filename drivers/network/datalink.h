//
// Data link

#ifndef __DATA_LINK_LAYER__
#define __DATA_LINK_LAYER__

void DataLinkLayerDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct Packet {
	
	
	
	uint8_t AddressDest [4];
	uint8_t AddressSrc  [4];
	
	uint8_t data        [20];
	
};

struct DataLinkLayerDeviceDriver {
	
	DataLinkLayerDeviceDriver() {load_device(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), (Device)DataLinkLayerDeviceDriverEntryPoint, _DEVICE_TYPE_LIBRARY__);}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
}static dataLinkLayerDeviceDriver;


void DataLinkLayerDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {networkInterfaceDriver.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {networkInterfaceDriver.shutdown(); break;}
		case _DEVICE_ADDRESS__: {
			WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
			networkInterfaceDriver.device_address = pointer.address;
		}
		
		//case 0x00: {dataLinkLayerDeviceDriver.writeTXflag(paramA); break;}
		//case 0x01: {dataLinkLayerDeviceDriver.readTXflag(paramA); break;}
		
		default: break;
	}
	
	return;
}







#endif
