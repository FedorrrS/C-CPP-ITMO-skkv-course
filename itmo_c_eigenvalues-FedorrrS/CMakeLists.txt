cmake_minimum_required(VERSION 3.5)
project(itmo_c_eigenvalues-FedorrrS)

set(CMAKE_C_STANDARD 99)

# gcc/g++
set(CMAKE_C_COMPILER /usr/bin/gcc)
# clang
#set(CMAKE_C_COMPILER /usr/bin/clang)

include_directories(include)

set(SOURCE_FILES
        src/algebra.c
        src/matrix.c
        src/householder.c
        src/qr_algorithm.c
        main.c)

add_executable(itmo_c_eigenvalues-FedorrrS ${SOURCE_FILES})
target_link_libraries(itmo_c_eigenvalues-FedorrrS m)