#ifndef _EMULATOR_X4__
#define _EMULATOR_X4__

#define  NOP   0x90
#define  LDA   0x86
#define  STA   0x87
#define  MOVB  0x88
#define  MOV   0x89
#define  INC   0xFD
#define  DEC   0xFC

#define  IN    0x85
#define  OUT   0x84

#define  ADD   0x00
#define  SUB   0x80
#define  MUL   0xF6
#define  DIV   0xF4

#define  POP   0x0F
#define  PUSH  0xF0

#define  CMP   0x38
#define  CMPR  0x39

#define  JMP   0xFE
#define  JE    0xFB
#define  JNE   0xF3

#define  CALL  0x9A
#define  RET   0xCB

#define  INT   0xCD







#define  rAL   0x00
#define  rAH   0x01
#define  rBL   0x02
#define  rBH   0x03
#define  rCL   0x04
#define  rCH   0x05
#define  rDL   0x06
#define  rDH   0x07


void EmulatorSetProgram(uint8_t* buffer, uint32_t size);
void EmulatorSetParam(uint8_t* buffer, uint32_t length);

void EmulatorStart(uint8_t* filename, uint8_t nameLength, uint16_t priority, uint8_t privilege, uint8_t type);


void EmulateX4(uint8_t messages);


// Compilation

void AssembleJoin(uint8_t* buffer, uint32_t begin_address, uint8_t* source, uint32_t length);


#endif
