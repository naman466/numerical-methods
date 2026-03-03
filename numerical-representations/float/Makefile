CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS =

TARGET = fp32_test

SOURCES = fp32_basic.cpp \
          fp32_arithmetic.cpp \
          fp32_comparison.cpp \
          fp32_io.cpp \
          fp32_test.cpp

OBJECTS = $(SOURCES:.cpp=.o)

HEADERS = FP32.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

debug: CXXFLAGS += -g -DDEBUG
debug: rebuild

help:
	@echo "Available targets:"
	@echo "  all     - Build the test program (default)"
	@echo "  run     - Build and run the test program"
	@echo "  clean   - Remove build artifacts"
	@echo "  rebuild - Clean and rebuild"
	@echo "  debug   - Build with debug symbols"
	@echo "  help    - Show this help message"

.PHONY: all run clean rebuild debug help