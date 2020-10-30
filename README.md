<a href="http://www.visionspace.com">
   <img src="https://www.visionspace.com/img/VISIONSPACE_HZ_BLACK_HR.png" alt="visionspace logo" title="visionspace_cicd" align="right" height="25px" />
</a>

# PocketPlus

## Project overview

### Introduction
This repository contains a c++ implementation of the Pocket+ compression algorithm described in the CCSDS 124.0-W-4 standard.

## Installation 

### Run tests
This project uses Google Test (GTest) with CMake for testing.
To install on Ubuntu run:
```bash
sudo apt-get install libgtest-dev
sudo apt-get cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp*.a /usr/lib
```

## Usage

