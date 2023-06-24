cmake_minimum_required(VERSION 3.25)
project(itmo_cpp_ln_FedorrrS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(SOURCE_FILES
        main.cpp src/LN.cpp)

add_executable(itmo_cpp_ln_FedorrrS ${SOURCE_FILES})