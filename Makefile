SHELL = bash

CXXSTANDARD := C++17

CPPFLAGS := -MD -MP -I./
CXXFLAGS := \
	-std=$(CXXSTANDARD) \
	-fanalyzer \
	-Wall \
	-Wextra

LDFLAGS :=
LDLIBS :=

NOTHING_LIB := build/nothing.a
NOTHING_SRC := $(shell find nothing -name \*.cpp)
NOTHING_OBJ := $(patsubst %.cpp,build/%.o,$(NOTHING_SRC))

$(mkdir -p $(sort $(dir $(NOTHING_LIB) $(NOTHING_OBJ))))

.PHONY: all
.all: $(NOTHING_LIB)

clean:
	rm -rf build bin

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/%.a:
	$(AR) rcs $@ $^

bin/%:
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@


$(NOTHING_LIB): $(NOTHING_OBJ)

-include $(shell find build -name \*.d 2>/dev/null)
