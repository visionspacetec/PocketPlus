<a href="http://www.visionspace.com">
   <img src="https://www.visionspace.com/img/VISIONSPACE_HZ_BLACK_HR.png" alt="visionspace logo" title="visionspace_cicd" align="right" height="25px" />
</a>

# PocketPlus

## Project overview

### Introduction
This repository contains a c++ implementation of the Pocket+ compression algorithm described in the CCSDS 124.0-W-4 standard.

## Installation
This project uses Google Test (GTest) with CMake for testing and building.
```bash
sudo apt-get install cmake git libgtest-dev
cd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib
cd ~
git clone https://github.com/visionspacetec/PocketPlus.git
cd PocketPlus
mkdir build
cd build
cmake ..
make
make test
```

To install the library:
```bash
sudo make install
```

## Usage

When using the installed PocketPlus library in another c++ project, include it with CMake:
```
find_library( PocketPlus REQUIRED )
```