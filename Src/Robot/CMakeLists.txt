# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge
#
#	Robot Library
#
#--------------------------------------
#  Define the sources
#
set(ROBOR_LIB_SRC  kmeans.cpp Math.cpp)
add_library(UU-RAIL-Robot SHARED ${ROBOT_LIB_SRC})

# The library  requires the libraries discovered in the master
# CMake file.
target_link_libraries(UU-RAIL-Robot ${CMAKE_REQUIRED_LIBRARIES})

#--------------------------------------
#   Includes
#
include_directories(. .. Include)


set(CMAKE_REQUIRED_LIBRARIES "UU-RAIL-Robot;${CMAKE_REQUIRED_LIBRARIES}")


#--------------------------------------
#   Install directions
# 
install(TARGETS UU-RAIL-Robot LIBRARY DESTINATION lib)


#-------------------------------------
# Grab the include directory into the build system too
add_subdirectory(Include)