COMPILER = g++
INC_DIR = ./include
SRC_DIR = src
OBJ_DIR = object
CXXFLAGS = -std=c++0x -pthread #-Wall -std=c++11
CXXFLAGS += -I$(INC_DIR)
EXEC = main
OBJECTS =  $(OBJ_DIR)/main.o \
		   $(OBJ_DIR)/preprocess.o \
		   $(OBJ_DIR)/similarity.o \
		   $(OBJ_DIR)/match.o \
		   $(OBJ_DIR)/utility.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	        $(COMPILER) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	  $(COMPILER) -c $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)
