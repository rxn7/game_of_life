CC := x86_64-w64-mingw32-g++
OBJ_DIR := obj/windows
BIN_DIR := bin/windows
INC := -Iwindows/include
LIBS := -Lwindows/lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32 -lfreetype

include ./Makefile