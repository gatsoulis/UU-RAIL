# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge
#
#	Examples compilation
#
#--------------------------------------
# All of the include directories so that 
# the examples just do #include "foo.h"
include_directories(
    .
    ..
	../Src/Math/Include
	../Src/Robot/Include
	../Src/ML/Include
	../Src/Vision/Include
	../Src/Util/Include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/bin/
   )


#-------------------------------------
# Define the sources for the examples
set(DumbTest_SRC test.cpp)


#------------------------------------
# Define executables
add_executable(DumbTest ${DumbTest_SRC})
target_link_libraries(DumbTest UU-RAIL-Math ${CMAKE_REQUIRED_LIBRARIES})
add_dependencies(DumbTest UU-RAIL-Math)


#------------------------------------
# Setting file specific flags
if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${DumbTest_SRC} PROPERTIES COMPILE_FLAGS "-Wall")
endif ()


#-----------------------------------
# Install directives
install(TARGETS DumbTest RUNTIME DESTINATION share/doc/uurail/examples)