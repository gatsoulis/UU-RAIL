# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge
#
#	Vision Library
#
#--------------------------------------
#  Define the sources
#
set(VISION_LIB_SRC  kmeans.cpp Math.cpp)
add_library(UU-RAIL-Vision SHARED ${VISION_LIB_SRC})

# The library  requires the libraries discovered in the master
# CMake file.
target_link_libraries(UU-RAIL-Vision ${CMAKE_REQUIRED_LIBRARIES})

#--------------------------------------
#   Includes
#
include_directories(. .. Include)


set(CMAKE_REQUIRED_LIBRARIES "UU-RAIL-Vision;${CMAKE_REQUIRED_LIBRARIES}")


#--------------------------------------
#   Install directions
# 
install(TARGETS UU-RAIL-Vision LIBRARY DESTINATION lib)


#-------------------------------------
# Grab the include directory into the build system too
add_subdirectory(Include)