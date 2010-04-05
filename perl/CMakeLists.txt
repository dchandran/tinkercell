FIND_PACKAGE( PerlLibs )

IF(PERLLIBS_FOUND)

   INCLUDE_DIRECTORIES( BEFORE ${PERL_INCLUDE_PATH} )
    
   FILE(GLOB PERL_SCRIPTS *.pm)

   INSTALL(FILES ${PERL_SCRIPTS}
     DESTINATION perl
     COMPONENT PerlScripts
   )

  ADD_LIBRARY( _tinkercell
  	  SHARED
	  tinkercell_wrap.c
	  runperl.c 
  )
    
  TARGET_LINK_LIBRARIES( _tinkercell
    _tinkercell
    tinkercellapi
    ${PERL_LIBRARIES}
  )
  
  SET_TARGET_PROPERTIES( _tinkercell PROPERTIES PREFIX "")
  
  IF ( WIN32 )
    INSTALL(TARGETS _tinkercell DESTINATION perl)
  ELSE ( WIN32 )
    INSTALL(TARGETS _tinkercell LIBRARY DESTINATION perl)
  ENDIF( WIN32 )
  
ENDIF(PERLLIBS_FOUND)

