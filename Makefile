
CC := gcc
CXX := g++

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude

LDFLAGS :=
LDLIBS := -lm

BUILD_DIR := build

C_SOURCES := src/main.c src/battery.c src/battery_manager.c \
             src/validation.c src/analytics.c src/prediction.c \
             src/storage.c src/report.c

APP_OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

TEST_SOURCES := tests/test_battery.cpp \
                tests/test_validation.cpp \
                tests/test_analytics.cpp \
                tests/test_prediction.cpp

TEST_OBJECTS := $(patsubst tests/%.cpp,$(BUILD_DIR)/%.test.o,$(TEST_SOURCES))

LIB_OBJECTS := $(BUILD_DIR)/battery.o \
               $(BUILD_DIR)/battery_manager.o \
               $(BUILD_DIR)/validation.o \
               $(BUILD_DIR)/analytics.o \
               $(BUILD_DIR)/prediction.o \
               $(BUILD_DIR)/storage.o \
               $(BUILD_DIR)/report.o

.PHONY: all run test clean

all: $(BUILD_DIR)/ev_battery

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/ev_battery: $(APP_OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

run: all
	./$(BUILD_DIR)/ev_battery

$(BUILD_DIR)/%.test.o: tests/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/test_runner: $(TEST_OBJECTS) $(LIB_OBJECTS)
	$(CXX) $(LDFLAGS) $^ -lgtest -lgtest_main -pthread $(LDLIBS) -o $@

test: $(BUILD_DIR)/test_runner
	./$(BUILD_DIR)/test_runner

clean:
	rm -rf $(BUILD_DIR) data/latest_report.txt

-include $(APP_OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d)
