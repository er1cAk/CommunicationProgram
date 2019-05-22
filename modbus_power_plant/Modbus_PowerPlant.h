//
// Created by Erik Dvorcak on 2019-03-02.
//

#ifndef COMMUNICATIONSERVER_MODBUS_POWER_PLANT_H
#define COMMUNICATIONSERVER_MODBUS_POWER_PLANT_H

#define COUNT_OF_READING_REGISTERS 1

#define INSTANT_POWER 1949
#define DC_VOLTAGE 1107
#define AC_VOLTAGE 1200
#define CURRENT 1103

#define alarms1 1172
#define alarms2 1173

#define alarms 1174

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
    int readAcVoltage(int inverter_id);
    int readCurrent(int inverter_id, double divisor);

//    void writeDataToDB(int invertor_id, double value, string query);

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
