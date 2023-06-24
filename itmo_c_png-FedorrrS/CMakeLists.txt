cmake_minimum_required(VERSION 3.25)
project(itmo_c_png_FedorrrS C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-lz -ldeflate -lisal")
set(CMAKE_EXE_LINKER_FLAGS "-lz -ldeflate -lisal")

set(SOURCE_FILES
        main.c src/png_utils.c src/pnm_utils.c)

add_executable(itmo_c_png_FedorrrS ${SOURCE_FILES})
target_link_libraries(itmo_c_png_FedorrrS m)

