SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

OPTION( EMBED_OCTAVE "Embed Octave in TinkerCell" OFF )
FIND_PATH( OCTAVE_INCLUDE_DIR octave.h ${TINKERCELL_SOURCE_DIR}/octave )
FIND_LIBRARY( OCTAVE_LIBRARY NAMES cruft octave octinterp PATH  ${TINKERCELL_SOURCE_DIR}/octave )

IF(EMBED_OCTAVE AND OCTAVE_LIBRARY AND OCTAVE_INCLUDE_DIR)

   INCLUDE_DIRECTORIES( BEFORE ${OCTAVE_INCLUDE_DIR} )
   
   ADD_LIBRARY( tcoct
  	  SHARED
	  tinkercell_wrap.cpp
  )
  
  #extend
  TARGET_LINK_LIBRARIES( tcoct
    tcoct
    ${OCTAVE_LIBRARIES}
  )

  ADD_LIBRARY( tcrunoct
  	  SHARED
  	  runoctave.c
  )

  #enbed
  TARGET_LINK_LIBRARIES( tcrunoct
    tcrunoct
    tinkercellapi
    ${OCTAVE_LIBRARIES}
  )

  IF ( WIN32 )
    
    ADD_CUSTOM_COMMAND (
          TARGET tcoct POST_BUILD
          COMMAND copy ${TINKERCELL_BINARY_DIR}/bin/plugins/c/tcoct.dll ${TINKERCELL_BINARY_DIR}/octave/tcoct.oct
      )
    
  ELSE ( WIN32 )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND (
              TARGET tcoct POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/plugins/c/libtcoct.so ${TINKERCELL_BINARY_DIR}/octave/tcoct.oct
        )
  ENDIF( WIN32 )
  
  FILE(GLOB OCTAVE_SCRIPTS *.m ${TINKERCELL_BINARY_DIR}/octave/tcoct.oct)

   INSTALL(FILES ${OCTAVE_SCRIPTS}
     DESTINATION octave
     COMPONENT OctaveScripts
   )
  
ENDIF(EMBED_OCTAVE AND OCTAVE_LIBRARY AND OCTAVE_INCLUDE_DIR)

