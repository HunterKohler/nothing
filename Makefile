SHELL = bash

CXXFLAGS := \
	-g  \
	-O0 \
	-std=c++20 \
	-fanalyzer \
	-Wall \
	-Wextra

CPPFLAGS := -MD -MP -I./

LDFLAGS :=
LDLIBS :=

LIB_ARCHIVE := build/nothing.a

LIB := $(shell find nothing -name \*.c)
TEST := $(shell find test -name \*.c)
MISC := $(shell find misc -name \*.c)

LIB_OBJ := $(patsubst %.c,build/%.o,$(LIB))
MISC_OBJ := $(patsubst %.c,build/%.o,$(MISC))
TEST_OBJ := $(patsubst %.c,build/%.o,$(TEST))
OBJ := $(sort $(LIB_OBJ) $(MISC_OBJ) $(TEST_OBJ))

MISC_BIN := $(patsubst %.c,bin/%,$(MISC))
TEST_BIN := $(patsubst %.c,bin/%,$(TEST))
BIN := $(sort $(MISC_BIN) $(TEST_BIN))

.PHONY: all clean misc test run_tests

all: $(MISC_BIN) $(TEST_BIN) $(LIB_ARCHIVE)

clean:
	@rm -rf bin build

lib: $(LIB_ARCHIVE)
misc: $(MISC_BIN)
test: $(TEST_BIN)

run_tests: $(TEST_BIN)
	@printf "Running all tests:\n"
	@for i in $(TEST_BIN); do \
		printf "%s:\n" "$$i"; \
		$$i; \
	done

$(BIN): bin/% : build/%.o $(LIB_ARCHIVE)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJ): build/%.o : %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIB_ARCHIVE): $(LIB_OBJ)
	ar -rcs $@ $^

-include $(shell find build -name *.d 2>/dev/null)