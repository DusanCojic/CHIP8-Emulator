CXX = g++
FLAGS = -Wall -Wextra

SDL_PATH = ./libs/SDL3
SDL_INCLUDE = -I$(SDL_PATH)/include
SDL_LIB = -L$(SDL_PATH)/lib
SDL_FLAGS = -lSDL3

SRC = main.cpp chip8.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = chip8

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(FLAGS) $(SDL_INCLUDE) $(SDL_LIB) -o $(TARGET) $(OBJ) $(SDL_FLAGS)

%.o: %.cpp
	$(CXX) $(FLAGS) $(SDL_INCLUDE) -c $< -o $@

clean:
	del $(TARGET).exe *.o
