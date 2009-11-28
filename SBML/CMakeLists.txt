#----------
# basic
#----------

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
   cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)


#-------------------------------
# include and link directories
#-------------------------------

INCLUDE_DIRECTORIES( 
	BEFORE
    ${TINKERCELL_SOURCE_DIR}/SBML/
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml
)

SET(LIBRARY_OUTPUT_PATH ${TINKERCELL_BINARY_DIR})

#----------
# libSBML
#----------

IF (WIN32)

	SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/SBML/libsbml/win32/libxml2/lib/libxml2.dll.a)
	SET (LIBXML2_INCLUDE_DIR ${TINKERCELL_SOURCE_DIR}/SBML/libsbml/win32/libxml2/include)
	
ELSEIF(WIN32)	

	FIND_PACKAGE(LIBXML2 REQUIRED)
	
ENDIF(WIN32)

INCLUDE_DIRECTORIES( 
	BEFORE
	${LIBXML2_INCLUDE_DIR}
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/xml 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/util 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/validator 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/validator/constraints/
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/units 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/sbml
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/sbml/layout 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/math 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/common 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/compress 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/annotation
)

#ADD_DEFINITIONS(-DLIBSBML_STATIC)
#ADD_DEFINITIONS(-D__cplusplus)
ADD_DEFINITIONS(-DLIBSBML_EXPORTS)
ADD_DEFINITIONS(-DLIBLAX_EXPORTS)
ADD_DEFINITIONS(-DCYGWIN)
ADD_DEFINITIONS(-DPACKAGE_NAME=\"SBML\")
ADD_DEFINITIONS(-DWIN32)
ADD_DEFINITIONS(-DNDEBUG)
ADD_DEFINITIONS(-DUSE_LIBXML)
ADD_DEFINITIONS(-DUSE_LAYOUT)
ADD_DEFINITIONS(-D_WINDOWS)
ADD_DEFINITIONS(-D_USRDLL)
ADD_DEFINITIONS(-D_EXPORTS)

IF (WIN32)
   ADD_DEFINITIONS(-DWIN32)
ENDIF (WIN32)

FILE( GLOB SBML_SRC_FILES
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/xml/XML*.cpp 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/xml/XML*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/xml/LibXML*.cpp 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/xml/LibXML*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/util/*.cpp 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/util/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/validator/*.cpp 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/validator/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/validator/constraints/*.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/validator/constraints/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/units/*.cpp 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/units/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/math/*.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/math/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/annotation/*.cpp 
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/annotation/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/sbml/*.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/sbml/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/sbml/layout/*.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/sbml/layout/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/common/*.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/common/*.c
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/compress/CompressCommon.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/compress/InputDecompressor.cpp
	${TINKERCELL_SOURCE_DIR}/SBML/libsbml/compress/OutputCompressor.cpp
)

ADD_LIBRARY( sbml
  #SHARED
  STATIC
  ${SBML_SRC_FILES}
)

TARGET_LINK_LIBRARIES( sbml
  sbml
  ${LIBXML2_LIBRARIES}
)


IF ( WIN32 )
  INSTALL(TARGETS sbml DESTINATION bin/../)
ELSE ( WIN32 )
  INSTALL(TARGETS sbml LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/TextParser.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

FILE( GLOB SBML_TOOL_HDRS *.h )
FILE( GLOB SBML_TOOL_SRC *.cpp )

QT4_WRAP_CPP( SBML_TOOL_MOC ${SBML_TOOL_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( SBMLTool
  SHARED
  ${SBML_TOOL_SRC}
  ${SBML_TOOL_MOC}
 )

TARGET_LINK_LIBRARIES( SBMLTool
  SBMLTool
  sbml
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS SBMLTool
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS SBMLTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

