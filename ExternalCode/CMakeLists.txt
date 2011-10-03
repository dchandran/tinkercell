CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

INCLUDE_DIRECTORIES( BEFORE
	${TINKERCELL_SOURCE_DIR}/c
    ${TINKERCELL_SOURCE_DIR}/ExternalCode
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/curl/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/expat
	${TINKERCELL_SOURCE_DIR}/ExternalCode/expat/lib
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/muparser
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lapack/INCLUDE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/cvode
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/cvode
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/sundials
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/sundials
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/nvector
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/nvector
)

#  All of these are third-party libraries

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TINKERCELL_BINARY_DIR})

OPTION( BUILD_LAPACK "check this if your system does not have lapack library" ON )
OPTION( BUILD_SBWCORE "systems biology workbench core" OFF )
OPTION( BUILD_BIOCODER "wet-lab protocol coder" OFF )

#Qwt and QwtPlot3D are not included in TinkerCellCore
#ADD_SUBDIRECTORY( qwt )       #plot drawing using Qt
#ADD_SUBDIRECTORY( qwtplot3d )  #drawing 3D plots using Qt

ADD_SUBDIRECTORY( muparser )  #math parser
ADD_SUBDIRECTORY( curl )      #needed by raptor
#ADD_SUBDIRECTORY( expat )      #small XML parser
ADD_SUBDIRECTORY( raptor2 )   #RDF parser
ADD_SUBDIRECTORY( raptor )    #RDF parser
ADD_SUBDIRECTORY( optim )      #Nelder-Mead optimization
ADD_SUBDIRECTORY( cvode260 )   #numerical integration (stiff solver)
IF (BUILD_LAPACK)
	ADD_SUBDIRECTORY( lapack )     #linear algebra
ENDIF(BUILD_LAPACK)
ADD_SUBDIRECTORY( lpsolve )    #linear programming
ADD_SUBDIRECTORY( libSBML )    #systems biology markup language parser
ADD_SUBDIRECTORY( Antimony )   #human-readable test-based reaction networks
ADD_SUBDIRECTORY( boost )      #regular expressions
ADD_SUBDIRECTORY( copasi )     #simulation and analysis package
ADD_SUBDIRECTORY( libstructural )   #network structural analysis based on stoichiometry matrix
ADD_SUBDIRECTORY( galib )   #genetic algorithms library
ADD_SUBDIRECTORY( cluster )   #clustering algorithms
IF (BUILD_SBWCORE)
	ADD_SUBDIRECTORY( sbw )        #systems biology workbench (Core only)
ENDIF (BUILD_SBWCORE)
IF (BUILD_BIOCODER)	
	ADD_SUBDIRECTORY( BioCoder )   #wet-lab protocol generator
ENDIF (BUILD_BIOCODER)
