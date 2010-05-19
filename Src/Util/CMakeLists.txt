# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge
#
#	Util Library
#
#--------------------------------------
#  Define the sources
#
set(UTIL_LIB_SRC  MatrixFile.cpp FileNamer.cpp)
add_library(UU-RAIL-Util SHARED ${UTIL_LIB_SRC})

# The library  requires the libraries discovered in the master
# CMake file.
target_link_libraries(UU-RAIL-Util ${CMAKE_REQUIRED_LIBRARIES})

#--------------------------------------
#   Includes
#
include_directories(. .. Include)


set(CMAKE_REQUIRED_LIBRARIES "UU-RAIL-Util;${CMAKE_REQUIRED_LIBRARIES}")


#--------------------------------------
#   Install directions
# 
install(TARGETS UU-RAIL-Util LIBRARY DESTINATION lib)


#-------------------------------------
# Grab the include directory into the build system too
add_subdirectory(Include)