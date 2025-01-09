cmake_minimum_required(VERSION 3.10)
project(Serwer)

set(CMAKE_CXX_STANDARD 17)

# Dodaj katalog z nagłówkami
include_directories(include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


find_package(SQLite3 REQUIRED)

# Wszystkie pliki źródłowe
set(SOURCES
    source/main.cpp
    source/logic/Serwer.cpp
)

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resource")

file(COPY ${RESOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR}/resources)

# Tworzenie pliku wykonywalnego
add_executable(Serwer ${SOURCES})

target_link_libraries(Serwer SQLite::SQLite3)
