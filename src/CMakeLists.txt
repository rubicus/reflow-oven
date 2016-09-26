###########################################################################
# This is the cmake file, located in the source directory, that will show
# what images should be built, from what pieces of source and to what kind
# of hardware.
#
# See the documentation for the the arduino cmake project
# (here: https://github.com/rubicus/arduino-cmake/blob/master/README.rst)
# as well as the examples provided with that for details on the meaning
# project on the meaning of the commands given in this file.
###########################################################################

#This project runs on an arduino 328, so lets put that as our default
set(ARDUINO_DEFAULT_BOARD nano328)
set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0) #USB-serial port. typical arduino

#Here is the build recipe for our program
generate_arduino_firmware(reflow_oven
  SKETCH reflow-oven.ino
  SRCS filter.cpp temperature.cpp init.cpp max6675.cpp)
