//
// Created by Erik Dvorcak on 2019-03-27.
//

#include "AuroraPowerPlant.h"

AuroraPowerPlant::AuroraPowerPlant(string _ip_address, int id) : PowerPlant(_ip_address, id) {
    aurora = Aurora(this->get_ip_address(), AURORA_DEFAULT_PORT);
}

AuroraPowerPlant::AuroraPowerPlant() = default;

void AuroraPowerPlant::readInvertersData() {
    try {
        _pstmt = _con->prepareStatement("SELECT * FROM INVERTERS WHERE PW_ID = ? ORDER BY ADDRESS DESC");
        _pstmt -> setInt(1, this->get_id());
        _pstmt -> executeUpdate();
        _res = _pstmt -> executeQuery();

        _res->afterLast();
        while (_res->previous()) {
            int id = _res->getInt("INVERTER_ID");
            uint8_t address = _res->getInt("ADDRESS");
            try {
                aurora.readState(address);
                if (aurora.dataState.InverterState == 2) {

                    cout << "Alarm state: " << aurora.dataState.AlarmState << endl;
                    cout << "Inverter state: " << aurora.dataState.InverterState << endl;
                    cout << "global state: " << aurora.dataState.GlobalState << endl;

                    readGridVoltage(id, address);
                    readGridPower(id, address);
                    readGridCurrent(id, address);
                }else{
                    cout << "Inverter state is not running!" << endl;
                }
            } catch (AuroraSendingRequestException &auroraSendingRequestException){
                cout << auroraSendingRequestException.what()<<endl;
            }catch (AuroraReceivingResponseException &auroraReceivingResponseException){
                cout << auroraReceivingResponseException.what()<<endl;
            }
        }
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

void AuroraPowerPlant::readGridVoltage(int id, uint8_t address) {
    if(_connectivity){
        aurora.readDSP(address, GRID_VOLTAGE, GLOBAL_TYPE_GLOBAL);
        if(aurora.dataDsp.TransmissionState == 0){
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO AC_VOLTAGE(INVERTER_ID, VALUE) VALUES(?,?)");
            cout<< "Voltage:" << aurora.dataDsp.Value << endl;
            clearInverterData();
        }
    }
}

void AuroraPowerPlant::readGridCurrent(int id, uint8_t address) {
        if(_connectivity){
            aurora.readDSP(address, GRID_CURRENT, GLOBAL_TYPE_GLOBAL);
            if(aurora.dataDsp.TransmissionState == 0){
                writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO CURRENT(INVERTER_ID, VALUE) VALUES(?,?)");
                cout<< "Current:" << aurora.dataDsp.Value << endl;
                clearInverterData();
            }
        }
}

void AuroraPowerPlant::readGridPower(int id, uint8_t address) {
    if(_connectivity){
        aurora.readDSP(address, GRID_POWER, GLOBAL_TYPE_GLOBAL );
        if(aurora.dataDsp.TransmissionState == 0){
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO POWER(INVERTER_ID, VALUE) VALUES(?,?)");
            cout<< "Power:" << aurora.dataDsp.Value << endl;
            clearInverterData();
        }
    }
}


void AuroraPowerPlant::readLastFourAlarms(int id, uint8_t address) {

}

void AuroraPowerPlant::clearInverterData() {
    aurora.dataDsp = {};
}

bool AuroraPowerPlant::connect() {
    try {
        aurora.aurora_connect();
    } catch (AuroraConnectException &auroraConnectException){
        return false;
    }
    return true;
}

void AuroraPowerPlant::disconnect() {
    aurora.aurora_disconnect();
}
