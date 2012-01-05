#------------------
# include dirs
#------------------

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/ModelingTools
	${TINKERCELL_SOURCE_DIR}/ModuleTools
    ${TINKERCELL_SOURCE_DIR}/Core/plugins
	${TINKERCELL_SOURCE_DIR}/Core/fileIO
	${TINKERCELL_SOURCE_DIR}/Core/interpreters
	${TINKERCELL_SOURCE_DIR}/Core/coding
	${TINKERCELL_SOURCE_DIR}/Core/plots
)

#------------------------
#  configure code
#------------------------

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/BasicTools/TinkerCellAboutBox.cpp.in
	${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
	@ONLY
)

#------------------
# header files
#------------------

ADD_DEFINITIONS(-DTC_EXPORTS)
FILE( GLOB ONTOLOGY_MAIN_HRDS
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.h
	${TINKERCELL_SOURCE_DIR}/BasicTools/*.h
	${TINKERCELL_SOURCE_DIR}/ModelingTools/ParametersTool.h
	${TINKERCELL_SOURCE_DIR}/ModelingTools/SpinBoxDelegate.h
)

QT4_WRAP_CPP( ONTOLOGY_MAIN_MOC ${ONTOLOGY_MAIN_HRDS})


#------------------
# source files
#------------------

FILE( GLOB ONTOLOGY_MAIN_SRC
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.cpp
	${TINKERCELL_SOURCE_DIR}/BasicTools/*.cpp
	${TINKERCELL_SOURCE_DIR}/ModelingTools/ParametersTool.cpp
	${TINKERCELL_BINARY_DIR}/BasicTools/*.cpp
	${TINKERCELL_SOURCE_DIR}/ModelingTools/SpinBoxDelegate.cpp
	Main.cpp
)


#create the main program and link it to the TinkerCellCore library  
ADD_EXECUTABLE( OntologyExample ${ONTOLOGY_MAIN_SRC} ${ONTOLOGY_MAIN_MOC})
TARGET_LINK_LIBRARIES( OntologyExample TinkerCellCore )

