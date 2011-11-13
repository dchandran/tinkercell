SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})
ADD_DEFINITIONS(-DTC_EXPORTS)

OPTION( USE_COPASI "Use COPASI for simulations" ON )

IF ( UNIX )
    ADD_CUSTOM_COMMAND ( 
        TARGET tinkercellapi POST_BUILD
        COMMAND source doSwig.sh
    )
ENDIF ( UNIX )

#***************************************
#    ODE and SSA libraries
#***************************************

ADD_DEFINITIONS(-DNO_FPRINTF_OUTPUT)
ADD_LIBRARY( ode
  STATIC
  eigen.c
  cvodesim.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim/mat.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim/neldermead.c
)

TARGET_LINK_LIBRARIES( ode ode lapack blas f2c sundials_cvode sundials_nvecserial )

SET_TARGET_PROPERTIES(ode PROPERTIES LINKER_LANGUAGE C)

ADD_LIBRARY( ssa
	STATIC
	mtrand.c
	ssa.c
	cells_ssa.c
)

SET_TARGET_PROPERTIES(ssa PROPERTIES LINKER_LANGUAGE C)
INSTALL(TARGETS ssa ode
	RUNTIME DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/lib
	LIBRARY DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/lib
	ARCHIVE DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/lib
)

#*******************************************************************************

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/c
    ${TINKERCELL_SOURCE_DIR}/ExternalCode
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

ADD_SUBDIRECTORY( sbml_sim )

IF (USE_COPASI)	 #use COPASI for the numerical analysis (more features)
	 
	 #all the simulation work is handled by C++ plugins
	INSTALL( FILES 
			${TINKERCELL_SOURCE_DIR}/API/TC_structs.h
			${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/copasi_api.h
			${TINKERCELL_SOURCE_DIR}/ExternalCode/lapack/INCLUDE/f2c.h
			${TINKERCELL_SOURCE_DIR}/ExternalCode/lapack/INCLUDE/clapack.h
			${TINKERCELL_SOURCE_DIR}/ExternalCode/lapack/INCLUDE/blaswrap.h
	  DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/c
	)

ELSE(USE_COPASI)   #use custom programs for the numerical analysis (less features, but faster)
	INCLUDE( CustomSimulationPrograms.cmake )
ENDIF (USE_COPASI)

INCLUDE_DIRECTORIES( BEFORE
	${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve
)

ADD_LIBRARY( lpsolve
  SHARED
  lpsolve.c
)

TARGET_LINK_LIBRARIES( lpsolve lpsolve lp_solve tinkercellapi )

INSTALL(TARGETS lpsolve 
	RUNTIME DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/plugins/c
	LIBRARY DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/plugins/c
	ARCHIVE DESTINATION ${TINKERCELL_PACKAGE_ROOT_DIR}/plugins/c
)

