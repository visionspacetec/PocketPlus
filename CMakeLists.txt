# CMake version
cmake_minimum_required( VERSION 3.1.0 )

# Project name
project( pocket_plus VERSION 0.2.0 DESCRIPTION "PocketPlus" )

# Compiler flags
set( CMAKE_CXX_STANDARD 17 )

# Includes
enable_testing()
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package( Threads REQUIRED )
find_package( GTest REQUIRED )
include_directories( ${GTEST_INCLUDE_DIRS} )

# Code coverage
include(${CMAKE_CURRENT_SOURCE_DIR}/CodeCoverage.cmake)
set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

# Include source folders
include_directories(
    ./src
    ./src/compressor
    ./src/decompressor
    ./src/utils
    ./tests
)

# Declare source files
set(SOURCES
    ./src/main.cpp
    ./src/compressor/pocketpluscompressor.cpp
    ./src/decompressor/pocketplusdecompressor.cpp
    ./src/utils/pocketplusutils.cpp
)
set(TEST_SOURCES
    ./tests/utils_tests.cpp
    ./tests/compressor_tests.cpp
    ./tests/decompressor_tests.cpp
)

# Declare library target
add_library( PocketPlus SHARED ${SOURCES} )
set_target_properties( PocketPlus PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( PocketPlus PROPERTIES PUBLIC_HEADER 
    "src/compressor/pocketpluscompressor.h;src/decompressor/pocketplusdecompressor.h;src/utils/pocketplusutils.h"
)

# Declare binary target
add_executable( PocketPlus.exe ${SOURCES} )

# Declare test target
add_executable( PocketPlus_tests ${TEST_SOURCES} )
target_link_libraries(
    PocketPlus_tests
    gtest
    gtest_main
    pthread
    PocketPlus
)

SETUP_TARGET_FOR_COVERAGE(
	PocketPlus_tests_coverage # Name for custom target.
	PocketPlus_tests          # Name of the test driver executable that runs the tests.
	coverage                  # Name of output directory.
)

# Discover test cases
gtest_discover_tests( PocketPlus_tests )

# Install
install( TARGETS PocketPlus LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include )
