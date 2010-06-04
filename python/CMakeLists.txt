SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})
FIND_PACKAGE( PythonLibs )

IF(PYTHONLIBS_FOUND)

   INCLUDE_DIRECTORIES( BEFORE ${PYTHON_INCLUDE_PATH} )
    
   FILE(GLOB PYTHON_SCRIPTS *.py)

   INSTALL(FILES ${PYTHON_SCRIPTS}
     DESTINATION python
     COMPONENT PythonScripts
   )

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
          COMMAND copy ${TINKERCELL_BINARY_DIR}/plugins/c/runpy.dll ${CMAKE_INSTALL_PREFIX}/python/_tinkercell.pyd
      )
    
  ELSE ( WIN32 )
     IF( APPLE )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/plugins/c/librunpy.dylib ${CMAKE_INSTALL_PREFIX}/python/_tinkercell.dylib
        )
    ELSE( APPLE )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/plugins/c/librunpy.so ${CMAKE_INSTALL_PREFIX}/python/_tinkercell.so
        )    
    ENDIF( APPLE )
  ENDIF( WIN32 )
  
ENDIF(PYTHONLIBS_FOUND)

