cmake_minimum_required(VERSION 3.10)

# Project name
project(AZ60A_ZC_Project C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/EEPROM.C
    src/INT_CTRL.C
    src/IO_DRV.C
    src/MAIN.C
    src/SPI.C
    src/UART_PIN.C
)

# Create executable
add_executable(az60a_zc ${SOURCES})

# For simulation purposes, define hardware registers as variables
target_compile_definitions(az60a_zc PRIVATE SIMULATION_MODE)

# Optional: Add compiler warnings
if(MSVC)
    target_compile_options(az60a_zc PRIVATE /W4)
else()
    target_compile_options(az60a_zc PRIVATE -Wall -Wextra)
endif()