# CMake version
cmake_minimum_required( VERSION 3.1.0 )

# Project name
project( pocket_plus_cpp VERSION 0.1.0 DESCRIPTION "PocketPlus" )

# Compiler flags
set( CMAKE_CXX_STANDARD 17 )

# Includes
enable_testing()
find_package( Threads REQUIRED )
find_package( GTest REQUIRED )
include_directories( ${GTEST_INCLUDE_DIRS} )

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
    ./src/compressor/compressor.h
    ./src/compressor/compressor.cpp 
    ./src/decompressor/decompressor.h
    ./src/decompressor/decompressor.cpp
    ./src/utils/utils.h
    ./src/utils/utils.cpp
)
set(TEST_SOURCES
    ./tests/utils_tests.cpp
    ./tests/compressor_tests.cpp 
    ./tests/decompressor_tests.cpp
)

# Declare target
add_executable( PocketPlus ${SOURCES} )
add_library( PocketPlus_lib STATIC ${SOURCES} )

# Declare tests
add_executable( PocketPlus_tests ${TEST_SOURCES} )
target_link_libraries( 
    PocketPlus_tests 
    gtest 
    gtest_main
    pthread 
    PocketPlus_lib 
)

# Discover test cases
gtest_discover_tests( PocketPlus_tests )

# Install
install( TARGETS PocketPlus DESTINATION bin )
set( CPACK_PROJECT_NAME ${PROJECT_NAME} )
set( CPACK_PROJECT_VERSION ${PROJECT_VERSION} )
include( CPack )
