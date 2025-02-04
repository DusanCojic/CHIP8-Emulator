CXX = g++
FLAGS = -Wall -Wextra

SRC = main.cpp chip8.cpp

TARGET = chip8

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(FLAGS) -o $(TARGET) $(SRC)

clean:
	del $(TARGET).exe