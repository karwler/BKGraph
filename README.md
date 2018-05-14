# BKGraph
A simple desktop application for displaying functions as graphs.  
It runs on Linux and Windows 7 or later.  
Used libraries are SDL2, SDL2_image, SDL2_ttf and any libraries that come with them.  

## Installation

### Requirements:
- CMake (unless your IDE can interpret CMakeLists.txt)  
- C++ compiler (preferably Clang, GCC or MSVC)  
- development libraries of SDL2, SDL2_image and SDL2_ttf (already included for MSVC)  

The minimum required CMake version is 3.1.3 and the compiler needs to support the C++11 standard.  

### Creating Project Files
Open a terminal, cd into the project directory and execute the following commands:  
```
mkdir build
cd build
cmake ..
```
All necessary project files should now be in the "build" directory.  

### Linux
All above mentioned libraries need to be installed manually.  
The default font is set to "Arial" so you'll probably need to install some kind of "ttf-ms-fonts" package.  
If you don't want to install any new fonts you can put an "arial.ttf" file in the executable's directory or change the default font in "src/utils/defaults.h" or run the program once and change the font in "settings.ini".  
You can use the "run.sh" script if the compiler creates a shared object rather than an executable and you can't start the program normally.  
There's also a launcher (.desktop file) in the "rsc" directory. You just have to set "Exec" and "Icon" to the executable's and icon't path.  

### Windows
CMakeLists.txt and included libraries are set up for MS Visual Studio.  
If you're using a different environment, you'll need to link your version of the libraries yourself.  

## Usage

### Function View
Right click to open a context menu for editing the fucntion list.  
Left checkbox controls whether function will be drawn.  
The box between the checkbox and input field sets the graph's color.  

Supported operations, constants and functions are:  
- +, -, *, /, ^, ! (addition, subtraction/negation, multiplication, division, power, factorial)  
- pi (π)  
- e (Euler's number)  
- abs (absolute value)  
- sqrt, cbrt (square/cube root)  
- exp (Euler's number raised to the given exponent)  
- ln, log (natural/common logarithm)  
- sin, cos, tan (sine, cosine, tangent)  
- asin, acos, atan (inverse sine/cosine/tangent)  
- sinh, cosh, tanh (hyperbolic sine/cosine/tangent)  
- asinh, acosh, atanh (inverse hyperbolic sine/cosine/tangent)  
- round, floor, ceil (round nearest/downwards/upwards)  
- trunc (cut off decimal part)  

Functions must have parentheses.  
Whitespaces are ignored.  

### Variable View
Right click to open a context menu for editing the fucntion list.  
Left input field sets the variable name and the right one sets it's value.  
Variable names can consist of upper-/lowercase letters from "A" to "Z" or "_".  

### Graph View
To move the viewport either hold down the left mouse button and move the mouse or use the arrow keys.  
To zoom either hold down Alt and the left mouse button and mov the mouse left or right or use the Page Up/Down keys.  
To center the viewport press C.  
To reset the viewoprt size press X.  
Right clicking on a graph let's you input an x value to get the function's corresponding y value.  
