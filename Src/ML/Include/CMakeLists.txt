# CMakeLists for the UU-RAIL project
# Initial version 19-05-10
# Chris Burbridge
#
#	Generic includes makefile
#	to ensure includes are included :-)
#
#--------------------------------------

install(DIRECTORY .
        DESTINATION include/UU-Rail
        FILES_MATCHING PATTERN "*.h"
                       PATTERN ".svn" EXCLUDE
					   PATTERN ".git" EXCLUDE
					   PATTERN "*~" EXCLUDE
	    
       )