SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

OPTION( EMBED_PYTHON "Embed Python in TinkerCell" OFF )
FIND_PACKAGE( PythonLibs )

IF(EMBED_PYTHON AND PYTHONLIBS_FOUND)

  INCLUDE_DIRECTORIES( BEFORE ${PYTHON_INCLUDE_PATH} )

  CONFIGURE_FILE( runpy.c.in 
    ${TINKERCELL_BINARY_DIR}/python/runpy.c
    @ONLY
  )
  
  ADD_LIBRARY( tcpy
  	  SHARED
	  ${TINKERCELL_BINARY_DIR}/python/runpy.c
	  tinkercell_wrap.c
  )
    
  TARGET_LINK_LIBRARIES( tcpy
    tcpy
    tinkercellapi
    ${PYTHON_LIBRARIES}
  )
  
  #SET_TARGET_PROPERTIES( tcpy PROPERTIES PREFIX "")
  
  IF ( WIN32 )
	#SET_TARGET_PROPERTIES( tcpy PROPERTIES SUFFIX ".pyd" )
    #INSTALL(TARGETS tcpy DESTINATION python)
    
    ADD_CUSTOM_COMMAND ( 
          TARGET tcpy POST_BUILD
          COMMAND copy ${TINKERCELL_BINARY_DIR}/bin/plugins/c/tcpy.dll ${TINKERCELL_BINARY_DIR}/python/_tinkercell.pyd
      )
    
  ELSE ( WIN32 )
     IF( APPLE )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/plugins/c/libtcpy.dylib ${TINKERCELL_BINARY_DIR}/python/_tinkercell.dylib
        )
    ELSE( APPLE )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/plugins/c/libtcpy.so ${TINKERCELL_BINARY_DIR}/python/_tinkercell.so
        )    
    ENDIF( APPLE )
  ENDIF( WIN32 )
  
  FILE(GLOB PYTHON_LOADER ${TINKERCELL_BINARY_DIR}/python/_tinkercell.*)
  FILE(GLOB PYTHON_SCRIPTS *.py)

  INSTALL(FILES ${PYTHON_SCRIPTS}
     DESTINATION python
     COMPONENT PythonScripts
  )
  
  INSTALL(FILES ${PYTHON_LOADER}
     DESTINATION python
     COMPONENT PythonScripts
  )
  
ENDIF(EMBED_PYTHON AND PYTHONLIBS_FOUND)

