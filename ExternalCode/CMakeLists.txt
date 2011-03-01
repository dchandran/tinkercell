CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

#  All of these are third-party libraries

SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

OPTION( BUILD_SBWCORE "systems biology workbench core" OFF )
OPTION( BUILD_BIOCODER "wet-lab protocol coder" OFF )

ADD_SUBDIRECTORY( muparser )  #Math parser
ADD_SUBDIRECTORY( curl )      #needed by raptor
ADD_SUBDIRECTORY( expat )      #small XML parser
ADD_SUBDIRECTORY( raptor2 )   #RDF parser
ADD_SUBDIRECTORY( raptor )    #RDF parser
ADD_SUBDIRECTORY( qwt )       #plot drawing using Qt
ADD_SUBDIRECTORY( qwtplot3d )  #drawing 3D plots using Qt
ADD_SUBDIRECTORY( optim )      #Nelder-Mead optimization
ADD_SUBDIRECTORY( cvode260 )   #numerical integration (stiff solver)
ADD_SUBDIRECTORY( lapack )     #linear algebra
ADD_SUBDIRECTORY( lpsolve )    #linear programming
ADD_SUBDIRECTORY( libSBML )    #systems biology markup language parser
ADD_SUBDIRECTORY( Antimony )   #human-readable test-based reaction networks
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
