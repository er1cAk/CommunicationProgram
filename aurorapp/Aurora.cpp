//
// Created by Erik Dvorcak on 2019-03-03.
//

#include "Aurora.h"

void printit(uint8_t *response){
    for (int idx = 0; idx < 8; ++idx) {
        cout <<"Response[ " << idx << " ]: " << (int)response[idx]<< " >> " << (unsigned char)response[idx] << endl;
    }
    cout << endl;
}

Aurora::Aurora(string HOST, int PORT) {
    this->HOST = HOST;
    this->PORT = PORT;
}

Aurora::Aurora() = default;

Aurora::~Aurora() = default;

void Aurora::aurora_connect() {
    if(HOST == "" || PORT == 0) {
        cout << "Missing Host or Port" << std::endl;
    } else {
        cout << "Found Proper Host "<< HOST << " and Port " << PORT <<endl;
    }

    _socket = socket(AF_INET, SOCK_STREAM, 0);

    if(_socket == -1) {
        cout <<"Error Opening Socket" <<endl;
    } else {
        cout <<"Socket Opened Successfully" << endl;
    }

    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = inet_addr(HOST.c_str());
    _server.sin_port = htons(PORT);

    if (connect(_socket, (struct sockaddr*)&_server, sizeof(_server)) < 0) {
        throw AuroraConnectException();
    } else {
        _connected = true;
    }
}

void Aurora::aurora_disconnect() {
    close(_socket);
    cout << "Socket has been closed." << endl;
}

ssize_t Aurora::sendRequest(uint8_t address, uint8_t command, uint8_t type, uint8_t global){
    if(_connected) {
        uint8_t message[REQUEST_SIZE];
        aurora_build_request(message, address, command, type, global);
        return send(_socket, message, (size_t) 10, 0);
    }else{
        throw AuroraConnectException();
    }
    return -1;
}

void Aurora::aurora_build_request(uint8_t *message, uint8_t address, uint8_t command, uint8_t type, uint8_t global) {
    message[0] = address;
    message[1] = command;
    message[2] = type;
    message[3] = global;
    message[4] = 0;
    message[5] = 0;
    message[6] = 0;
    message[7] = 0;

    //getting CRC_16
    uint16_t  crc_16 = crc16(message, 8);

    uint8_t BccHi = (uint8_t)((crc_16 & 0xFF00) >> 8), //high byte
            BccLo = (uint8_t)(crc_16 & 0xFF); //low byte

    message[8] = BccLo;
    message[9] = BccHi;
}

uint16_t Aurora::crc16(uint8_t *message, size_t length){
    uint16_t crc = 0xffff;
    for (size_t idx = 8; idx > 0; idx--) {
        crc ^= *message++;
        for (size_t idx = 0; idx < length; idx++){
            if (crc & 0x0001) {
                crc = ( crc >> 1 ) ^ POLY;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return ~crc ;
}

ssize_t Aurora::receiveResponse(uint8_t *buffer){
    fd_set rfds;
    struct timeval tv;
    int result;

    FD_ZERO(&rfds);
    FD_SET(_socket, &rfds);


    tv.tv_sec = 2;
    tv.tv_usec = 0;

    result = select(_socket + 1, &rfds, NULL, NULL, &tv);

    if (result == -1){
        return -1;
    }else if (result){
        if(FD_ISSET(_socket, &rfds)){
            return read(_socket, buffer, 256);
        }
    }else{
        return -2;
    }
}

void Aurora::readState(uint8_t address) {
    if(_connected) {
        if (sendRequest(address, READ_STATE, 0, 0) > -1) {
            uint8_t response[RESPONSE_SIZE];
            if (receiveResponse(response) > -1) {
                dataState.TransmissionState = response[0];
                dataState.GlobalState = response[1];
                dataState.InverterState = response[2];
                dataState.AlarmState = response[5];
            }else{
                throw AuroraReceivingResponseException();
            }
        }else{
            throw AuroraSendingRequestException();
        }
    }
}

void Aurora::readDSP(uint8_t address, uint8_t type, uint8_t global) {
    if(_connected){
        if( sendRequest(address, READ_DSP, type, global)  > -1 ){
            uint8_t response[RESPONSE_SIZE];
            if(receiveResponse(response) > -1){
                writeReadedData(response);
            }else{
                throw AuroraReceivingResponseException();
            }
        }else{
            throw AuroraSendingRequestException();
        }
    }
}

void Aurora::writeReadedData(uint8_t *response) {
    toFloat.asBytes[0] = response[5];
    toFloat.asBytes[1] = response[4];
    toFloat.asBytes[2] = response[3];
    toFloat.asBytes[3] = response[2];

    dataDsp.TransmissionState = response[0];
    dataDsp.GlobalState = response[1];
    dataDsp.ReadState = true;
    dataDsp.Value = toFloat.asFloat;

}
