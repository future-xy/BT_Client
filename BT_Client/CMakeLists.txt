cmake_minimum_required(VERSION 3.20.1)

if (CMAKE_HOST_WIN32)
    message("Start building!")
else ()
    message(FATAL_ERROR "This project only works on WINDOWS!")
endif ()

project(BT_Client)

set(CMAKE_CXX_STANDARD 14)

add_executable(BT_Client src/main.cpp src/hash.cpp src/torrent.cpp)

target_include_directories(BT_Client 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
