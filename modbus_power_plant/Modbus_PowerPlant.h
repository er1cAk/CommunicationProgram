//
// Created by Erik Dvorcak on 2019-03-02.
//

#ifndef COMMUNICATIONSERVER_MODBUS_POWER_PLANT_H
#define COMMUNICATIONSERVER_MODBUS_POWER_PLANT_H

#define COUNT_OF_READING_REGISTERS 1

#define INSTANT_POWER 1949
#define DC_VOLTAGE 1107
#define CURRENT 1103

#define alarms 1171
#define alarms1 1172

#define MODBUS_DEFAULT_PORT 502

#include "../power_plant/PowerPlant.h"
#include "../modbuspp/modbus.h"
#include "../modbuspp/modbus_exception.h"

using namespace std;
class ModbusPowerPlant: public PowerPlant{
private:
    modbus modbus1 = modbus("", 0);

    ssize_t readInstantPower(int inverter_id, double divisor);
    int readDcVoltage(int inverter_id);
    int readCurrent(int inverter_id, double divisor);
    int readAlarms(int inverter_id);
    int readAlarms1(int inverter_id);
    void checkAlarms(int inverter_id);
    string DescriptionAlarm(uint8_t code);
    string DescriptionAlarm1(uint8_t code);

public:
    typedef struct{
        float Power;
        int DC;
        int AC;
        float Current;
    }InverterData;

    InverterData inverterData;

    ModbusPowerPlant(string _ip_address, int id);
    ModbusPowerPlant();
    bool connect();
    void disconnect();
    void readInvertorsData();
};

#endif //COMMUNICATIONSERVER_MODBUS_POWER_PLANT_H
