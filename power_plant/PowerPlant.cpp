//
// Created by Erik Dvorcak on 2019-03-01.
//

#include "PowerPlant.h"


PowerPlant::PowerPlant(string ip_address, int id) {
    _ip_address = ip_address;
    _id = id;
    _driver = get_driver_instance();
    _con = _driver->connect("tcp://127.0.0.1:3306", "monitoringServer", "enprotech");
    _con->setSchema("monitoring");
}

PowerPlant::PowerPlant() = default;

PowerPlant::~PowerPlant() = default;


std::string PowerPlant::get_ip_address() {
    return _ip_address;
}

void PowerPlant::set_ip_address(const string &_ip_address) {
    PowerPlant::_ip_address = _ip_address;
}

int PowerPlant::get_id() {
    return _id;
}

void PowerPlant::set_id(int _id) {
    PowerPlant::_id = _id;
}

void PowerPlant::writeDataToDB(int invertor_id, double value, string query){
    try {
        _pstmt = _con->prepareStatement(query);
        _pstmt->setInt(1, invertor_id);
        _pstmt->setDouble(2, value);
        _pstmt->executeQuery();
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}
