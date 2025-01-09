cmake_minimum_required(VERSION 3.0.5)

# Określ nazwę projektu
project(Klient)

set(CMAKE_CXX_STANDARD 17)

# Znajdź bibliotekę wxWidgets
find_package(wxWidgets 3.2.4 REQUIRED COMPONENTS net core base)
if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

include_directories(include)

# Ustaw wersję kompilatora C++ (jeśli używasz C++11 lub wyższej)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dodaj pliki źródłowe do projektu
set(SOURCES
    
    source/Klient.cpp
    source/logic/LoginFrame.cpp
    source/logic/ClientLogic.cpp
    source/logic/Packers.cpp
    
)


# Utwórz plik wykonywalny
add_executable(Klient ${SOURCES})

# Linkuj z biblioteką wxWidgets
target_link_libraries(Klient ${wxWidgets_LIBRARIES})

# Uwzględnij ścieżki do nagłówków wxWidgets
include_directories(${wxWidgets_INCLUDE_DIRS})