CPP := g++
CPPFLAGS := -std=c++17 -Wall -Wextra -Weffc++ -pedantic

TARGETS := $(wildcard *.cpp)
HEADERS := $(wildcard *.hpp)
OUTPUT := familytree

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(TARGETS) $(HEADERS)
	$(CPP) $(CPPFLAGS) -o $@ $(TARGETS)

clean:
	rm $(OUTPUT)