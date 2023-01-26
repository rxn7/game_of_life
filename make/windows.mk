CC := x86_64-w64-mingw32-g++
PLATFORM := windows

SFML_DIR := windows/SFML-2.5.1
CFLAGS := -DSFML_STATIC
INCFLAGS := -I$(SFML_DIR)/include
LDFLAGS := -L$(SFML_DIR)/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -static-libgcc -static-libstdc++ -lopengl32 -lwinmm -lgdi32 -lfreetype -pthread --static

include make/shared.mk

all: setup compile
