# Game of Life

Conway's game of life written in C++ using SFML library.  
[Trello board](https://trello.com/b/4bR9hQpB/game-of-life)

# Screenshots

![screenshot1](.github/screenshot1.png)
![screenshot2](.github/screenshot2.png)

# Features

- Zooming (scroll with your mouse)
- Moving around (move the mouse while pressing the mouse scroll)
- Pausing (hit escape or click the GUI pause button)
- Placing cells (hold/click left mouse button)
- Removing cells (hold/click right mouse button)

# Optimizations

Used optimizations:

- Cell culling (render only visible cells) (chunk culling should replace this in the future)
- Vertex array (sending data to the GPU in single draw call)
- Logic is calculated on separate thread
- VertexArray building is on separate thread
- VertexArray is rebuilt only when necessary

# How to build

Clone the project with submodules by running `git clone https://github.com/rxn7/game_of_life --recurse_submodules`<br>
Configure the project by running `./configure.sh`<br>
Build and run the debug version by running `./debug.sh`<br>
Build and run the release version by running `./release.sh`<br>

# To do

- Chunk system
- Infinite world
- Game settings (GUI)
