#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/net.h>

void functionNet(uint8_t* param, uint8_t param_length) {
    
    lowercase( &param[0] );
    lowercase( &param[1] );
    lowercase( &param[2] );
    lowercase( &param[3] );
    
    //
    // Set the baud rate
    //
    
    if ((param[0] == 'b') & (param[1] == 'a') & (param[2] == 'u') & (param[3] == 'd')) {
        
        uint8_t msg[] = "Baud set = ?   ";
        
        if (param[5] == '0') {msg[11] = '9'; msg[12] = '0'; msg[13] = '0';                ntSetBaudRate(0);}
        if (param[5] == '1') {msg[11] = '1'; msg[12] = '2'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(1);}
        if (param[5] == '2') {msg[11] = '2'; msg[12] = '4'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(2);}
        if (param[5] == '3') {msg[11] = '4'; msg[12] = '8'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(3);}
        if (param[5] == '4') {msg[11] = '9'; msg[12] = '6'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(4);}
        if (param[5] == '5') {msg[11] = '2'; msg[12] = '8'; msg[13] = 'K';                ntSetBaudRate(5);}
        if (param[5] == '6') {msg[11] = '3'; msg[12] = '3'; msg[13] = 'K';                ntSetBaudRate(6);}
        if (param[5] == '7') {msg[11] = '5'; msg[12] = '6'; msg[13] = 'K';                ntSetBaudRate(7);}
        if (param[5] == '8') {msg[11] = '7'; msg[12] = '6'; msg[13] = 'K';                ntSetBaudRate(8);}
        if (param[5] == '9') {msg[11] = '1'; msg[12] = '2'; msg[13] = '5'; msg[14] = 'K'; ntSetBaudRate(9);}
        
        print(msg, sizeof(msg));
        printLn();
        
        return;
    }
    
    //
    // Send the parameter message
    //
    
    if ((param[0] == 's') & (param[1] == 'e') & (param[2] == 'n') & (param[3] == 'd')) {
        
        uint8_t offset = 10;
        
        struct NetworkPacket packet;
        
        for (uint8_t i=0; i < 16; i++) 
            packet.data[i] = 0x55;
        
        packet.start   = NETWORK_PACKET_START_BYTE;
        
        packet.addr_d[0]   = 0x00;
        packet.addr_d[1]   = 0x00;
        packet.addr_s[0]   = 0x01;
        packet.addr_s[1]   = 0x00;
        
        // Indicate we are a client attempting to connect to a server
        packet.data[0] = 0x55;
        packet.data[1] = 0x00;
        
        //for (uint8_t i=0; i < (param_length - offset) + 1; i++) 
        //    packet.data[i] = param[offset + i];
        
        packet.data[0] = 'A';
        packet.data[1] = 'S';
        packet.data[2] = 'S';
        packet.data[3] = 'T';
        packet.data[4] = 'W';
        packet.data[5] = 'A';
        packet.data[6] = 'T';
        
        packet.stop    = NETWORK_PACKET_STOP_BYTE;
        
        // Transmit the packet
        ntPacketSend(&packet);
        
        return;
    }
    
    
    //
    // Connect to a server, adding your address to the routing table
    //
    
    if ((param[0] == 'c') & (param[1] == 'o') & (param[2] == 'n')) {
        
        struct NetworkPacket packet;
        
        for (uint8_t i=0; i < 16; i++) 
            packet.data[i] = 0x55;
        
        packet.start   = NETWORK_PACKET_START_BYTE;
        
        packet.addr_d[0]   = 0xff;
        packet.addr_d[1]   = 0xff;
        packet.addr_s[0]   = 'H';
        packet.addr_s[1]   = 'I';
        
        // Indicate we are a client attempting to connect to a server
        packet.data[0] = 0x55;
        packet.data[1] = 0x00;
        
        packet.stop    = NETWORK_PACKET_STOP_BYTE;
        
        ntReceiveClear();
        
        // Transmit the packet
        ntPacketSend(&packet);
        
        uint8_t bufferSz = 0;
        
        for (uint32_t i=0; i < 30000; i++) {
            
            uint8_t dataBuffer[32];
            
            bufferSz = ntReceive(dataBuffer, 32);
            
            if (bufferSz > sizeof(struct NetworkPacket) - 1) {
                
                // Check return packet
                if (dataBuffer[0]  != NETWORK_PACKET_START_BYTE) continue;
                if (dataBuffer[21] != NETWORK_PACKET_STOP_BYTE) continue;
                
                if (dataBuffer[3] != 0xff) continue;
                if (dataBuffer[4] != 0xff) continue;
                
                uint8_t addrStrLow[8];
                uint8_t addrStrHigh[8];
                
                int_to_string(packet.addr_d[0], addrStrLow);
                int_to_string(packet.addr_d[1], addrStrHigh);
                
                uint8_t msgReplyFrom[] = "Connected to host";
                print(msgReplyFrom, sizeof(msgReplyFrom));
                printLn();
                
                break;
            }
            
            _delay_us(1);
            
        }
        
        if (bufferSz == 0) {
            
            uint8_t msgReplyFrom[] = "Cannot reach host";
            print(msgReplyFrom, sizeof(msgReplyFrom));
            printLn();
            
        }
        
        return;
    }
    
    
    //
    // Ping the server
    //
    
    if ((param[0] == 'p') & (param[1] == 'i') & (param[2] == 'n') & (param[3] == 'g')) {
        
        uint8_t offset = 10;
        
        struct NetworkPacket packet;
        
        for (uint8_t i=0; i < 16; i++) 
            packet.data[i] = 0x55;
        
        packet.start   = NETWORK_PACKET_START_BYTE;
        
        packet.addr_d[0]   = 0xff;
        packet.addr_d[1]   = 0xff;
        packet.addr_s[0]   = 0x00;
        packet.addr_s[1]   = 0x00;
        
        // Indicate we are a client attempting to connect to a server
        packet.data[0] = 0x55;
        packet.data[1] = 0x00;
        
        for (uint8_t i=0; i < (param_length - offset) + 1; i++) 
            packet.data[i] = param[offset + i];
        
        packet.stop    = NETWORK_PACKET_STOP_BYTE;
        
        ntReceiveClear();
        
        for (uint8_t i=0; i < 1; i++) {
            
            ntPacketSend(&packet);
            
            uint8_t dataBuffer[32];
            
            uint8_t bufferSz = 0;
            
            for (uint16_t i=0; i < 30000; i++) {
                
                bufferSz = ntReceive(dataBuffer, 32);
                
                if (bufferSz > sizeof(struct NetworkPacket) - 1) {
                    
                    // Check return packet
                    if (dataBuffer[0]  != NETWORK_PACKET_START_BYTE) continue;
                    if (dataBuffer[21] != NETWORK_PACKET_STOP_BYTE) continue;
                    
                    uint8_t addrStrLow[8];
                    uint8_t addrStrHigh[8];
                    
                    int_to_string(packet.addr_d[0], addrStrLow);
                    int_to_string(packet.addr_d[1], addrStrHigh);
                    
                    uint8_t msgReplyFrom[] = "Reply from ";
                    print(msgReplyFrom, sizeof(msgReplyFrom));
                     
                    print(addrStrLow, 4);
                    
                    uint8_t msgPeriod[] = ".";
                    print(msgPeriod, sizeof(msgPeriod));
                    
                    print(addrStrHigh, 4);
                   
                    printLn();
                    
                    break;
                }
                
                _delay_us(1);
                
            }
            
            if (bufferSz > 0) 
                continue;
            
            // Connection timed out
            
            _delay_ms(1000);
            
            uint8_t msgRequestTimedOut[] = "Request timed out";
            print(msgRequestTimedOut, sizeof(msgRequestTimedOut));
            printLn();
            
            continue;
        }
        
        return;
    }
    
    //
    // Hammer the network with continuous packets
    //
    
    if ((param[0] == 'r') & (param[1] == 'e') & (param[2] == 'p')) {
        
        while (1) {
            
            uint8_t offset = 10;
            
            struct NetworkPacket packet;
            
            for (uint8_t i=0; i < 16; i++) 
                packet.data[i] = 0x55;
            
            packet.start   = NETWORK_PACKET_START_BYTE;
            
            packet.addr_d[0]   = 0xff;
            packet.addr_d[1]   = 0xff;
            packet.addr_s[0]   = 0x00;
            packet.addr_s[1]   = 0x00;
            
            packet.data[0] = 0x00;
            packet.data[1] = 0x00;
            
            for (uint8_t i=0; i < (param_length - offset) + 1; i++) 
                packet.data[i] = param[offset + i];
            
            packet.stop    = NETWORK_PACKET_STOP_BYTE;
            
            ntPacketSend(&packet);
            
            _delay_ms(1);
            
        }
        
    }
    
    
    
    //
    // Check for any packets
    //
    
    if ((param[0] == 'r') & (param[1] == 'e') & (param[2] == 'c') & (param[3] == 'v')) {
        
        struct NetworkPacket packet;
        
        uint8_t state = ntPacketReceive(&packet);
        ntPacketClean();
        
        if (state == 1) {
            
            uint8_t msgAddressDestA[10];
            uint8_t msgAddressDestB[10];
            
            uint8_t msgAddressSrceA[10];
            uint8_t msgAddressSrceB[10];
            
            uint8_t placeDstA = int_to_string(packet.addr_d[0], msgAddressDestA);
            uint8_t placeDstB = int_to_string(packet.addr_d[1], msgAddressDestB);
            
            uint8_t placeSrcA = int_to_string(packet.addr_s[0], msgAddressSrceA);
            uint8_t placeSrcB = int_to_string(packet.addr_s[1], msgAddressSrceB);
            
            if (placeDstA == 0) placeDstA++;
            if (placeDstB == 0) placeDstB++;
            
            if (placeSrcA == 0) placeSrcA++;
            if (placeSrcB == 0) placeSrcB++;
            
            print(msgAddressDestA, placeDstA+1);
            uint8_t msgPeriod[] = ".";
            print(msgPeriod, sizeof(msgPeriod));
            print(msgAddressDestB, placeDstB+1);
            
            printLn();
            
            print(msgAddressSrceA, placeSrcA+1);
            print(msgPeriod, sizeof(msgPeriod));
            print(msgAddressSrceB, placeSrcB+1);
            
            printLn();
            
        } else {
            
            uint8_t msgNoSuccess[] = "No messages";
            
            print(msgNoSuccess, sizeof(msgNoSuccess));
            printLn();
            
        }
        
        return;
    }
    
    return;
}


void registerCommandNet(void) {
    
    uint8_t netCommandName[] = "net";
    
    ConsoleRegisterCommand(netCommandName, sizeof(netCommandName), functionNet);
    
    return;
}
