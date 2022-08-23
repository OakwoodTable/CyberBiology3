# CyberBiology3
Artificial evolution

This is a little entertainment project, evolution simulator. The code itself is far from best. Lazily written, a lot of code in headers, unconditional jumps at couple of places etc. I'll make it better in later versions!


## Screenshots

![Screenshot1](/Screenshots/1.png?raw=true "Screenshot1")
![Screenshot2](/Screenshots/3.png?raw=true "Screenshot2")


## What you need to build it

+ SDL

+ Dear Im GUI


## Interface

### Main window
Contains all info about simulation

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
