# CMake version
cmake_minimum_required( VERSION 3.1.0 )

# Project name
project( pocket_plus VERSION 0.1.0 DESCRIPTION "PocketPlus" )

# Compiler flags
set( CMAKE_CXX_STANDARD 17 )

# Includes
enable_testing()
set(THREADS_PREFER_PTHREAD_FLAG ON)
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
    #./src/compressor/compressor.h
    ./src/compressor/pocketpluscompressor.cpp
    #./src/decompressor/decompressor.h
    ./src/decompressor/pocketplusdecompressor.cpp
    #./src/utils/utils.h
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
add_executable( PocketPlus.exe ${SOURCES} )#PocketPlus )

# Declare test target
add_executable( PocketPlus_tests ${TEST_SOURCES} )# PocketPlus )
target_link_libraries(
    PocketPlus_tests
    gtest
    gtest_main
    pthread
    PocketPlus
)

# Discover test cases
gtest_discover_tests( PocketPlus_tests )

# Install
install( TARGETS PocketPlus LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include )
         #ARCHIVE DESTINATION lib
#RUNTIME DESTINATION bin)
