CXX=clang++

CXXFLAGS=--std=c++11 -g

all: late_parser


late_parser: late_parser.o late_core.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o late_parser

# g++ --std=c++11 -MM *.cpp
late_core.o: late_core.cpp late_core.hpp
late_parser.o: late_parser.cpp late_parser.hpp late_core.hpp
