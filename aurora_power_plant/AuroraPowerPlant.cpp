//
// Created by Erik Dvorcak on 2019-03-27.
//

#include "AuroraPowerPlant.h"
#include "../constants.h"

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
            try {
                aurora.readState(address);
                if (aurora.dataState.InverterState == STATE_RUNNING) {
                    if(status_id != STATUS_ONLINE){
                        this->updateInverterStatus(id, STATUS_ONLINE);
                    }
                    readGridVoltage(id, address);
                    readGridPower(id, address);
                    readGridCurrent(id, address);
                }else{
//                    cout<<aurora.dataState.InverterState<< aurora.dataState.AlarmState<<endl;
                    if(aurora.dataState.InverterState == STATE_STANDBY || aurora.dataState.GlobalState == STATE_WAIT_SUN){
                        this->updateInverterStatus(id, STATUS_STAND_BY);
                    }
                    if(aurora.dataState.AlarmState > 0){
                        this->writeAlarmToDB(id, aurora.dataState.AlarmState, DescriptionAlarmState(aurora.dataState.AlarmState));
                    }
                }
            } catch (exception &e){
                if(status_id != 4)
                    this->updateInverterStatus(id, STATUS_COMMUNICATION_ERROR);
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
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO DC_VOLTAGE(INVERTER_ID, VALUE) VALUES(?,?)");
            clearInverterData();
        }
    }
}

void AuroraPowerPlant::readGridCurrent(int id, uint8_t address) {
        if(_connectivity){
            aurora.readDSP(address, GRID_CURRENT, GLOBAL_TYPE_GLOBAL);
            if(aurora.dataDsp.TransmissionState == 0){
                writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO CURRENT(INVERTER_ID, VALUE) VALUES(?,?)");
                clearInverterData();
            }
        }
}

void AuroraPowerPlant::readGridPower(int id, uint8_t address) {
    if(_connectivity){
        aurora.readDSP(address, GRID_POWER, GLOBAL_TYPE_GLOBAL );
        if(aurora.dataDsp.TransmissionState == 0){
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO POWER(INVERTER_ID, VALUE) VALUES(?,?)");
            clearInverterData();
        }
    }
}

void AuroraPowerPlant::clearInverterData() {
    aurora.dataDsp = {};
    aurora.dataState = {};
}

bool AuroraPowerPlant::connect() {
    return aurora.aurora_connect();
}

void AuroraPowerPlant::disconnect() {
    aurora.aurora_disconnect();
}

string AuroraPowerPlant::DescriptionAlarmState(uint8_t id) {
    switch (id){
        case 2:
            return ("Input OC");
        case 3:
            return ("Input UV");
        case 4:
            return ("Input OV");
        case 5:
            return ("Sun Low");
        case 6:
            return ("No Parameters");
        case 7:
            return ("Bulk OV");
        case 8:
            return ("Comm.Error");
        case 9:
            return ("Output OC");
        case 10:
            return ("IGBT Sat");
        case 11:
            return ("Bulk UV");
        case 12:
            return ("Internal error");
        case 13:
            return ("Grid Fail");
        case 14:
            return ("Bulk Low");
        case 15:
            return ("Ramp Fail");
        case 16:
            return ("Dc / Dc Fail");
        case 17:
            return ("Wrong Mode");
        case 18:
            return ("Ground Fault");
        case 19:
            return ("Over Temp.");
        case 20:
            return ("Bulk Cap Fail");
        case 21:
            return ("Inverter Fail");
        case 22:
            return ("Start Timeout");
        case 23:
            return ("Ground Fault");
        case 24:
            return ("Degauss error");
        case 25:
            return ("Ileak sens.fail");
        case 26:
            return ("DcDc Fail");
        case 27:
            return ("Self Test Error 1");
        case 28:
            return ("Self Test Error 2");
        case 29:
            return ("Self Test Error 3");
        case 30:
            return ("Self Test Error 4");
        case 31:
            return ("DC inj error");
        case 32:
            return ("Grid OV");
        case 33:
            return ("Grid UV");
        case 34:
            return ("Grid OF");
        case 35:
            return ("Grid UF");
        case 36:
            return ("Z grid Hi");
        case 37:
            return ("Internal error");
        case 38:
            return ("Riso Low");
        case 39:
            return ("Vref Error");
        case 40:
            return ("Error Meas V");
        case 41:
            return ("Error Meas ");
        case 42:
            return ("Error Meas Z");
        case 43:
            return ("Error Meas Ileak");
        case 44:
            return ("Error Read V");
        case 45:
            return ("Error Read I");
        case 46:
            return ("Table fail");
        case 47:
            return ("Fan Fail");
        case 48:
            return ("UTH");
        case 49:
            return ("Interlock fail");
        case 50:
            return ("Remote Off");
        case 51:
            return ("Vout Avg errror");
        case 52:
            return ("Battery low");
        case 53:
            return ("Clk fail");
        case 54:
            return ("Input UC");
        case 55:
            return ("Zero Power");
        case 56:
            return ("Fan Stucked");
        case 57:
            return ("DC Switch Open");
        case 58:
            return ("Tras Switch Open");
        case 59:
            return ("AC Switch Open");
        case 60:
            return ("Bulk UV");
        case 61:
            return ("Autoexclusion");
        case 62:
            return ("Grid df / dt");
        case 63:
            return ("Den switch Open");
        case 64:
            return ("Jbox fail");
        default: break;
    }
}