FIND_PACKAGE( PythonLibs )

IF(PYTHONLIBS_FOUND)

   INCLUDE(${PYTHON_INCLUDE_PATH})
    
   FILE(GLOB PYTHON_SCRIPTS *.py)

   INSTALL(FILES ${PYTHON_SCRIPTS}
     DESTINATION py
     COMPONENT PythonScripts
   )

  CONFIGURE_FILE( runpy.c.in 
    ${TINKERCELL_BINARY_DIR}/py/runpy.c
    @ONLY
  )
  
  ADD_LIBRARY( _tinkercell
  	  SHARED
	  tinkercell_wrap.c
	  ${TINKERCELL_BINARY_DIR}/py/runpy.c 
  )
    
  TARGET_LINK_LIBRARIES( _tinkercell
    _tinkercell
    tinkercellapi
    ${PYTHON_LIBRARIES}
  )
  
  SET_TARGET_PROPERTIES( _tinkercell PROPERTIES PREFIX "")
  
  IF ( WIN32 )
    INSTALL(TARGETS _tinkercell DESTINATION py)
  ELSE ( WIN32 )
    INSTALL(TARGETS _tinkercell LIBRARY DESTINATION py)
  ENDIF( WIN32 )
  
ENDIF(PYTHONLIBS_FOUND)

