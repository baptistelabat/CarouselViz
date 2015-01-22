// Copyright 2014, University of Freiburg
// Systems Theory Lab
// Author: Elias Rosch <eliasrosch@googlemail.com>

#ifndef CPP_UDP_UDP_H
#define CPP_UDP_UDP_H

#include<unistd.h>
#include <arpa/inet.h>
#include "./Sensor.h"

/*
 * Class for using UDP as transmission protocol
 * for the MPU 9xxx sensor values
 */

class UDP {
    public:
        // Constructor/Destructor
        UDP();
        ~UDP();

        // Functions to initialize the UDP communication
        // Initializes udp-socket that communicates only with
        // the specified IP-Address and Port
        void initUDP(const char* other_ip, unsigned short other_port, unsigned short own_port);
        
        // Send 16-Bit-integer via UDP
        void sendUDP(int16_t value);
        // Sends one Structure of Sensorvalues
        void sendUDPstruct(SensorValues *values);

        // Receive 16-Bit integer via UDP
        int16_t receiveUDP();
        
        SensorValues* receiveUDPstruct();

        // Close the udp-socket
        void closeUDP();

    private:
        struct SensorValues _values;
        struct sockaddr_in _addr_me;
        struct sockaddr_in _addr_other;
        // A helper is needed to be able to compare the 
        // address from which the message came with
        // the address we're listening to
        struct sockaddr_in _recv_addr;
        int _udp_socket;
        int _socketlen;
        int _recv_len;
};

#endif // CPP_UDP_UDP_H
