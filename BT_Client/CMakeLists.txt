cmake_minimum_required(VERSION 3.13)
project(BT_Client)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. hash.cpp)

add_executable(x hash.cpp x.cpp torrent.cpp)
