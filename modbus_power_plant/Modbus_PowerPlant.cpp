//
// Created by Erik Dvorcak on 2019-03-02.
//

#include "Modbus_PowerPlant.h"

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
        _pstmt = _con->prepareStatement("SELECT * FROM INVERTERS WHERE PW_ID = ? ORDER BY ADDRESS DESC");
        _pstmt -> setInt(1, this->get_id());
        _pstmt -> executeUpdate();
        _res = _pstmt -> executeQuery();

        _res->afterLast();
        cout << endl;
        while (_res->previous()) {
            cout << "RESULT PW_ID( " << this -> get_id() << " ), ADDRESS: " << _res -> getString("ADDRESS") << endl;
            try {
                modbus1.modbus_set_slave_id(_res->getInt("ADDRESS"));
                int id = _res->getInt("INVERTER_ID");
                readInstantPower(id, _res->getInt("DIVISOR"));
                readDcVoltage(id);
                readAcVoltage(id);
                readCurrent(id, _res->getInt("DIVISOR"));
            } catch (modbus_exception &e) {
                cout << e.what() << endl;
            }
        }
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

ssize_t ModbusPowerPlant::readInstantPower(int invertor_id, double divisor) {
    uint16_t buffer[1];
    try {
        modbus1.modbus_read_input_registers(INSTANT_POWER, COUNT_OF_READING_REGISTERS, buffer);
        double power = (buffer[0] / divisor) * 1000;
        writeDataToDB(invertor_id, power, "INSERT INTO POWER(INVERTER_ID, VALUE ) VALUES(?,?)");
    } catch (modbus_exception &e){
        cout << e.what() << endl;
        return -1;
    }
    return 0;
}

int ModbusPowerPlant::readDcVoltage(int invertor_id) {
    uint16_t buffer[1];
    try {
        modbus1.modbus_read_input_registers(DC_VOLTAGE, COUNT_OF_READING_REGISTERS, buffer);
        writeDataToDB(invertor_id, buffer[0], "INSERT INTO DC_VOLTAGE(INVERTER_ID, VALUE ) VALUES(?,?)");
        cout << "DC_VOLTAGE: " << buffer[0] << endl;
    } catch (modbus_exception &e){
        cout << e.what() << endl;
    }
}
int ModbusPowerPlant::readAcVoltage(int invertor_id) {
    uint16_t buffer[1];
    try {
        modbus1.modbus_read_input_registers(AC_VOLTAGE, COUNT_OF_READING_REGISTERS, buffer);
        writeDataToDB(invertor_id, buffer[0], "INSERT INTO AC_VOLTAGE(INVERTER_ID, VALUE ) VALUES(?,?)");
        cout << "AC_VOLTAGE: " << buffer[0] << endl;
    } catch (modbus_exception &e){
        cout << e.what() << endl;
    }
}
int ModbusPowerPlant::readCurrent(int invertor_id, double divisor) {
    uint16_t buffer[1];
    try {
        modbus1.modbus_read_input_registers(CURRENT, COUNT_OF_READING_REGISTERS, buffer);
        writeDataToDB(invertor_id, ((double)(buffer[0]/divisor)), "INSERT INTO CURRENT(INVERTER_ID, VALUE ) VALUES(?,?)");
        cout << "CURRENT:  " << ((double)(buffer[0]/divisor)) << endl;
    } catch (modbus_exception &e){
        cout << e.what() << endl;
    }
}