SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

OPTION( EMBED_PYTHON "Embed Python in TinkerCell" OFF )
FIND_PACKAGE( PythonLibs )

IF(EMBED_PYTHON AND PYTHONLIBS_FOUND)
  IF ( WIN32 )
      ADD_DEFINITIONS(-D_WIN32)
  ENDIF( WIN32 )

  INCLUDE_DIRECTORIES( BEFORE ${PYTHON_INCLUDE_PATH} )

#  CONFIGURE_FILE( runpy.c.in 
#    ${TINKERCELL_BINARY_DIR}/python/runpy.c
#   @ONLY
#  )
  
  ADD_LIBRARY( tcpy
  	  SHARED
#	  ${TINKERCELL_BINARY_DIR}/python/runpy.c
      runpy.c
	  tinkercell_wrap.c
  )
    
  TARGET_LINK_LIBRARIES( tcpy
    tcpy
    tinkercellapi
    ${PYTHON_LIBRARIES}
  )
  
  IF ( WIN32 )
    ADD_CUSTOM_COMMAND ( 
          TARGET tcpy POST_BUILD
          COMMAND move /Y ..\\bin\\plugins\\c\\libtcpy.dll ..\\bin\\python\\_tinkercell.pyd
     )
     FILE(GLOB PYTHON_LOADER ${TINKERCELL_BINARY_DIR}/python/_tinkercell.pyd)
  ELSE ( WIN32 )
     IF( APPLE )
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/plugins/c/libtcpy.dylib ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.dylib
        )
        FILE(GLOB PYTHON_LOADER ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.dylib)
    ELSE( APPLE )
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD 
              COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/plugins/c/libtcpy.so ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.so
        )
        FILE(GLOB PYTHON_LOADER ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.so)
    ENDIF( APPLE )
  ENDIF( WIN32 )
  
  FILE(GLOB PYTHON_SCRIPTS *.py)

  INSTALL(FILES ${PYTHON_SCRIPTS}
     DESTINATION python
     COMPONENT Python
  )
  
  INSTALL(FILES ${PYTHON_LOADER}
     DESTINATION python
     COMPONENT Python
  )
  
ENDIF(EMBED_PYTHON AND PYTHONLIBS_FOUND)

