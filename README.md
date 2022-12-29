# CyberBiology
Artificial evolution

This is a little entertainment project, evolution simulator. The code itself is far from best. Lazily written, a lot of code in headers, unconditional jumps at couple of places etc. I'll make it better in later versions!


## Screenshots

![Screenshot1](/Screenshots/1.png?raw=true "Screenshot1")
![Screenshot2](/Screenshots/3.png?raw=true "Screenshot2")


## What you need to build it

+ SDL
+ [Dear Im GUI](https://github.com/Tyyppi77/imgui_sdl)
+ [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
+ [ImPlot](https://github.com/epezent/implot)
+ Visual studio 2022 and all the tools needed to build C++ apps


## How to run this code

1. Install packages
    - `cmake` to configure and build project
    - С/С++ - compiler, for example `g++` or `clang++` / `Visual studio 2022` for Windows.

2. Clone submodules

```console
$ git submodule update --init --recursive
```

3. Make dirrectrory build and enter into it

```console
$ mkdir build && cd build
```

4. Configure project
    - [Windows]: `$ cmake .. -G"Visual Studio 17 2022" -A x64`
    - [Linux]: `$ cmake ..`

5. Build

```console
$ cmake --build .
```

## Interface

### Main window
Contains all info about simulation

### Controls
+ Start/Stop button (Pause/unpause)
+ PS reward - how much energy a bot gets with one tick of photosynthesis
+ skip - how many turns you calculate before you draw new frame (set to 0 if you want to render every frame)
+ brush - brush size for mouse actions sich as remove (eraser tool)

### Selection
Info about selected bot, some useful buttons to mess with it

### Display
Rendering method. 
+ Bot natural color (just its color)
+ Bot rations (carnivores red, herbivores green, grey if never ate, blue if eats apples or organics)
+ Energy (yellow = low energy, red = maximum energy)
+ No render means you only compute without rendering, starts to simulate very fast

### Mouse function
Choose what to do with your mouse clicks

### Aditional
Some more windows


## Keyboard

Using keyboard is more handy because interface becomes laggy during simulation

+ "Space" - Start/Pause
+ "Plus sign near numpad" - make 1 step
+ "F1" - Spawn a group of random bots
+ "F2" - Create vertical wall
+ "Numpad 1-4" - Select rendering method
+ "Home" - Set camera at X = 0
+ Arrow keys (left, right) - Move camera
+ "Page Up", "Page Down" - Move camera fast
+ "End" - Find first bot on screen


## How to use Settings file
First of all you need to setup your screen resolution, otherwise you wouldn't see the interface.
After that you change your rendering area width ("FieldRenderCellsWidth") and
world height ("FieldCellsHeight") to match your screen. World can be wider than your render area,
but you have to keep its width divisible by 16 without a remainder, it's needed for multi threading.

All changable parameters are listed in that file.
