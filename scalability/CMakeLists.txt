cmake_minimum_required(VERSION 3.15)
project(h4d1)

set(CMAKE_CXX_STANDARD 14)

add_executable(h4d1 main.cpp serversocket.h GeneralException.h threadpool.h client_test.cpp)