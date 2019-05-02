//
// Created by Erik Dvorcak on 2019-03-27.
//

#ifndef COMMUNICATIONSERVER_AURORAPOWERPLANT_H
#define COMMUNICATIONSERVER_AURORAPOWERPLANT_H

#define AURORA_DEFAULT_PORT 4001

#define GLOBAL_TYPE_MODULE 0
#define GLOBAL_TYPE_GLOBAL 1

#define GRID_VOLTAGE 1
#define GRID_CURRENT 2
#define GRID_POWER 3

#include "../power_plant/PowerPlant.h"
#include "../aurorapp/Aurora.h"

class AuroraPowerPlant: public PowerPlant {
private:
    Aurora aurora = Aurora("", AURORA_DEFAULT_PORT);

    void readGridPower(int id,uint8_t address);
    void readGridVoltage(int id, uint8_t address);
    void readGridCurrent(int id, uint8_t address);
    void readLastFourAlarms(int id, uint8_t address);

    void clearInverterData();

    typedef struct{
        float gridVoltageValue;
        float gridCurrentValue;
        float gridPowerValue;
    }InverterData;

    typedef struct {

    }LastFourAlarms;

public:
    AuroraPowerPlant(string _ip_address, int id);
    AuroraPowerPlant();
    void readInvertersData();
    void connect();
    void disconnect();
};


#endif //COMMUNICATIONSERVER_AURORAPOWERPLANT_H
