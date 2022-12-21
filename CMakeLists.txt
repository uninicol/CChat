cmake_minimum_required(VERSION 3.22)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
project(CChat C)
add_executable(CChat main.c client.c server.c chat.c chat.h)
find_package(LibreSSL REQUIRED)
target_link_libraries(CChat LibreSSL::TLS)
target_link_libraries(CChat LibreSSL::Crypto)
target_link_libraries(CChat LibreSSL::SSL)


set(CMAKE_C_STANDARD 99)
