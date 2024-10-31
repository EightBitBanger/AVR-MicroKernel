#ifndef _EMULATOR_X4__
#define _EMULATOR_X4__


void EmulatorSetProgram(uint8_t* buffer, uint32_t size);

void EmulatorStart(uint8_t* filename, uint8_t nameLength, uint16_t priority, uint8_t type);


void EmulateX4(uint8_t messages);

#endif
