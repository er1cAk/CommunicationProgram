//
// Created by Erik Dvorcak on 2019-03-01.
//

#ifndef COMMUNICATIONSERVER_POWER_PLANT_H
#define COMMUNICATIONSERVER_POWER_PLANT_H

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cppconn/resultset.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <libconfig.h++>

using namespace std;

class PowerPlant {
private:
    string _ip_address;
    int _id;

    sql::Driver *_driver;

protected:

    sql::Connection *_conn;
    sql::Statement *_stmt;
    sql::ResultSet *_res;
    sql::PreparedStatement *_pstmt;
    bool _connectivity;

    void set_ip_address(const string &_ip_address);
    void set_id(int _id);
    void writeDataToDB(int inverter_id, double value, string query);
    void writeAlarmToDB(int inverter_id, uint8_t code, string description);
    void updateInverterStatus(int inverter_id, int status);

public:
    PowerPlant(string _ip_address, int id);
    PowerPlant();
    ~PowerPlant();

    string get_ip_address();
    int get_id();
    void updatePowerPlantStatus(int status);

};


#endif //COMMUNICATIONSERVER_POWER_PLANT_H
