FILE(GLOB DOC_FILES *.html *.pdf *.htm *.PDF *.HTML *.HTM)

INSTALL( FILES ${DOC_FILES} 
  DESTINATION Documentation
)

#IF( DOXYGENDOC )

#FIND_PACKAGE( Doxygen )

#IF( DOXYGEN_FOUND )
#    IF( DOXYGEN_DOT_FOUND )
#      ADD_SUBDIRECTORY( Doxygen )
#    ELSE( DOXYGEN_DOT_FOUND )
#      MESSAGE( SEND_ERROR 
#        "Dot MUST be installed (visit http://www.graphviz.org/)" )
#    ENDIF( DOXYGEN_DOT_FOUND )
#  ELSE( DOXYGEN_FOUND )
#    MESSAGE( SEND_ERROR
#      "Doxygen MUST be installed 
#        (visit http://www.stack.nl/~dimitri/doxygen/)" )
#  ENDIF( DOXYGEN_FOUND )
#ENDIF( DOXYGENDOC )

