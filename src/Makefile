CXX = clang++
LD = ld
CXXFLAGS = -Wall -O3 -s -std=gnu++0x -D__STRICT_ANSI__
INSTALL_PROGRAM = install
BINDIR = /usr/bin

all: valency

clean:
		rm -f ../bin/valency

valency: builtin_arithmetic.cpp builtin.cpp chrono.cpp functions.hpp generic.cpp interactive.cpp main.cpp types.hpp
		$(CXX) $(CXXFLAGS) main.cpp -o ../bin/valency

install: ../bin/valency
		$(INSTALL_PROGRAM) ../bin/valency $(BINDIR)/valency
