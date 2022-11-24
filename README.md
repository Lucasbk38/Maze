# Maze

This is a repo of one of my c++ project, it is a Maze I created.


## Setup

To start it, you just run `main.zsh` (it only works when you have g++ installed and only on macOs and should on linux)
if you want it to work on windows, just run Cu.zsh, compile `main.cpp`, run it, and run `Ga.zsh`.

PS: For it to create the video, you need to have ffmpeg installed.

## Customization

You can access file `main.cpp` file and change its params on the top.

## Example

This is a [video](video.mp4) generated with the following settings :

```c++
const int RANDOM_SEED = 13042011;
const int COLOR_COEF = 4;
const int WIDTH = 160;
const int HEIGHT = 90;

const int generateLogRate = 10;
const int solveLogRate = 10;
const int hideLogRate = 10;
```
