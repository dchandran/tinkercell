#---------------------------------------------------------------------------
# GAlib from lancet.mit.edu/ga/
#---------------------------------------------------------------------------

INCLUDE_DIRECTORIES( 
	BEFORE
	${TINKERCELL_SOURCE_DIR}/ExternalCode/galib/
	${TINKERCELL_SOURCE_DIR}/ExternalCode/galib/ga
)

FILE(GLOB GALIB_FILES ga/*.cpp )

ADD_LIBRARY( galib
  STATIC
  ${GALIB_FILES}
)
