#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/network/router.h>

struct RoutingTable routingTable;


void InitiateRouter(void) {
    
    //
    // Assemble default packets
    //
    
    // Client handshake packet
    
    struct NetworkPacket packetClientHandshake;
    packetClientHandshake.start = NETWORK_PACKET_START_BYTE;
    
    packetClientHandshake.addr_d[0] = 0x00; // To the client
    packetClientHandshake.addr_d[1] = 0x00;
    packetClientHandshake.addr_s[0] = 0xff; // From the router
    packetClientHandshake.addr_s[1] = 0xff;
    
    for (uint8_t i=0; i < NETWORK_PACKET_DATA_SIZE; i++) 
        packetClientHandshake.data[i] = 0x55;
    
    packetClientHandshake.data[0] = 0x55;
    packetClientHandshake.data[1] = 0x00;
    
    packetClientHandshake.stop = NETWORK_PACKET_STOP_BYTE;
    
    
    // Router handshake packet
    
    struct NetworkPacket packetRouterHandshakeOutgoing;
    packetRouterHandshakeOutgoing.start = NETWORK_PACKET_START_BYTE;
    
    packetRouterHandshakeOutgoing.addr_d[0] = 0xff; // To the router
    packetRouterHandshakeOutgoing.addr_d[1] = 0xff;
    packetRouterHandshakeOutgoing.addr_s[0] = 0xff; // From a router requesting a connection
    packetRouterHandshakeOutgoing.addr_s[1] = 0xff;
    
    for (uint8_t i=0; i < NETWORK_PACKET_DATA_SIZE; i++) 
        packetRouterHandshakeOutgoing.data[i] = 0x55;
    
    packetRouterHandshakeOutgoing.stop = NETWORK_PACKET_STOP_BYTE;
    
    
    
    
    
    //
    // Initiate the routing table
    //
    
    for (uint8_t s=0; s < NUMBER_OF_PERIPHERALS; s++) {
        
        routingTable.device_state[s] = 0x00;
        routingTable.device_type[s]  = 0x00;
        
        routingTable.address_low[s]  = 0x00;
        routingTable.address_high[s] = 0x00;
        
        routingTable.device_state[s] = 0;
        routingTable.device_type[s] = 0;
        
        // Initiate NIC baud rate
        ntBindDevice(s);
        
        _delay_ms(10);
        
        if (ntCheckDevice() == 0) 
            continue;
        
        _delay_ms(10);
        
        ntSetBaudRate( NETWORK_BAUD_RATE );
        
        routingTable.device_state[s] = 1; // Normal active NIC device
        
        routingTable.device_type[s]  = CLIENT_TYPE_UNKNOWN;
        
        continue;
    }
    
    
    
    //
    // Connect to adjacent routers to set the default gateway(s)
    //
    
    for (uint8_t s=0; s < NUMBER_OF_PERIPHERALS; s++) {
        
        if (routingTable.device_state[s] == 0) 
            continue;
        
        ntBindDevice(s);
        
        ntPacketClearAll();
        
        ntPacketSend(&packetRouterHandshakeOutgoing);
        
    }
    
    
    
    
    
    
    struct NetworkPacket receive;
    
    while(1) {
        
        for (uint8_t s=0; s < NUMBER_OF_PERIPHERALS; s++) {
            
            if (routingTable.device_state[s] == 0) 
                continue;
            
            receive.addr_d[0] = 0x00; receive.addr_d[1] = 0x00;
            receive.addr_s[0] = 0x00; receive.addr_s[1] = 0x00;
            
            ntBindDevice(s);
            
            // Check incoming packet
            if (ntPacketReceive(&receive, 1) == 0) 
                continue;
            
            // Check packet address
            if ((receive.addr_d[0] == 0x00) & (receive.addr_d[1] == 0x00) & 
                (receive.addr_s[0] == 0x00) & (receive.addr_s[1] == 0x00)) {
                
                continue;
            }
            
            // Clear old packet data
            ntPacketClearAll();
            
            
            //
            // Incoming to this server
            //
            
            if ((receive.addr_d[0] == 0xff) & 
                (receive.addr_d[1] == 0xff)) {
                
                //
                // Client connection request
                //
                
                if ((receive.data[0] == 0x55) & 
                    (receive.data[1] == 0x00)) {
                    
                    // Send the return packet acknowledging the client connection request
                    ntPacketSend( &packetClientHandshake );
                    
                    // Client is an addressable end point node
                    routingTable.device_type[s] = CLIENT_TYPE_NODE;
                    
                    // Collect the client address
                    routingTable.address_low[s]  = receive.addr_s[0];
                    routingTable.address_high[s] = receive.addr_s[1];
                    
                    continue;
                }
                
                
                //
                // Router connection request
                //
                
                if ((receive.addr_s[0] == 0xff) & 
                    (receive.addr_s[1] == 0xff)) {
                    
                    // Device is connected to a router
                    if (routingTable.device_type[s] != CLIENT_TYPE_ROUTER) {
                        
                        routingTable.device_type[s] = CLIENT_TYPE_ROUTER;
                        
                        // Send the return packet acknowledging the router connection request
                        ntPacketSend( &packetRouterHandshakeOutgoing );
                        
                    }
                    
                }
                
                
                //
                // Ping request
                //
                
                if ((receive.addr_s[0] == 0x00) & 
                    (receive.addr_s[1] == 0x00)) {
                    
                    // Return the ping packet to the sender
                    ntPacketSend( &receive );
                    
                }
                
                continue;
            }
            
            
            //
            // Handle client packets and route them to the destination address
            // If the destination address does not exist the packet will be 
            // routed through the default gateway to the next router(s).
            
            //
            // Check if the destination address node is
            // in this routing table
            
            uint8_t destinationDeviceLocated = 0;
            
            for (uint8_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
                
                if (routingTable.device_type[d] == CLIENT_TYPE_NODE) {
                    
                    // Find the destination packet address in the routing table
                    if ((receive.addr_d[0] == routingTable.address_low[d]) & 
                        (receive.addr_d[1] == routingTable.address_high[d])) {
                        
                        destinationDeviceLocated = 1;
                        
                        ntBindDevice(d);
                        
                        ntPacketSend(&receive);
                        
                        break;
                    }
                    
                }
                
            }
            
            
            // Do NOT send the packet back out if the destination
            // was located on this routing table
            
            if (destinationDeviceLocated == 1) 
                continue;
            
            
            //
            // Route the packet to any default gateway currently connected
            //
            
            for (uint8_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
                
                // Do not send the pack
                if (s == d) 
                    continue;
                
                // Check if the destination is valid
                if (routingTable.device_type[d] != CLIENT_TYPE_ROUTER) 
                    continue;
                
                // Only send the packet if the source is connected
                if ((routingTable.device_type[s] == CLIENT_TYPE_NODE) | 
                    (routingTable.device_type[s] == CLIENT_TYPE_ROUTER) | 
                    (routingTable.device_type[s] == CLIENT_TYPE_SERVER)) {
                    
                    ntBindDevice(d);
                    
                    ntPacketSend(&receive);
                    
                }
                
            }
            
            continue;
        }
        
    }
    
    
    
    
    
    
    
    return;
}







