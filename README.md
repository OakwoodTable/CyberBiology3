# CyberBiology3
Artificial evolution

This is a little entertainment project, evolution simulator. The code itself is far from best. Lazily written, a lot of code in headers, unconditional jumps at couple of places etc. I'll make it better in later versions!


## Screenshots

![Screenshot1](/Screenshots/1.png?raw=true "Screenshot1")
![Screenshot2](/Screenshots/3.png?raw=true "Screenshot2")

## Video of work (Ru)
https://www.youtube.com/watch?v=_tdGKfAyRhw


## What you need to build it

+ SDL
https://www.libsdl.org/

+ Dear Im GUI
https://github.com/ocornut/imgui

+ sdl + ImGUI Implementation from here
https://github.com/Tyyppi77/imgui_sdl

+ SDL_ttf
https://github.com/libsdl-org/SDL_ttf

+ Visual studio 2022 and all the tools needed to build C++ apps


## How to run this code

+ Set include directories for all libraries listed above. And paths for SDL dll-s.  
+ Place imgui_sdl.h, imgui_sdl.cpp and SDL2_ttf.dll at project root, or change #include paths in headers
+ Enjoy!

## Interface

### Main window
Contains all info about simulation

### System
Info about your PC

### Controls
+ Start/Stop button (Pause/unpause)
+ food - ignore it
+ skip - how many turns you calculate before you draw new frame (set to 0 if you want render on every frame)
+ brush - brush size for mouse actions sich as remove

### Selection
Info about selected bot, some useful buttons to mess with it

### Display
Rendering method. 
+ Bot natural color (just its color)
+ Bot rations (carnivores red, herbivores green, grey if never ate)
+ Energy (yellow = low energy, red = maximum energy)
+ No render means you only compute without rendering, starts to simulate very fast

### Console
Whatever

### Mouse function
Choose what to do with your mouse clicks

### Aditional
Some more windows


## Keyboard

Using keyboard is more handy because interface becomes laggy during simulation

+ "Space" - Start/Pause
+ "Plus sign near numpad" - make 1 step
+ "Backspace" - Spawn group of randoms
+ "Numpad 1-4" - Select render method
