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
        _pstmt = _conn->prepareStatement(
                "SELECT INVERTER_ID, ADDRESS, STATUS_ID FROM INVERTERS WHERE PW_ID = ? AND STATUS_ID != 3 ORDER BY ADDRESS DESC");
        _pstmt->setInt(1, this->get_id());
        _pstmt->executeUpdate();
        _res = _pstmt->executeQuery();

        _res->afterLast();
        while (_res->previous()) {
            int id = _res->getInt("INVERTER_ID");
            int status_id = _res->getInt("STATUS_ID");
            int address = _res->getInt("ADDRESS");
            try {
                aurora.readState(address);
                if (aurora.dataState.InverterState == STATE_RUNNING) {
                    if (status_id != STATUS_ONLINE) {
                        this->updateInverterStatus(id, STATUS_ONLINE);
                    }
                    readGridVoltage(id, address);
                    readGridPower(id, address);
                    readGridCurrent(id, address);
                } else {
                    if (aurora.dataState.InverterState == STATE_STANDBY ||
                        aurora.dataState.GlobalState == STATE_WAIT_SUN) {
                        this->updateInverterStatus(id, STATUS_STAND_BY);
                    }
                    if (aurora.dataState.AlarmState > 0 && aurora.dataState.AlarmState < 65) {
                        this->writeAlarmToDB(id, aurora.dataState.AlarmState,
                                             DescriptionAlarmState(aurora.dataState.AlarmState));
                        if (status_id != STATUS_IN_ERROR) {
                            this->updateInverterStatus(id, STATUS_IN_ERROR);
                        }
                    } else {
                        if (status_id == STATUS_IN_ERROR) {
                            this->updateInverterStatus(id, STATUS_ONLINE);
                        }
                    }
                }
            } catch (exception &e) {
                if (status_id != 4)
                    this->updateInverterStatus(id, STATUS_COMMUNICATION_ERROR);
            }
        }
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

void AuroraPowerPlant::readGridVoltage(int id, uint8_t address) {
    if (_connectivity) {
        aurora.readDSP(address, GRID_VOLTAGE, GLOBAL_TYPE_GLOBAL);
        if (aurora.dataDsp.TransmissionState == 0) {
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO DC_VOLTAGE(INVERTER_ID, VALUE) VALUES(?,?)");
            clearInverterData();
        }
    }
}

void AuroraPowerPlant::readGridCurrent(int id, uint8_t address) {
    if (_connectivity) {
        aurora.readDSP(address, GRID_CURRENT, GLOBAL_TYPE_GLOBAL);
        if (aurora.dataDsp.TransmissionState == 0) {
            writeDataToDB(id, aurora.dataDsp.Value, "INSERT INTO CURRENT(INVERTER_ID, VALUE) VALUES(?,?)");
            clearInverterData();
        }
    }
}

void AuroraPowerPlant::readGridPower(int id, uint8_t address) {
    if (_connectivity) {
        aurora.readDSP(address, GRID_POWER, GLOBAL_TYPE_GLOBAL);
        if (aurora.dataDsp.TransmissionState == 0) {
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
    switch (id) {
        case (uint8_t) 2:
            return ("Input OC");
        case (uint8_t) 3:
            return ("Input UV");
        case (uint8_t) 4:
            return ("Input OV");
        case (uint8_t) 5:
            return ("Sun Low");
        case (uint8_t) 6:
            return ("No Parameters");
        case (uint8_t) 7:
            return ("Bulk OV");
        case (uint8_t) 8:
            return ("Comm.Error");
        case (uint8_t) 9:
            return ("Output OC");
        case (uint8_t) 10:
            return ("IGBT Sat");
        case (uint8_t) 11:
            return ("Bulk UV");
        case (uint8_t) 12:
            return ("Internal error");
        case (uint8_t) 13:
            return ("Grid Fail");
        case (uint8_t) 14:
            return ("Bulk Low");
        case (uint8_t) 15:
            return ("Ramp Fail");
        case (uint8_t) 16:
            return ("Dc / Dc Fail");
        case (uint8_t) 17:
            return ("Wrong Mode");
        case (uint8_t) 18:
            return ("Ground Fault");
        case (uint8_t) 19:
            return ("Over Temp.");
        case (uint8_t) 20:
            return ("Bulk Cap Fail");
        case (uint8_t) 21:
            return ("Inverter Fail");
        case (uint8_t) 22:
            return ("Start Timeout");
        case (uint8_t) 23:
            return ("Ground Fault");
        case (uint8_t) 24:
            return ("Degauss error");
        case (uint8_t) 25:
            return ("Ileak sens.fail");
        case (uint8_t) 26:
            return ("DcDc Fail");
        case (uint8_t) 27:
            return ("Self Test Error 1");
        case (uint8_t) 28:
            return ("Self Test Error 2");
        case (uint8_t) 29:
            return ("Self Test Error 3");
        case (uint8_t) 30:
            return ("Self Test Error 4");
        case (uint8_t) 31:
            return ("DC inj error");
        case (uint8_t) 32:
            return ("Grid OV");
        case (uint8_t) 33:
            return ("Grid UV");
        case (uint8_t) 34:
            return ("Grid OF");
        case (uint8_t) 35:
            return ("Grid UF");
        case (uint8_t) 36:
            return ("Z grid Hi");
        case (uint8_t) 37:
            return ("Internal error");
        case (uint8_t) 38:
            return ("Riso Low");
        case (uint8_t) 39:
            return ("Vref Error");
        case (uint8_t) 40:
            return ("Error Meas V");
        case (uint8_t) 41:
            return ("Error Meas ");
        case (uint8_t) 42:
            return ("Error Meas Z");
        case (uint8_t) 43:
            return ("Error Meas Ileak");
        case (uint8_t) 44:
            return ("Error Read V");
        case (uint8_t) 45:
            return ("Error Read I");
        case (uint8_t) 46:
            return ("Table fail");
        case (uint8_t) 47:
            return ("Fan Fail");
        case (uint8_t) 48:
            return ("UTH");
        case (uint8_t) 49:
            return ("Interlock fail");
        case (uint8_t) 50:
            return ("Remote Off");
        case (uint8_t) 51:
            return ("Vout Avg errror");
        case (uint8_t) 52:
            return ("Battery low");
        case (uint8_t) 53:
            return ("Clk fail");
        case (uint8_t) 54:
            return ("Input UC");
        case (uint8_t) 55:
            return ("Zero Power");
        case (uint8_t) 56:
            return ("Fan Stucked");
        case (uint8_t) 57:
            return ("DC Switch Open");
        case (uint8_t) 58:
            return ("Tras Switch Open");
        case (uint8_t) 59:
            return ("AC Switch Open");
        case (uint8_t) 60:
            return ("Bulk UV");
        case (uint8_t) 61:
            return ("Autoexclusion");
        case (uint8_t) 62:
            return ("Grid df / dt");
        case (uint8_t) 63:
            return ("Den switch Open");
        case (uint8_t) 64:
            return ("Jbox fail");
        default:
            break;
    }
}