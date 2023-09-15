# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -lpthread

# Targets
all: sender receiver

sender: sender.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

receiver: receiver.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f sender receiver
