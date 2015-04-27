CXX=clang++

CXXFLAGS= --std=c++11 -O2

LDFLAGS= -lboost_regex

all: late_parser


late_parser: late_parser.o late_core.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o late_parser

# g++ --std=c++11 -MM *.cpp
late_core.o: late_core.cpp late_core.hpp
late_parser.o: late_parser.cpp late_parser.hpp late_core.hpp
