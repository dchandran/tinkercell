SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

 INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/libsbml
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/libsbml/src
    ${TINKERCELL_SOURCE_DIR}/c/sbml_sim
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
 
IF (WIN32)
   ADD_DEFINITIONS(-D_WIN32)
ENDIF (WIN32)

ADD_LIBRARY( sbml_sim 
	SHARED 
	sbml_sim.cpp )

TARGET_LINK_LIBRARIES( sbml_sim 
   sbml_sim 
   sbml 
   ode
   ssa 
   muparser
)
 
#IF ( WIN32 )
#  INSTALL(TARGETS sbml_sim DESTINATION bin/../)
#ELSE ( WIN32 )
#  INSTALL(TARGETS sbml_sim LIBRARY DESTINATION bin/../)
#ENDIF( WIN32 )

