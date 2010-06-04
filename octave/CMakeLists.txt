SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

FIND_PATH( OCTAVE_INCLUDE_DIR octave.h ${TINKERCELL_SOURCE_DIR}/octave )
FIND_LIBRARY( OCTAVE_LIBRARY NAMES cruft octave octinterp PATH  ${TINKERCELL_SOURCE_DIR}/octave )

IF(OCTAVE_LIBRARY AND OCTAVE_INCLUDE_DIR)

   INCLUDE_DIRECTORIES( BEFORE ${OCTAVE_INCLUDE_DIR} )
    
   FILE(GLOB OCTAVE_SCRIPTS *.m)

   INSTALL(FILES ${OCTAVE_SCRIPTS}
     DESTINATION octave
     COMPONENT OctaveScripts
   )

  ADD_LIBRARY( tcoct
  	  SHARED
  	  runoctave.c
	  tinkercell_wrap.cxx
  )
    
  TARGET_LINK_LIBRARIES( tcpy
    tcpy
    tinkercellapi
    ${OCTAVE_LIBRARIES}
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
  
ENDIF(OCTAVE_LIBRARY AND OCTAVE_INCLUDE_DIR)

