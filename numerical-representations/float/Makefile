CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS =

TARGET = test_fp32

SOURCES = fp32_basic.cpp \
          fp32_arithmetic.cpp \
          fp32_comparison.cpp \
          fp32_io.cpp \
          example.cpp
          # test_fp32.cpp

OBJECTS = $(SOURCES:.cpp=.o)
# EXTRA_OBJECTS = test_fp32.o 

HEADERS = FP32.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

rebuild: clean all

debug: CXXFLAGS += -g -DDEBUG
debug: rebuild

help:
	@echo "Available targets: all, run, clean, rebuild, debug, help"

.PHONY: all run clean rebuild debug help