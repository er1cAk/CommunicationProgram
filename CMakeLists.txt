cmake_minimum_required( VERSION 3.10 )
project( CommunicationServer )

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

set( CMAKE_CXX_STANDARD 14)

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
        constants.h)


include_directories(/usr/local/include)

link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(${PROJECT_NAME} curl mysqlcppconn ${libname}++)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)