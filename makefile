CPP.COMPILER ?= g++
BUILD ?= debug

DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(DIR)/src
TEST_DIR := $(DIR)/tests
INCLUDE_DIR := $(SRC_DIR)
BUILD_DIR := $(DIR)/build/$(BUILD)

EXECUTABLE := $(BUILD_DIR)/exec
TEST_EXECUTABLE := $(BUILD_DIR)/test_exec

CPP.FLAGS := -pedantic -Wall -Wextra -I$(INCLUDE_DIR) -std=c++20
ifeq ($(BUILD),release)
	CPP.FLAGS += -Ofast
else
	CPP.FLAGS += -g -O0 -lgtest -D DEBUG
endif

CPP.SRC := $(shell find $(SRC_DIR) -name '*.cpp')
CPP.TEST := $(shell find $(TEST_DIR) -name '*.cpp') $(filter-out $(SRC_DIR)/main.cpp, $(CPP.SRC))
CPP.HEADERS := $(shell find $(INCLUDE_DIR) -name '*.h')

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	$(EXECUTABLE) $(ARGS)

run-vg: $(EXECUTABLE)
	valgrind --leak-check=full $(EXECUTABLE)


test: $(TEST_EXECUTABLE)
	$(TEST_EXECUTABLE)

test-vg: $(TEST_EXECUTABLE)
	valgrind --leak-check=full $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(CPP.TEST) $(CPP.HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CPP.COMPILER) $(CPP.TEST) $(CPP.FLAGS) -o $(TEST_EXECUTABLE)

$(EXECUTABLE): $(CPP.SRC) $(CPP.HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CPP.COMPILER) $(CPP.SRC) $(CPP.FLAGS) -o $(EXECUTABLE)

clean:
	rm -r build/
