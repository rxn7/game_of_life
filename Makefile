CC := g++
OBJ_DIR := obj
BIN_DIR := bin
DIR_SRC := src
OUT := $(BIN_DIR)/game_of_life
INC := -Isrc
CFLAGS := -std=c++20 -O3
SRC := $(wildcard $(addsuffix /*.cpp, $(DIR_SRC)))
OBJ := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
LIBS := -lsfml-graphics -lsfml-window -lsfml-system

all: create_dirs copy_resources $(OBJ) $(OUT)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OUT): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(LIBS) $(OBJ) -o $@

create_dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

copy_resources:
	@rm -rf bin/res
	@cp -r res bin/

clean:
	rm $(OBJ) $(OUT)