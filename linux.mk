CC := clang
OBJ_DIR := obj/linux
BIN_DIR := bin/linux
LDFLAGS := -Wl,--copy-dt-needed-entries -lsfml-graphics -lsfml-window -lsfml-system

include ./Makefile