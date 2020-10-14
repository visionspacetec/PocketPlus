# CMake version
cmake_minimum_required( VERSION 3.0.0 )

# Project name
project( pocket_plus_cpp VERSION 0.0.1 DESCRIPTION "PocketPlus" )

# Compiler flags
set( CMAKE_CXX_STANDARD 17 )

# Includes
include( CTest )
enable_testing()
find_package( Threads REQUIRED )

# Include source folders
include_directories(
    ./src
)

# Declare source files
set(SOURCES 
    ./src/main.cpp 
)

# Declare target
add_executable( PocketPlus ${SOURCES} )

# Install
install( TARGETS PocketPlus DESTINATION bin )
set( CPACK_PROJECT_NAME ${PROJECT_NAME} )
set( CPACK_PROJECT_VERSION ${PROJECT_VERSION} )
include( CPack )
