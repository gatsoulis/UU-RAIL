# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge

#--------------------------------------
# Project
#
project(UU-RAIL)
cmake_minimum_required(VERSION 2.6)


#---------------------------------------
# Check for required libraries
#
# FIND_PACKAGE(PNG REQUIRED)


#--------------------------------------
# Compiler flags
#
#TODO: there should be both debug and release versions
add_definitions(-Wall -Wextra -pedantic -march=i686 -msse3 -fmessage-length=0 -O3 -g)


#--------------------------------------
# Documentation installation
#
set(DOC_FILES README)
set(DOC_PATH "share/doc/uurail")
install(FILES ${DOC_FILES}
        DESTINATION ${DOC_PATH})


#-------------------------------------
# Add the Src and Examples
add_subdirectory(Src)
add_subdirectory(Examples)