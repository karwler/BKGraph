# BKGraph
Work in progress ¯\_(ツ)_/¯  

## Installation
Used libraries are SDL2 and SDL2_ttf.  
The minimum required Cmake version is (probably) 3.1.3 and your compiler needs to support at least C++11 (also probably).  
To create project files open up a terminal, \CMD or whatever it is you're using, cd into the "BKGraph" directory and execute the following commands:  
```shell
mkdir build  
cd build  
cmake ..  
```
All necessary project files should now be in the "build" directory.  

### Linux
All libraries need to be installed manually.  
Installing the development packages for the above mentioned libraries should do the trick.  
The default font is set to "Arial", so you probably need to install some kind of "ttf-ms-fonts" package.  
If you don't want to install any new fonts, you can just change the default font in the "src/utils/defaults.h" file or put an "arial.ttf" file in the executable's directory.  

### Windows
CMakeLists.txt and included libraries are set up for MS Visual Studio.  
If you're using a different environment, you'll need to link your version of the libraries yourself.  
