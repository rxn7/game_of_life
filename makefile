SRC = -c src/*.cc
INCLUDE = -I src -I include
OBJS = Main.o Game.o
LIBS = -L lib -lsfml-graphics -lsfml-window -lsfml-system

all: compile link

compile: 
	g++ -std=c++17 -pthread -Os  $(INCLUDE) $(SRC)

link:
	g++ $(OBJS) -o bin/ConwaysGameOfLife.exe $(LIBS)