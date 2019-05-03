#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>
#include <cppconn/resultset.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <curl/curl.h>

#include "modbuspp/modbus.h"
#include "modbuspp/modbus_exception.h"
#include "modbus_power_plant/Modbus_PowerPlant.h"
#include "aurorapp/Aurora.h"
#include "aurora_power_plant/AuroraPowerPlant.h"

using namespace std;
using namespace libconfig;

void onlyForQuickChanges(sql::Connection *conn);

int main() {
    ModbusPowerPlant modbusPowerPlant;
    AuroraPowerPlant auroraPowerPlant;
    Config cfg;
    CURL *curl;
    CURLcode res;

    try{
        cfg.readFile("../config.cfg");
    }catch(const FileIOException &fioex){
        std::cerr << "I/O error while reading file." << std::endl;
        return(EXIT_FAILURE);
    }catch(const ParseException &pex){
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return(EXIT_FAILURE);
    }

    try {
        string db_name= cfg.lookup("DB_NAME");
        string db_url = cfg.lookup("DB_URL");
        string db_pass = cfg.lookup("DB_PASS");
        string db_user = cfg.lookup("DB_USER");
//        try {
//            sql::Driver *driver;
//            sql::Connection *con;
//            sql::Statement *stmt;
//            sql::ResultSet *res;
//            sql::PreparedStatement *pstmt;
//
//            driver = get_driver_instance();
//            con = driver->connect(db_url, db_user, db_pass);
//            con->setSchema(db_name);
////            onlyForQuickChanges(con);
//
//            pstmt = con->prepareStatement("SELECT * FROM POWER_PLANTS WHERE STATUS_ID != 3 ORDER BY PW_ID DESC");
//            res = pstmt->executeQuery();
//
//            res->afterLast();
//            while (res->previous()) {
//                switch(res->getInt("PROTOCOL_ID")){
//                    case 1:
//                        modbusPowerPlant = ModbusPowerPlant(res->getString("IP_ADDRESS"), res->getInt("PW_ID"));
//                        if(modbusPowerPlant.connect()) {
//                            modbusPowerPlant.readInvertorsData();
//                            modbusPowerPlant.disconnect();
//                        } else{
//                            modbusPowerPlant.disconnect();
//                        }
//                        break;
//                    case 2:
//
//                        auroraPowerPlant = AuroraPowerPlant(res->getString("IP_ADDRESS"), res->getInt("PW_ID"));
//
//                        if(auroraPowerPlant.connect()){
//                            auroraPowerPlant.readInvertersData();
//                            auroraPowerPlant.disconnect();
//                        }else{
//                            auroraPowerPlant.disconnect();
//                        }
//                        break;
//                    default:
//                        cout << "Bad protocol" << endl;
//                        break;
//                }
//            }
//            delete(con);
//        } catch (sql::SQLException &e){
//            cout << "# ERR: SQLException in " << __FILE__;
//            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
//            cout << "# ERR: " << e.what();
//            cout << " (MySQL error code: " << e.getErrorCode();
//            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
//        }

        //TICK, http post request
        curl = curl_easy_init();
        if(curl) {
            /* First set the URL that is about to receive our POST. This URL can
               just as well be a https:// URL if that is what should receive the
               data. */
            string server_url = cfg.lookup("SERVER_TICK_URL");
            curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
            /* Now specify the POST data */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));

            /* always cleanup */
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    } catch (const SettingNotFoundException &nfex){
        cerr << "DB_USER or DB_PASS or DB_NAME or DB_URL was not found in config.cfg" << endl;
    }
}
void onlyForQuickChanges(sql::Connection *conn) {
    sql::PreparedStatement *ps;

    for(int idx = 1; idx < 21; idx++ ){
        ps = conn->prepareStatement("INSERT INTO INVERTERS(ADDRESS,DIVISOR, PW_ID, STATUS_ID) VALUES(?,?,?,?)");
        ps->setInt(1, idx);
        ps->setInt(2, 10);
        ps->setInt(3, 6);
        ps->setInt(4, 1);
        ps->executeQuery();
    }
}