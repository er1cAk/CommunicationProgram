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
        cout << endl;
        while (_res->previous()) {
            cout << "RESULT PW_ID( " << this -> get_id() << " ), ADDRESS: " << _res -> getString("ADDRESS") << endl;
            int id = _res->getInt("INVERTER_ID");
            uint8_t address = _res->getInt("ADDRESS");
            readGridVoltage(id, address);
            readGridPower(id, address);
            readGridCurrent(id, address);
        }
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

void AuroraPowerPlant::readGridVoltage(int id, uint8_t address) {
    try{
        aurora.readDSP(address, GRID_VOLTAGE, GLOBAL_TYPE_GLOBAL);

        if(aurora.dataDsp.TransmissionState == 0){
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO AC_VOLTAGE(INVERTER_ID, VALUE) VALUES(?,?)");
            cout<< "Voltage:" << aurora.dataDsp.Value << endl;
            clearInverterData();
        }
    } catch (AuroraConnectException &ace){
        cout << ace.what() << endl;
    } catch (AuroraSendingRequestException &asre){
        cout << asre.what() << endl;
    } catch (AuroraReceivingResponseException &arre){
        cout << arre.what() << endl;
    }
}

void AuroraPowerPlant::readGridCurrent(int id, uint8_t address) {
    try{
        aurora.readDSP(address, GRID_CURRENT, GLOBAL_TYPE_GLOBAL);
        if(aurora.dataDsp.TransmissionState == 0){
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO CURRENT(INVERTER_ID, VALUE) VALUES(?,?)");
            cout<< "Current:" << aurora.dataDsp.Value << endl;
            clearInverterData();
        }
    } catch (AuroraConnectException &ace){
        cout << ace.what() << endl;
    } catch (AuroraSendingRequestException &asre){
        cout << asre.what() << endl;
    } catch (AuroraReceivingResponseException &arre){
        cout << arre.what() << endl;
    }
}

void AuroraPowerPlant::readGridPower(int id, uint8_t address) {
    try {
        aurora.readDSP(address, GRID_POWER, GLOBAL_TYPE_GLOBAL );
        if(aurora.dataDsp.TransmissionState == 0){
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO POWER(INVERTER_ID, VALUE) VALUES(?,?)");
            cout<< "Power:" << aurora.dataDsp.Value << endl;
            clearInverterData();
        }
    } catch (AuroraConnectException &ace){
        cout << ace.what() << endl;
    } catch (AuroraSendingRequestException &asre){
        cout << asre.what() << endl;
    } catch (AuroraReceivingResponseException &arre){
        cout << arre.what() << endl;
    }
}


void AuroraPowerPlant::readLastFourAlarms(int id, uint8_t address) {

}

void AuroraPowerPlant::clearInverterData() {
    aurora.dataDsp = {};
}

void AuroraPowerPlant::connect() {
    aurora.aurora_connect();
}

void AuroraPowerPlant::disconnect() {
    aurora.aurora_disconnect();
}
