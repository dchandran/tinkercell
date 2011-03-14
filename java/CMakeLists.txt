SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/java)

OPTION( EMBED_JAVA "Embed Java in TinkerCell" OFF )

IF(EMBED_JAVA)

FIND_PACKAGE( JNI )
    
IF(JAVA_JVM_LIBRARY AND JAVA_INCLUDE_PATH)

   MESSAGE(STATUS "Using Java library: ${JAVA_JVM_LIBRARY}")
   IF ( JAVA_INCLUDE_PATH2 )
      INCLUDE_DIRECTORIES( BEFORE ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
   ELSE ( JAVA_INCLUDE_PATH2 )
      INCLUDE_DIRECTORIES( BEFORE ${JAVA_INCLUDE_PATH})
   ENDIF ( JAVA_INCLUDE_PATH2 )
   #LINK_DIRECTORIES( BEFORE ${JAVA_LIB_DIR})

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
   ADD_LIBRARY( tcjava
      SHARED
      runjava.cpp
      #tinkercell_wrap.c
  )

   TARGET_LINK_LIBRARIES( tcjava
      tcjava
      ${JAVA_JVM_LIBRARY}
  )
 
  #extend
  ADD_LIBRARY( swigtcjava
      SHARED
      tinkercell_wrap.c
      ${TCAPI_SRC}
  )

   TARGET_LINK_LIBRARIES( swigtcjava
      swigtcjava
      ${JAVA_JVM_LIBRARY}
  )
 
  INSTALL(TARGETS tcjava DESTINATION java)
  
  IF ( WIN32 )    
    IF ( MINGW )
       ADD_CUSTOM_COMMAND (
             TARGET swigtcjava POST_BUILD
             COMMAND move /Y ..\\bin\\java\\libswigtcjava.dll ..\\bin\\java\\tinkercell.dll
         )
    ELSE ( MINGW )
       ADD_CUSTOM_COMMAND (
           TARGET swigtcjava POST_BUILD
           COMMAND move /Y ..\\bin\\java\\swigtcjava.dll ..\\bin\\java\\tinkercell.dll
       )
    ENDIF ( MINGW )
  ELSE ( WIN32 )
  IF( APPLE )
    ADD_CUSTOM_COMMAND (
         TARGET swigtcjava POST_BUILD
         COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/java/libswigtcjava.dylib ${TINKERCELL_BINARY_DIR}/bin/java/tinkercell.so
    )
  ELSE ( APPLE )
    ADD_CUSTOM_COMMAND (
         TARGET swigtcjava POST_BUILD
         COMMAND cp ${TINKERCELL_BINARY_DIR}/bin/java/libswigtcjava.so ${TINKERCELL_BINARY_DIR}/bin/java/tinkercell.so
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

ELSE(JAVA_JVM_LIBRARY AND JAVA_INCLUDE_PATH)

    MESSAGE("Please define the following FILEPATH items: JAVA_JVM_LIBRARY AND JAVA_INCLUDE_PATH")

ENDIF(JAVA_JVM_LIBRARY AND JAVA_INCLUDE_PATH)

ENDIF(EMBED_JAVA)

