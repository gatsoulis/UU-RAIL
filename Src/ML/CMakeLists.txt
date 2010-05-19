# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge
#
#	ML Library
#
#--------------------------------------
#  Define the sources
#
set(ML_LIB_SRC  node.cpp som.cpp)
add_library(UU-RAIL-ML SHARED ${ML_LIB_SRC})

# The library  requires the libraries discovered in the master
# CMake file.
target_link_libraries(UU-RAIL-ML ${CMAKE_REQUIRED_LIBRARIES})

#--------------------------------------
#   Includes
#
include_directories(. .. Include)


set(CMAKE_REQUIRED_LIBRARIES "UU-RAIL-ML;${CMAKE_REQUIRED_LIBRARIES}")


#--------------------------------------
#   Install directions
# 
install(TARGETS UU-RAIL-ML LIBRARY DESTINATION lib)


#-------------------------------------
# Grab the include directory into the build system too
add_subdirectory(Include)