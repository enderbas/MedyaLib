[![CMake](https://github.com/enderbas/MedyaLib/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/enderbas/MedyaLib/actions/workflows/cmake.yml)
# MedyaLib
Tag based media index program. Save images with location time and person information. Add tags to images. Search combinational. This is MedyaLib.
[![medyalib1.gif](https://s8.gifyu.com/images/medyalib1.gif)](https://gifyu.com/image/Ssiqx)

# Features
- Save medias with catchy words
- Search with saved badges
- Show medias in customizable sized screen
# Requirements
- CMake 3.16 or higher
- Qt5

## Cmake
```bash
    sudo apt install cmake
```
## Qt 
```bash
    sudo apt install build-essential
    sudo apt install qt5-default
```
# Building the project
A similar CMake routine to the one below:

```cmake
mkdir build/ && cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=/absolute/path/to/custom/install/directory
cmake --build .
```