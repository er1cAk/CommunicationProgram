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
        cout<< "Connection Error" << endl;
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
    message[4] = 32;
    message[5] = 32;
    message[6] = 32;
    message[7] = 32;

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
    timeval tv;
    tv.tv_sec = 2;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(_socket, &rfds);

    int ready = select(_socket + 1, &rfds, NULL, NULL, &tv);

    switch (ready) {
        case 0:
            _timeout = true;
            cout << "Time out" << endl;
            return 0;
//            THIS DOESNT WORK PROPERLY
//      case -1:
//            cout << "Error" << endl;
//            return 0;
        default:
            return recv(_socket, (char *) buffer, 1024, 0);
//            if(recv(_socket, (char *) buffer, 1024, 0) > 0){
//                uint16_t waitedCrc =  ((uint16_t ) buffer[7] << 8) | (uint16_t )buffer[6];
//                cout << "waited CRC: " << waitedCrc << endl;
//                if( waitedCrc == crc16(buffer, 7))
//                    return 1;
//            }
    }
}

void Aurora::aurora_read_state(uint8_t address) {
    if(_connected) {
        if (sendRequest(address, READ_STATE, 0, 0) > -1) {
            uint8_t response[RESPONSE_SIZE];

            if (receiveResponse(response) == 1) {
                printit(response);
            }else{
                throw AuroraReceivingResponseException();
            }
        }else{
            throw AuroraSendingRequestException();
        }
    } else {
        throw AuroraConnectException();
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
    }else{
        throw AuroraConnectException();
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
