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
  
      ADD_LIBRARY( tinkercell
  	      SHARED
    	  tinkercell_wrap.c
	      runperl.c
      )
    
      TARGET_LINK_LIBRARIES( tinkercell
        tinkercell
        tinkercellapi
        ${PERL_LIBRARIES}
      )
  
      SET_TARGET_PROPERTIES( tinkercell PROPERTIES PREFIX "")
  
      IF ( WIN32 )
         INSTALL(TARGETS tinkercell DESTINATION perl)
      ELSE ( WIN32 )
         INSTALL(TARGETS tinkercell LIBRARY DESTINATION perl)
      ENDIF( WIN32 )
     
  ENDIF ( NOT PERL_RETURN_VALUE )
ENDIF(PERLLIBS_FOUND)

