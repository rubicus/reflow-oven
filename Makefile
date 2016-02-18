BOARD_TAG    = nano328
ARDUINO_LIBS = LiquidCrystal # MAX6675_library (now modded in project)
#ARCHITECTURE = avr
OPTIMIZATION_LEVEL = s
CFLAGS = -std=c11
CPPFLAGS = -std=gnu++11

include Arduino-Makefile/Arduino.mk

check-syntax:
		$(CXX) -c -include Arduino.h -x c++ $(CXXFLAGS) $(CPPFLAGS) -fsyntax-only $(CHK_SOURCES)
