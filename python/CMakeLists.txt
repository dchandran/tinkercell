SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/python)

OPTION( EMBED_PYTHON "Embed Python in TinkerCell" OFF )
FIND_PACKAGE( PythonLibs )

SET(PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_PATH})

IF(EMBED_PYTHON)

   IF (NOT (IS_DIRECTORY ${PYTHON_INCLUDE_DIR} AND EXISTS ${PYTHON_LIBRARY}))
      MESSAGE(WARNING "Path PYTHON_INCLUDE_DIR and file-path PYTHON_LIBRARY not found. Use Add Entry to add them")
   ENDIF(NOT (IS_DIRECTORY ${PYTHON_INCLUDE_DIR} AND EXISTS ${PYTHON_LIBRARY}))

  IF ( WIN32 )
      ADD_DEFINITIONS(-D_WIN32)
      ADD_DEFINITIONS(-DNODLOPEN)
  ENDIF( WIN32 )
  
  IF ( APPLE )
      SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Python")
      ADD_DEFINITIONS(-DNODLOPEN)
  ENDIF ( APPLE )

  INCLUDE_DIRECTORIES( BEFORE ${PYTHON_INCLUDE_DIR} )

  CONFIGURE_FILE( runpy.c.in 
    ${TINKERCELL_BINARY_DIR}/python/runpy.c
   @ONLY
  )
  ADD_DEFINITIONS(-DLIB_EXPORTS=0)
  
  ADD_LIBRARY( tcpy
  	  SHARED
	  ${TINKERCELL_BINARY_DIR}/python/runpy.c
#      runpy.c
	  tinkercell_wrap.c
  )
    
  TARGET_LINK_LIBRARIES( tcpy
    tcpy
    tinkercellapi
    ${PYTHON_LIBRARY}
  )
  
  IF ( WIN32 )
    ADD_CUSTOM_COMMAND ( 
          TARGET tcpy POST_BUILD
          COMMAND move /Y ..\\bin\\python\\libtcpy.dll ..\\bin\\python\\_tinkercell.pyd
     )
     FILE(GLOB PYTHON_LOADER ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.pyd)
  ELSE ( WIN32 )
     IF( APPLE )
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND mv ${TINKERCELL_BINARY_DIR}/bin/python/libtcpy.dylib ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.dylib
        )
        FILE(GLOB PYTHON_LOADER ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.dylib)
    ELSE( APPLE )
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD 
              COMMAND mv ${TINKERCELL_BINARY_DIR}/bin/python/libtcpy.so ${TINKERCELL_BINARY_DIR}/bin/python/_tinkercell.so
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
  
ENDIF(EMBED_PYTHON)

