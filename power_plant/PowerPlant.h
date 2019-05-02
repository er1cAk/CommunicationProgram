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

    sql::Connection *_con;
    sql::Statement *_stmt;
    sql::ResultSet *_res;
    sql::PreparedStatement *_pstmt;

    void set_ip_address(const string &_ip_address);
    void set_id(int _id);
    void writeDataToDB(int invertor_id, double value, string query);

public:
    PowerPlant(string _ip_address, int id);
    PowerPlant();
    ~PowerPlant();

    std::string get_ip_address();
    int get_id();

};


#endif //COMMUNICATIONSERVER_POWER_PLANT_H
