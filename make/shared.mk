.PHONY = setup create_dirs copy_res clean compile

SRC_DIR := src
OBJ_DIR := obj/$(PLATFORM)
BIN_DIR := bin/$(PLATFORM)
OUT := $(BIN_DIR)/game_of_life
INCFLAGS += -Isrc -Iinclude
CFLAGS += -std=c++20
SRC += $(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp */*/*/*/*/*.cpp)
OBJ += $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

setup: create_dirs copy_res
compile: $(OBJ) $(OUT)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

create_dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

copy_res:
	@rm -rf $(BIN_DIR)/res
	@cp -r res $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
