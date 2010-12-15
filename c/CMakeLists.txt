SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

OPTION( USE_COPASI "Use COPASI for simulations" ON )

IF ( UNIX )
    ADD_CUSTOM_COMMAND ( 
        TARGET tinkercellapi POST_BUILD
        COMMAND source doSwig.sh
    )
ENDIF ( UNIX )

##### Simple ODE and SSA libraries ########

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
IF ( WIN32 )
	  INSTALL(TARGETS ssa ode DESTINATION lib)
ELSE ( WIN32 )
	  INSTALL(TARGETS ssa ode ARCHIVE DESTINATION lib)
ENDIF( WIN32 )

ADD_SUBDIRECTORY( sbml_sim )

##############

IF (NOT TINKERCELL_LITE_PARTS)  #don't add any numerical packages in "parts" mode

	IF (USE_COPASI)	 #use COPASI for the numerical analysis (more features)
	     
	      #done by C++ plugin
	
	ELSE(USE_COPASI)   #use custom programs for the numerical analysis (less features, but faster)
		INCLUDE( CustomSimulationPrograms.cmake )
	ENDIF (USE_COPASI)

ENDIF (NOT TINKERCELL_LITE_PARTS)



