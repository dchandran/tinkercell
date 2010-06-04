SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})
FIND_PACKAGE( Perl )
FIND_PACKAGE( PerlLibs )

IF(PERLLIBS_FOUND AND PERL_FOUND)

   INCLUDE_DIRECTORIES( BEFORE ${PERL_INCLUDE_PATH} )
   
   EXECUTE_PROCESS ( COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ccopts
        OUTPUT_VARIABLE PERL_OUTPUT
        RESULT_VARIABLE PERL_RETURN_VALUE )
   
   IF ( NOT PERL_RETURN_VALUE )
       SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PERL_OUTPUT}" )

       FILE(GLOB PERL_SCRIPTS *.pm)

       INSTALL(FILES ${PERL_SCRIPTS}
          DESTINATION perl
          COMPONENT PerlScripts
       )
  
      ADD_LIBRARY( tcperl
  	      SHARED
    	  tinkercell_wrap.c
	      runperl.c
      )
    
      TARGET_LINK_LIBRARIES( tcperl
        tcperl
        tinkercellapi
        ${PERL_LIBRARIES}
      )

  #SET_TARGET_PROPERTIES( tcperl PROPERTIES PREFIX "")

  IF ( WIN32 )
	#SET_TARGET_PROPERTIES( tcpy PROPERTIES SUFFIX ".pyd" )
    #INSTALL(TARGETS tcpy DESTINATION python)
    
    ADD_CUSTOM_COMMAND ( 
          TARGET tcpy POST_BUILD
          COMMAND copy ${TINKERCELL_BINARY_DIR}/plugins/c/tcperl.dll ${CMAKE_INSTALL_PREFIX}/perl/_tinkercell.pyd
      )
    
  ELSE ( WIN32 )
     IF( APPLE )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/plugins/c/libtcperl.dylib ${CMAKE_INSTALL_PREFIX}/perl/_tinkercell.dylib
        )
    ELSE( APPLE )
        #INSTALL(TARGETS tcpy LIBRARY DESTINATION python)
        ADD_CUSTOM_COMMAND ( 
              TARGET tcpy POST_BUILD
              COMMAND cp ${TINKERCELL_BINARY_DIR}/plugins/c/libtcperl.so ${CMAKE_INSTALL_PREFIX}/perl/_tinkercell.so
        )    
    ENDIF( APPLE )
  ENDIF( WIN32 )
  
  ENDIF ( NOT PERL_RETURN_VALUE )
ENDIF(PERLLIBS_FOUND AND PERL_FOUND)

