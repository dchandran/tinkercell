SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES( BEFORE
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libstructural
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libstructural/include
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi
)

FILE(GLOB LIBSTRUCT_SRC
    LibLA/*.cpp
    LibStructural/libstructural.cpp
)
  
ADD_DEFINITIONS(-DNO_SBML)
ADD_DEFINITIONS(-D__cplusplus)

ADD_LIBRARY ( structural
#   SHARED
  STATIC
  ${LIBSTRUCT_SRC}
)

TARGET_LINK_LIBRARIES( structural
  structural
  lapack
  blas
  f2c
)

