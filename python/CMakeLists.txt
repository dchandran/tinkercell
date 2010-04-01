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
  
  ADD_LIBRARY( _tinkercell
  	  SHARED
	  tinkercell_wrap.c
	  ${TINKERCELL_BINARY_DIR}/python/runpy.c 
  )
    
  TARGET_LINK_LIBRARIES( _tinkercell
    _tinkercell
    tinkercellapi
    ${PYTHON_LIBRARIES}
  )
  
  SET_TARGET_PROPERTIES( _tinkercell PROPERTIES PREFIX "")
  
  IF ( WIN32 )
    INSTALL(TARGETS _tinkercell DESTINATION python)
  ELSE ( WIN32 )
    INSTALL(TARGETS _tinkercell LIBRARY DESTINATION python)
  ENDIF( WIN32 )
  
ENDIF(PYTHONLIBS_FOUND)

