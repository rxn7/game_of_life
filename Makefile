SRC_DIR += src
OUT += $(BIN_DIR)/game_of_life
INC += -Isrc -Iinclude
CFLAGS += -std=c++20 -O3
SRC += $(wildcard $(addsuffix /*.cpp, $(SRC_DIR)))
OBJ += $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: create_dirs copy_resources $(OBJ) $(OUT)

create_dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

copy_resources:
	@rm -rf $(BIN_DIR)/res
	@cp -r res $(BIN_DIR)/

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OUT): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(OBJ) -o $@