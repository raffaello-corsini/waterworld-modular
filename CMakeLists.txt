# Preferable to avoid warnings during execution
cmake_minimum_required (VERSION 3.1)

#if(UNIX)
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
#endif()

set (CMAKE_CXX_STANDARD 11)

# Set the executable along with the required source files
add_executable(project project.cc)

# Instruct to link against the ariadne library, and the bdd library
target_link_libraries(project ariadne bdd)
