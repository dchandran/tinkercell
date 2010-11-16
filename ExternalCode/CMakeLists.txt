CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

#  All of these are third-party libraries

SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

ADD_SUBDIRECTORY( muparser )  #Math parser
#ADD_SUBDIRECTORY( curl )      #needed by raptor
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
ADD_SUBDIRECTORY( sbw )        #systems biology workbench (Core only)
ADD_SUBDIRECTORY( copasi )     #simulation and analysis package
ADD_SUBDIRECTORY( BioCoder )   #wet-lab protocol generator
