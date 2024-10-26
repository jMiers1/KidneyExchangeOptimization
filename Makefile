# Makefile for simple C++ project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# Target executable
TARGET = program

# Source files
SRCS = main.cpp functions.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target to build the executable
$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)  

# Rule to build object files
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@ 

# Custom run target to build, execute, and clean up
run: $(TARGET)
	@echo "Surpressed message" 
	@./$(TARGET)
	@make clean > /dev/null 2>&1

# Clean up build files
clean:
	@rm -f $(OBJS) $(TARGET)

# Phony targets (not files)
.PHONY: clean run
