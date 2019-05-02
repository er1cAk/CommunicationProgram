//
// Created by Erik Dvorcak on 2019-03-03.
//

#ifndef COMMUNICATIONSERVER_AURORA_H
#define COMMUNICATIONSERVER_AURORA_H

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "AuroraException.h"

#define POLY 0x8408

#define REQUEST_SIZE 10
#define RESPONSE_SIZE 8
#define READ_STATE 50
#define READ_DSP 59

using namespace std;

class Aurora {
private:
    bool _connected;
    bool _timeout;
    int _socket;
    uint8_t _address;
    int _msg;
    int PORT;
    string HOST;
    struct sockaddr_in _server;

    union {
        uint8_t asBytes[4];
        float asFloat;
    } toFloat;

    typedef struct {
        uint8_t TransmissionState;
        uint8_t GlobalState;
    }state;
private:

    void aurora_build_request(uint8_t *message, uint8_t address, uint8_t command, uint8_t type, uint8_t global);
    void writeReadedData(uint8_t *response);
    uint16_t crc16(uint8_t *message, size_t length);
    ssize_t sendRequest(uint8_t address, uint8_t command, uint8_t type, uint8_t global);
    ssize_t receiveResponse(uint8_t *buffer);

public:
    typedef struct {
        uint8_t TransmissionState;
        uint8_t GlobalState;
        float Value;
        bool ReadState;
    }DataDSP;

    DataDSP dataDsp;

    Aurora(string HOST, int PORT);

    Aurora();

    ~Aurora();

    void aurora_connect();
    void aurora_disconnect();
    void aurora_read_state(uint8_t address);
    void readDSP(uint8_t address, uint8_t type, uint8_t global);

};


#endif //COMMUNICATIONSERVER_AURORA_H
