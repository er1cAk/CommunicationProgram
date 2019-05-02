cmake_minimum_required( VERSION 3.10 )
project( CommunicationServer )

set( CMAKE_CXX_STANDARD 14)

set( MYSQL_CONNECTOR_DIR
        /usr/local/opt/mysql-connector-c++ )

FIND_PATH(CONFIG_INCLUDE_DIR libconfig.h /usr/include /usr/local/include)

FIND_LIBRARY(CONFIG_LIBRARY NAMES config PATH /usr/lib /usr/local/lib)

set( SOURCE_FILES
        main.cpp

        modbuspp/modbus.h
        modbuspp/modbus.cpp
        modbuspp/modbus_exception.h

        power_plant/PowerPlant.cpp
        power_plant/PowerPlant.h

        modbus_power_plant/Modbus_PowerPlant.cpp
        modbus_power_plant/Modbus_PowerPlant.h

        aurorapp/Aurora.cpp
        aurorapp/Aurora.h
        aurorapp/AuroraException.h

        aurora_power_plant/AuroraPowerPlant.cpp
        aurora_power_plant/AuroraPowerPlant.h
)


include_directories(/usr/local/include)

link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} curl mysqlcppconn)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)