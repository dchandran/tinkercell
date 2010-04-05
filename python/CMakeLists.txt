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
	  ${TINKERCELL_BINARY_DIR}/python/runpy.c
	  tinkercell_wrap.c
  )
    
  TARGET_LINK_LIBRARIES( _tinkercell
    _tinkercell
    tinkercellapi
    ${PYTHON_LIBRARIES}
  )
  
  SET_TARGET_PROPERTIES( _tinkercell PROPERTIES PREFIX "")
  
  IF ( UNIX )
    ADD_CUSTOM_COMMAND( 
        TARGET _tinkercell POST_BUILD
        COMMAND cat _tinkercell.py tc2py.py > tinkercell.py
  	    COMMAND mv tinkercell.py _tinkercell.py
	)
  ENDIF ( UNIX )
  
  IF ( WIN32 )
	SET_TARGET_PROPERTIES( _tinkercell PROPERTIES SUFFIX ".pyd" )
    INSTALL(TARGETS _tinkercell DESTINATION python)
  ELSE ( WIN32 )
    INSTALL(TARGETS _tinkercell LIBRARY DESTINATION python)
  ENDIF( WIN32 )
  
ENDIF(PYTHONLIBS_FOUND)

