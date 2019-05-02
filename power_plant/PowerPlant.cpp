//
// Created by Erik Dvorcak on 2019-03-01.
//

#include "PowerPlant.h"


PowerPlant::PowerPlant(string ip_address, int id) {
    libconfig::Config cfg;

    try{
        cfg.readFile("../config.cfg");
    }catch(const libconfig::FileIOException &fioex){
        std::cerr << "I/O error while reading file." << std::endl;
    }catch(const libconfig::ParseException &pex){
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
    }

    try {
        string db_name= cfg.lookup("DB_NAME");
        string db_url = cfg.lookup("DB_URL");
        string db_pass = cfg.lookup("DB_PASS");
        string db_user = cfg.lookup("DB_USER");
        _ip_address = ip_address;
        _id = id;
        _driver = get_driver_instance();
        _con = _driver->connect(db_url, db_user, db_pass);
        _con->setSchema(db_name);
    } catch (const libconfig::SettingNotFoundException &nfex){
        cerr << "No 'name' setting in configuration file." << endl;
    }
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
