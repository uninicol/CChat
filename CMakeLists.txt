cmake_minimum_required(VERSION 3.23)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
project(CmakeCCHAT C)
add_executable(CmakeCCHAT main.c client.c client.h server.c server.h host-address.c host-address.h)
find_package(LibreSSL REQUIRED)
target_link_libraries(CmakeCCHAT LibreSSL::TLS)
target_link_libraries(CmakeCCHAT LibreSSL::Crypto)
target_link_libraries(CmakeCCHAT LibreSSL::SSL)


set(CMAKE_C_STANDARD 99)

#set(LD_LIBRARY_PATH "/home/linuxbrew/.linuxbrew/opt/libressl/lib")
#set(CFLAGS "-Wall -g -O0")
#set(LDFLAGS "-ltls -lssl -lcrypto")
