cmake_minimum_required(VERSION 3.19)
project(Database)

set(CMAKE_CXX_STANDARD 17)

add_executable(Database main.cpp VirtualDisk.cpp VirtualDisk.h try.cpp BPTree.cpp BPTree.h Node.h Node.cpp)

set(Boost_INCLUDE_DIR C:/MinGW/include/)
set(Boost_LIBRARY_DIR C:/MinGW/lib/)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})