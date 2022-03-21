SHELL = bash

CMAKE ?= cmake
MAKE ?= make
GIT ?= git

CXXSTANDARD = c++20

CPPFLAGS = -MMD -MP -I ./
CXXFLAGS = \
	-std=$(CXXSTANDARD) \
	-fanalyzer \
	-Wall \
	-Wextra \
	-Werror=suggest-override

LDFLAGS = -L ./build/lib
LDLIBS =

# NOTHING_LIB = build/lib/libnothing.a
NOTHING_SRC = $(shell find nothing -name \*.cpp)
NOTHING_OBJ = $(patsubst %.cpp,build/%.o,$(NOTHING_SRC))

NOTHING_TEST_SRC = $(shell find test -name \*.cpp)
NOTHING_TEST_OBJ = $(patsubst %.cpp,build/%.o,$(NOTHING_TEST_SRC))
NOTHING_TEST_BIN = build/bin/test
NOTHING_TEST_CPPFLAGS = \
	-isystem ./$(GTEST_INCLUDE) \
	-Wno-analyzer-possible-null-argument \
	-Wno-analyzer-malloc-leak

GTEST_LIB = $(patsubst %,build/lib/lib%.a,gtest gtest_main gmock)
GTEST_DIR = third_party/googletest
GTEST_INCLUDE = $(GTEST_DIR)/googletest/include
GTEST_SUBMODULE = $(GTEST_DIR)/.git

DIRS = $(sort $(dir \
	$(NOTHING_LIB) \
	$(NOTHING_OBJ) \
	$(NOTHING_TEST_BIN) \
	$(NOTHING_TEST_OBJ) \
	$(GTEST_LIB)))

$(shell mkdir -p $(DIRS))

.PHONY: all test clean

all: $(NOTHING_LIB) test

test: $(NOTHING_TEST_BIN)

run_tests: test
	$(NOTHING_TEST_BIN)

clean:
	$(RM) -r build
	if [[ -e $(GTEST_SUBMODULE) ]] ; then \
		$(GIT) submodule deinit $(GTEST_DIR) >/dev/null ; \
	fi

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/%.a:
	$(AR) $(ARFLAGS) $@ $^

build/bin/%:
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(GTEST_INCLUDE): $(GTEST_SUBMODULE)

$(GTEST_SUBMODULE):
	$(GIT) submodule update --init $(GTEST_DIR)

$(GTEST_LIB) &: $(GTEST_SUBMODULE)
	$(CMAKE) -B./build/gtest $(GTEST_DIR)
	$(MAKE) -C build/gtest/
	cp build/gtest/lib/* build/lib

$(NOTHING_LIB): $(NOTHING_OBJ)
$(NOTHING_TEST_OBJ): $(GTEST_INCLUDE)
$(NOTHING_TEST_OBJ): CPPFLAGS += $(NOTHING_TEST_CPPFLAGS)
$(NOTHING_TEST_BIN): $(NOTHING_TEST_OBJ) $(NOTHING_LIB) $(GTEST_LIB)

-include $(shell find build -name \*.d 2>/dev/null)

