# THIS CMAKEFILE IS NO LONGER USED BECAUSE
# CLUSTER SOURCE IS INCLUDED IN TINKERCELLCORE

INCLUDE_DIRECTORIES( 
	BEFORE
	${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster
	${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src
)

FILE( GLOB CLUSTER_SRC ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src/*.c)

IF (WIN32 AND MINGW)
	ADD_DEFINITIONS(-DDBL_EPSILON=2.22045e-16)
	ADD_DEFINITIONS(-DDBL_MIN=1e-999)
	ADD_DEFINITIONS(-DDBL_MIN_EXP=-999)
	ADD_DEFINITIONS(-DDBL_MAX=1.79769e+308)
ENDIF (WIN32 AND MINGW)
	
ADD_LIBRARY( cluster
#  SHARED
  STATIC
  ${CLUSTER_SRC} 
)

