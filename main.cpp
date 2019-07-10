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
#include <string>

#include "modbus_power_plant/Modbus_PowerPlant.h"
#include "aurora_power_plant/AuroraPowerPlant.h"
#include "constants.h"

using namespace std;
using namespace libconfig;

void sendTick(string URL, int PW_ID);

int main() {
    ModbusPowerPlant modbusPowerPlant;
    AuroraPowerPlant auroraPowerPlant;
    Config cfg;

    try {
        cfg.readFile("../config.cfg");
    } catch (const FileIOException &fioex) {
        cerr << "I/O error while reading file." << std::endl;
        return (EXIT_FAILURE);
    } catch (const ParseException &pex) {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
             << " - " << pex.getError() << std::endl;
        return (EXIT_FAILURE);
    }

    try {
        string db_name = cfg.lookup("DB_NAME");
        string db_url = cfg.lookup("DB_URL");
        string db_pass = cfg.lookup("DB_PASS");
        string db_user = cfg.lookup("DB_USER");
        string url = cfg.lookup("SERVER_TICK_URL");
        try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::ResultSet *res;
            sql::PreparedStatement *pstmt;

            driver = get_driver_instance();
            con = driver->connect(db_url, db_user, db_pass);
            con->setSchema(db_name);

            pstmt = con->prepareStatement(
                    "SELECT * FROM POWER_PLANTS WHERE STATUS_ID != 3 AND STATUS_ID != 2 ORDER BY PW_ID DESC");
            res = pstmt->executeQuery();

            res->afterLast();

            while (res->previous()) {

                string IP_ADDRESS = res->getString("IP_ADDRESS");
                int PW_ID = res->getInt("PW_ID");
                int STATUS_ID = res->getInt("STATUS_ID");

                switch (res->getInt("PROTOCOL_ID")) {
                    case 1:
                        modbusPowerPlant = ModbusPowerPlant(IP_ADDRESS, PW_ID);

                        if (modbusPowerPlant.connect()) {
                            if (STATUS_ID != STATUS_ONLINE) {
                                modbusPowerPlant.updatePowerPlantStatus(STATUS_ONLINE);
                            }
                            modbusPowerPlant.readInvertersData();
                            modbusPowerPlant.disconnect();
                        } else {
                            if (STATUS_ID != STATUS_COMMUNICATION_ERROR) {
                                modbusPowerPlant.updatePowerPlantStatus(STATUS_COMMUNICATION_ERROR);
                            }
                            modbusPowerPlant.disconnect();
                        }
                        break;

                    case 2:
                        auroraPowerPlant = AuroraPowerPlant(IP_ADDRESS, PW_ID);

                        if (auroraPowerPlant.connect()) {
                            if (STATUS_ID != STATUS_ONLINE) {
                                auroraPowerPlant.updatePowerPlantStatus(STATUS_ONLINE);
                            }
                            auroraPowerPlant.readInvertersData();
                            auroraPowerPlant.disconnect();
                        } else {
                            if (STATUS_ID != STATUS_COMMUNICATION_ERROR) {
                                auroraPowerPlant.updatePowerPlantStatus(STATUS_COMMUNICATION_ERROR);
                            }
                            auroraPowerPlant.disconnect();
                        }
                        break;
                    default:
                        cout << "Bad protocol" << endl;
                        break;
                }
                sendTick(url, PW_ID);
            }
            delete (con);
        } catch (sql::SQLException &e) {
            cout << "# ERR: SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
    } catch (const SettingNotFoundException &nfex) {
        cerr << "DB_USER or DB_PASS or DB_NAME or DB_URL was not found in config.cfg" << endl;
    }
}

void sendTick(string URL, int PW_ID){
    CURL *curl;
    CURLcode res;
    //TICK, http post request
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        string param = "pwId=" + to_string(PW_ID);

        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param.c_str());

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}