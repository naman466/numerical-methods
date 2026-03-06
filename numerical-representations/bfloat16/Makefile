CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS =

TEST_TARGET = test_bfloat16
EXAMPLE_TARGET = example_bfloat16

BFLOAT_SOURCES = bf16_basic.cpp \
                 bf16_arithmetic.cpp \
                 bf16_comparison.cpp \
                 bf16_io.cpp

TEST_SOURCES = $(BFLOAT_SOURCES) bf16_test.cpp
EXAMPLE_SOURCES = $(BFLOAT_SOURCES) example.cpp

TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
EXAMPLE_OBJECTS = $(EXAMPLE_SOURCES:.cpp=.o)

HEADERS = BF16.h

all: $(TEST_TARGET) $(EXAMPLE_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(EXAMPLE_TARGET): $(EXAMPLE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

example: $(EXAMPLE_TARGET)
	./$(EXAMPLE_TARGET)

run: test example

clean:
	rm -f $(TEST_OBJECTS) $(EXAMPLE_OBJECTS) $(TEST_TARGET) $(EXAMPLE_TARGET)

rebuild: clean all

debug: CXXFLAGS += -g -DDEBUG
debug: rebuild

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build test and example programs (default)"
	@echo "  test     - Build and run test program"
	@echo "  example  - Build and run example program"
	@echo "  run      - Build and run both programs"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and rebuild"
	@echo "  debug    - Build with debug symbols"
	@echo "  help     - Show this help message"

.PHONY: all test example run clean rebuild debug help