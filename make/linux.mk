CC := g++
PLATFORM := linux
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

include make/shared.mk

all: setup compile
