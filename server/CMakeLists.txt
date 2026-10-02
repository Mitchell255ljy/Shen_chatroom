cmake_minimum_required(VERSION 3.10)
project(Shen_Chatroom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(NLOHMANN_JSON_DIR ${CMAKE_SOURCE_DIR}/third_party)
if(NOT EXISTS "${NLOHMANN_JSON_DIR}/json.hpp")
    message(FATAL_ERROR "json.hpp not found in ${NLOHMANN_JSON_DIR}")
endif()

find_package(Threads REQUIRED)

find_path(MYSQL_INCLUDE_DIR
    NAMES mysql/mysql.h
    PATHS /usr/include/mysql /usr/include /usr/local/mysql/include
)
find_library(MYSQL_LIBRARY
    NAMES mysqlclient mariadb
    PATHS /usr/lib64/mysql /usr/lib64 /usr/lib /usr/lib/x86_64-linux-gnu
)

if(NOT MYSQL_INCLUDE_DIR)
    message(FATAL_ERROR "MySQL header (mysql/mysql.h) not found")
endif()
if(NOT MYSQL_LIBRARY)
    message(FATAL_ERROR "MySQL client library not found")
endif()

message(STATUS "MySQL include dir: ${MYSQL_INCLUDE_DIR}")
message(STATUS "MySQL library:     ${MYSQL_LIBRARY}")

set(SERVER_SOURCES
    main.cpp
    ../common/protocol.h
    ../common/message_schema.h
    net/Connection.cpp
    net/ThreadPool.cpp
    net/Service.cpp
    net/SessionManager.cpp
    protocol/Protocol.cpp
    handler/Handler.cpp
    db/MysqlClient.cpp
    db/MysqlPool.cpp
    db/UserDao.cpp
    util/ConfigManager.cpp
)

add_executable(shen_chatroom ${SERVER_SOURCES})

target_include_directories(shen_chatroom
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/third_party
        ${MYSQL_INCLUDE_DIR}
)

target_link_libraries(shen_chatroom PRIVATE
    ${MYSQL_LIBRARY}
    Threads::Threads
)