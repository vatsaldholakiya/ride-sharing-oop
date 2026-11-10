CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2
SRCS     := main.cpp Ride.cpp Driver.cpp Rider.cpp RideSharingSystem.cpp
TARGET   := ridesharing

$(TARGET): $(SRCS) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
