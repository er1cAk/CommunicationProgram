//
// Created by Erik Dvorcak on 2019-03-02.
//

#include "Modbus_PowerPlant.h"
#include "../constants.h"

ModbusPowerPlant::ModbusPowerPlant(string ip_address, int id): PowerPlant(ip_address, id){
    modbus1 = modbus(this -> get_ip_address(), MODBUS_DEFAULT_PORT);
}
ModbusPowerPlant::ModbusPowerPlant() = default;


bool ModbusPowerPlant::connect() {
    return modbus1.modbus_connect();
}

void ModbusPowerPlant::disconnect() {
    try {
        modbus1.modbus_close();
    } catch (modbus_exception &e){
        cout<< e.what() <<endl;
    }
}

void ModbusPowerPlant::readInvertorsData() {
    try {
        _pstmt = _conn->prepareStatement("SELECT * FROM INVERTERS WHERE PW_ID = ? AND STATUS_ID != 3 AND STATUS_ID != 2 ORDER BY ADDRESS DESC");
        _pstmt -> setInt(1, this->get_id());
        _pstmt -> executeUpdate();
        _res = _pstmt -> executeQuery();

        _res->afterLast();
        while (_res->previous()) {
            try {
                modbus1.modbus_set_slave_id(_res->getInt("ADDRESS"));
                int id = _res->getInt("INVERTER_ID");
                int status_id = _res->getInt("STATUS_ID");
                if(readInstantPower(id, _res->getInt("DIVISOR")) > 0){
                    if(status_id != STATUS_ONLINE){
                        this->updateInverterStatus(id, STATUS_ONLINE);
                    }
                    readDcVoltage(id);
                    readCurrent(id, _res->getInt("DIVISOR"));
                }else{
                    if(status_id != STATUS_COMMUNICATION_ERROR) {
                        this->updateInverterStatus(id, STATUS_COMMUNICATION_ERROR);
                    }
                    writeDataToDB(id, 0, "INSERT INTO DC_VOLTAGE(INVERTER_ID, VALUE ) VALUES(?,?)");
                    writeDataToDB(id, 0, "INSERT INTO AC_VOLTAGE(INVERTER_ID, VALUE ) VALUES(?,?)");
                    writeDataToDB(id, 0, "INSERT INTO CURRENT(INVERTER_ID, VALUE ) VALUES(?,?)");
                }
                checkAlarms(id);
            } catch (modbus_exception &e) {
                cout << e.what() << endl;
            }
        }
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

ssize_t ModbusPowerPlant::readInstantPower(int inverter_id, double divisor) {
    uint16_t buffer[1];
    try {
        if(modbus1.modbus_read_input_registers(INSTANT_POWER, COUNT_OF_READING_REGISTERS, buffer) == 1){
            double power = (buffer[0] / divisor) * 1000;
            writeDataToDB(inverter_id, power, "INSERT INTO POWER(INVERTER_ID, VALUE ) VALUES(?,?)");
        }else{
            writeDataToDB(inverter_id, 0, "INSERT INTO POWER(INVERTER_ID, VALUE ) VALUES(?,?)");
            return -1;
        }
    } catch (modbus_exception &e){
        cout << e.what() << endl;
    }
    return 1;
}

int ModbusPowerPlant::readDcVoltage(int inverter_id) {
    uint16_t buffer[1];
    try {
        modbus1.modbus_read_input_registers(DC_VOLTAGE, COUNT_OF_READING_REGISTERS, buffer);
        writeDataToDB(inverter_id, buffer[0], "INSERT INTO DC_VOLTAGE(INVERTER_ID, VALUE ) VALUES(?,?)");
    } catch (modbus_exception &e){
        cout << e.what() << endl;
    }
}

int ModbusPowerPlant::readCurrent(int inverter_id, double divisor) {
    uint16_t buffer[1];
    try {
        modbus1.modbus_read_input_registers(CURRENT, COUNT_OF_READING_REGISTERS, buffer);
        writeDataToDB(inverter_id, ((double)(buffer[0]/divisor)), "INSERT INTO CURRENT(INVERTER_ID, VALUE ) VALUES(?,?)");
    } catch (modbus_exception &e){
        cout << e.what() << endl;
    }
}

void ModbusPowerPlant::checkAlarms(int inverter_id) {
    readAlarms(inverter_id);
//    readAlarms1(inverter_id);
    readAlarms2(inverter_id);
}

int ModbusPowerPlant::readAlarms(int inverter_id) {
    uint16_t buffer[1];
    modbus1.modbus_read_input_registers(alarms, COUNT_OF_READING_REGISTERS, buffer);
    for (int i = 16; i > 0; i--) {
        if ((buffer[0] >> 15) == 1) {
            writeAlarmToDB(inverter_id, i, DescriptionAlarm(i));
        }
    }
}

int ModbusPowerPlant::readAlarms1(int inverter_id) {
    uint16_t buffer[1];
    modbus1.modbus_read_input_registers(alarms1, COUNT_OF_READING_REGISTERS, buffer);
    for (int i = 16; i > 0; i--) {
        if ((buffer[0] >> 15) == 1) {
            writeAlarmToDB(inverter_id, 100 + i, DescriptionAlarm1(i));
        }
    }
}

int ModbusPowerPlant::readAlarms2(int inverter_id) {
    uint16_t buffer[1];
    modbus1.modbus_read_input_registers(alarms2, COUNT_OF_READING_REGISTERS, buffer);
    for (int i = 16; i > 0; i--) {
        if ((buffer[0] >> 15) == 1) {
            writeAlarmToDB(inverter_id, 200 + i, DescriptionAlarm2(i));
        }
    }
}

string ModbusPowerPlant::DescriptionAlarm(uint8_t code) {
    switch (code) {
        case 0:
            return "Over current";
        case 1:
            return "Over voltage on DC bus";
        case 2:
            return "Under voltage on DC bus";
        case 3:
            return "Reserve";
        case 4:
            return "False";
        case 5:
            return "Over voltage protection failure";
        case 6:
            return "Over voltage protection failure";
        case 7:
            return "False";
        case 8:
            return "Over temperature warning";
        case 9:
            return "System bus error";
        case 10:
            return "LCL Fan warning active";
        case 11:
            return "Follower in failure";
        case 12:
            return "Insulation warning active";
        case 13:
            return "M/F switch fault";
        case 14:
            return "Frequency limit";
        case 15:
            return "Supply voltage limit";
        default:
            break;
    }
}

string ModbusPowerPlant::DescriptionAlarm1(uint8_t code) {
    return null
}

string ModbusPowerPlant::DescriptionAlarm2(uint8_t code) {
    switch (code) {
        case 0:
            return "False";
        case 1:
            return "F29 Thermistor warning";
        case 2:
            return "False";
        case 3:
            return "Supply phase warning";
        case 4:
            return "False";
        case 5:
            return "Over voltage protection failure";
        case 6:
            return "Over voltage protection failure";
        case 7:
            return "False";
        case 8:
            return "Over temperature warning";
        case 9:
            return "System bus error";
        case 10:
            return "LCL Fan warning active";
        case 11:
            return "Follower in failure";
        case 12:
            return "Insulation warning active";
        case 13:
            return "M/F switch fault";
        case 14:
            return "Frequency limit";
        case 15:
            return "Supply voltage limit";
        default:
            break;
    }
}