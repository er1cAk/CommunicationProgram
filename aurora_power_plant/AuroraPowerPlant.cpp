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
        _pstmt = _conn->prepareStatement("SELECT * FROM INVERTERS WHERE PW_ID = ? AND STATUS_ID != 3 ORDER BY ADDRESS DESC");
        _pstmt -> setInt(1, this->get_id());
        _pstmt -> executeUpdate();
        _res = _pstmt -> executeQuery();

        _res->afterLast();
        while (_res->previous()) {
            int id = _res->getInt("INVERTER_ID");
            int status_id = _res->getInt("STATUS_ID");
            int address = _res->getInt("ADDRESS");
            cout<<endl;
            try {
                aurora.readState(address);
                if (aurora.dataState.InverterState == STATE_RUNNING) {
                    readGridVoltage(id, address);
                    readGridPower(id, address);
                    readGridCurrent(id, address);
                }else{
                    cout << "Inverter state is not running!" << aurora.dataState.GlobalState << endl;
                }
            } catch (exception &e){
                if(status_id != 4)
                    updateInverterStatus(id, "UPDATE INVERTER SET STATUS_ID=4 WHERE INVERTER_ID=?");
                cout << e.what() << endl;
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

void AuroraPowerPlant::clearInverterData() {
    aurora.dataDsp = {};
}

bool AuroraPowerPlant::connect() {
    return aurora.aurora_connect();
}

void AuroraPowerPlant::disconnect() {
    aurora.aurora_disconnect();
}
