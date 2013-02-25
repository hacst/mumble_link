CXX = g++

CXXFLAGS = -g -fPIC -shared

MODULES = mumble_link.so

all: $(MODULES)

mumble_link.so: mumble_link.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ -I/usr/include/python2.7/ -lboost_python -lrt

clean:
	rm -f $(MODULES)

.PHONY: all
.PHONY: clean
