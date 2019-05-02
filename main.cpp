#include <iostream>
#include <mysql_driver.h>
#include <sstream>
#include <libconfig.h++>

#include "modbuspp/modbus.h"
#include "modbuspp/modbus_exception.h"
#include "modbus_power_plant/Modbus_PowerPlant.h"
#include "aurorapp/Aurora.h"
#include "aurora_power_plant/AuroraPowerPlant.h"

using namespace std;

void onlyForQuickChanges(sql::Connection *conn);

int main() {
    ModbusPowerPlant modbusPowerPlant;
    AuroraPowerPlant auroraPowerPlant;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;
        sql::PreparedStatement *pstmt;

        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "monitoringServer", "enprotech");
        con->setSchema("monitoring");

        pstmt = con->prepareStatement("SELECT * FROM POWER_PLANTS WHERE STATUS_ID != 3 ORDER BY PW_ID DESC");
        res = pstmt->executeQuery();

        res->afterLast();
        while (res->previous()) {
            switch(res->getInt("PROTOCOL_ID")){
                case 1:
                    modbusPowerPlant = ModbusPowerPlant(res->getString("IP_ADDRESS"), res->getInt("PW_ID"));
                    modbusPowerPlant.connect();
                    modbusPowerPlant.readInvertorsData();
                    modbusPowerPlant.disconnect();
                    break;
                case 2:
                    auroraPowerPlant = AuroraPowerPlant(res->getString("IP_ADDRESS"), res->getInt("PW_ID"));
                    auroraPowerPlant.connect();
                    auroraPowerPlant.readInvertersData();
                    auroraPowerPlant.disconnect();
                default:
                    cout << "Bad protocol" << endl;
                    break;
            }
        }
        delete(con);
    } catch (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}
void onlyForQuickChanges(sql::Connection *conn) {
    sql::PreparedStatement *ps;

    for(int idx = 1; idx < 21; idx++ ){
        ps = conn->prepareStatement("INSERT INTO INVERTORS(SLAVE_ID, PW_ID) VALUES(?,?)");
        ps->setInt(1, idx);
        ps->setInt(2, 7);
        ps->executeQuery();
    }
}