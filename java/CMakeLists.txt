SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/octave)

OPTION( EMBED_JAVA "Embed Java in TinkerCell" OFF )

IF(EMBED_JAVA)

FIND_PACKAGE( JAVA )
FIND_PACKAGE( JNI )
    
IF(JAVA_LIBRARY AND JAVA_VM_LIBRARY AND JAVA_INCLUDE_DIR AND JAVA_LIB_DIR)

   MESSAGE(STATUS "Using Java libraries: ${JAVA_LIBRARY} ${JAVA_VM_LIBRARY}")
   INCLUDE_DIRECTORIES( BEFORE ${JAVA_INCLUDE_DIR} )
   LINK_DIRECTORIES( BEFORE ${JAVA_LIB_DIR})

  IF ( WIN32 )
      ADD_DEFINITIONS(-D_WIN32)
      IF (MINGW)
         ADD_DEFINITIONS(-D__MINGW32__)
		 IF (USE_STDCALL)
			ADD_DEFINITIONS(-DUSE_STDCALL)
		 ENDIF(USE_STDCALL)
      ENDIF(MINGW)
  ENDIF( WIN32 )

   FILE(GLOB TCAPI_SRC ${TINKERCELL_SOURCE_DIR}/API/*.c)

   #embed
   ADD_LIBRARY( tcoct
      SHARED
      runjava.cpp
      tinkercell_wrap.cpp
  )

   TARGET_LINK_LIBRARIES( tcjava
      tcjava
      ${JAVA_LIBRARY}
      ${JAVA_VM_LIBRARY}
  )
 
  #extend
  ADD_LIBRARY( swigtcjava
      SHARED
      tinkercell_wrap.cpp
      ${TCAPI_SRC}
  )

   TARGET_LINK_LIBRARIES( swigtcjava
      swigtcjava
      ${JAVA_LIBRARY}
      ${JAVA_VM_LIBRARY}
  )
 
  INSTALL(TARGETS tcjava DESTINATION java)
  
  IF ( WIN32 )    
    ADD_CUSTOM_COMMAND (
          TARGET swigtcjava POST_BUILD
          COMMAND move /Y ..\\bin\\octave\\swigtcjava.dll ..\\bin\\octave\\tinkercell.dll
      )
  ELSE ( WIN32 )
  IF( APPLE )
    ADD_CUSTOM_COMMAND (
         TARGET swigtcjava POST_BUILD
         COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/java/swigtcjava.dylib ${TINKERCELL_BINARY_DIR}/bin/octave/tinkercell.so
    )
  ELSE ( APPLE )
    ADD_CUSTOM_COMMAND (
         TARGET swigtcjava POST_BUILD
         COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/java/swigtcjava.so ${TINKERCELL_BINARY_DIR}/bin/octave/tinkercell.so
    )
  ENDIF( APPLE )
  ENDIF( WIN32 )

  FILE(GLOB JAVA_LOADER ${TINKERCELL_BINARY_DIR}/bin/java/tinkercell.*)
  FILE(GLOB JAVA_FILES *.java ${TINKERCELL_BINARY_DIR}/java/*.java)

  INSTALL(FILES ${JAVA_FILES}
     DESTINATION java
     COMPONENT Java
   )

  INSTALL(FILES ${JAVA_LOADER}
     DESTINATION java
     COMPONENT Java
   )
ELSE(JAVA_LIBRARY AND JAVA_VM_LIBRARY AND JAVA_INCLUDE_DIR AND JAVA_LIB_DIR)

    MESSAGE("Please define the following FILEPATH items: JAVA_LIBRARY JAVA_VM_LIBRARY JAVA_INCLUDE_DIR JAVA_LIB_DIR")

ENDIF(JAVA_LIBRARY AND JAVA_VM_LIBRARY AND AVA_INCLUDE_DIR AND JAVA_LIB_DIR)

ENDIF(EMBED_JAVA)

