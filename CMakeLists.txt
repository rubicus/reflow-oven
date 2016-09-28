#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#

# set the arduino toolchain file:
set(CMAKE_TOOLCHAIN_FILE arduino-cmake/cmake/ArduinoToolchain.cmake)

#Make sure we work with C11 and C++11
set(ARDUINO_C_FLAGS "-std=c11 -mcall-prologues -ffunction-sections -fdata-sections")
set(ARDUINO_CXX_FLAGS "-std=gnu++11 -mcall-prologues -ffunction-sections -fdata-sections -fno-exceptions")

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoExample C CXX)

print_board_list()
print_programmer_list()

add_subdirectory(src)   #add the example directory into build
