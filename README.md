<a href="http://www.visionspace.com">
   <img src="https://www.visionspace.com/img/VISIONSPACE_HZ_BLACK_HR.png" alt="visionspace logo" title="visionspace_cicd" align="right" height="25px" />
</a>

# PocketPlus

## Project overview

### Introduction
POCKET+ is a lossless compression algorithm for fixed length time series data.

This repository contains a C++17 based implementation of a variety of the POCKET+ compression algorithm, described in the CCSDS 124.0-W-4 standard (version November 2020).
The implementation was done to validate the compression performance and properties of the algorithm.
Since the standard was not yet released, some changes might happen before the final release.

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

To install the library in /usr/local/lib run:
```bash
sudo make install
sudo ldconfig
```
If you forget to run ldconfig, the programm links but does not run.

## Usage

When using the installed PocketPlus library in another C++ project, include it with CMake:
```
find_library( PocketPlus REQUIRED )
```

In you code, import the compressor, decompressor and the utility functions as needed:
```c++
#include "pocketpluscompressor.h"
#include "pocketplusdecompressor.h"
#include "pocketplusutils.h"
```

A simple example can be found in the provided [main function](https://github.com/visionspacetec/PocketPlus/blob/master/src/main.cpp), which is only ment to showcase and test the algorithm.

## Find out more

This work was created following a standard defined by the **Consultative Committee for Space Data Systems** (CCSDS): https://public.ccsds.org

## Contributing

If you would like help implementing a new feature or fix a bug, check out our **[Contributing](https://github.com/visionspacetec/PocketPlus/blob/master/.github/contributing.md)** page and the **[Code of Conduct](https://github.com/visionspacetec/PocketPlus/blob/master/.github/code_of_conduct.md)**!

## Questions or need help?

Please open an **[issue](https://github.com/visionspacetec/PocketPlus/issues/new/choose)** for bug reporting, enhancement or feature requests.
