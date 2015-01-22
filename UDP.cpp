// Copyright 2014, University of Freiburg
// Systems Theory Lab
// Author: Elias Rosch <eliasrosch@googlemail.com>

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "./UDP.h"
#include "./Sensor.h"

#define MAXBUF 2
#define PORT 8080
#define SERVER_IP "10.42.0.1"

/*
 * Class for using UDP as transmission protocol
 * for the MPU 9xxx sensor values
 */

UDP::UDP(){
}

UDP::~UDP(){
}

// Initializes the communication via udp
void UDP::initUDP(const char* other_ip, unsigned short other_port, unsigned short own_port) {
    //create a UDP socket with inet and datagrams
    if ((_udp_socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Failed to create own udp socket: %s\n", strerror(errno));
        exit(1);
    }
    
    // Set the contents of the local address struct
    memset((char *) &_addr_me, 0, sizeof(_addr_me));
    _addr_me.sin_family = AF_INET;
    _addr_me.sin_port = htons(own_port);
    // Listen to incoming messages of any address
    _addr_me.sin_addr.s_addr = INADDR_ANY;
    

    // bind socket to port
    if( bind(_udp_socket, (struct sockaddr*)&_addr_me, sizeof(_addr_me) ) == -1) {
    printf("Failed to bind socket to port: %s\n", strerror(errno));
    exit(1);
    }

    //Set the contents of the other address struct
    memset((char *) &_addr_other, 0, sizeof(_addr_other));
    _addr_other.sin_family = AF_INET;
    _addr_other.sin_port = htons(other_port);
    // Send data to the given ip-address and port
    if (inet_aton(other_ip, &_addr_other.sin_addr) == 0) {
        printf("Failed to create sender ip-struct: %s\n", strerror(errno));
        exit(1);
    }
    _socketlen = sizeof(_addr_other);
}

int16_t UDP::receiveUDP() {
    int16_t value = 0;
    // try to receive some data ( blocking! )


    if ((_recv_len = recvfrom(_udp_socket, &value, sizeof(value), 0, (struct sockaddr *)&_recv_addr, (socklen_t*)&_socketlen)) == -1) {
        printf("Failed to receive udp data: %s\n", strerror(errno));
        exit(1);
     }
     if (inet_ntoa(_recv_addr.sin_addr) != inet_ntoa(_addr_other.sin_addr)) {
        printf("Received from wrong IP-Address: %s\n", inet_ntoa(_recv_addr.sin_addr));
     }
     // return (int16_t) atoi(buf);
     return ntohs(value);
}

SensorValues* UDP::receiveUDPstruct() {
    // try to receive some data ( blocking! )

    if ((_recv_len = recvfrom(_udp_socket, &_values, sizeof(struct SensorValues), 0, (struct sockaddr *)&_recv_addr, (socklen_t*)&_socketlen)) == -1) {
        printf("Failed to receive udp data: %s\n", strerror(errno));
        exit(1);
     }
     if (inet_ntoa(_recv_addr.sin_addr) != inet_ntoa(_addr_other.sin_addr)) {
        printf("Received from wrong IP-Address: %s\n", inet_ntoa(_recv_addr.sin_addr));
     }
     return &_values;
}

void UDP::sendUDP(int16_t value) {
    value = htons(value);
    if (sendto(_udp_socket, &value, sizeof(value), 0, (struct sockaddr*)&_addr_other, _socketlen) == -1) {
        printf("Failed to send udp data: %s\n", strerror(errno));
        exit(1);
    }
}

void UDP::sendUDPstruct(SensorValues *values) {
    if (sendto(_udp_socket, values, sizeof(struct SensorValues), 0, (struct sockaddr*)&_addr_other, _socketlen) == -1) {
        printf("Failed to send udp data: %s\n", strerror(errno));
        exit(1);
    }
}

void UDP::closeUDP() {
    close(_udp_socket);
}

