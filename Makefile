# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -ljpeg

# Source files and target executable
SOURCES = app.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = app

# Default target
all: $(EXECUTABLE)

# Rule for generating the executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

# Rules for generating object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

