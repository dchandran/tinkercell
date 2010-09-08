FILE(GLOB DOC_FILES *.html *.pdf *.htm)
FILE(GLOB UserDocumentation_images UserDocumentation_images/*.PNG UserDocumentation_images/*.png)
FILE(GLOB PartGraphics_Documentat_images PartGraphics_Documentat_images/*.PNG PartGraphics_Documentat_images/*.png)

INSTALL( FILES ${DOC_FILES} 
  DESTINATION Documentation
)

INSTALL( FILES ${UserDocumentation_images} 
  DESTINATION Documentation/UserDocumentation_images
)

INSTALL( FILES ${PartGraphics_Documentat_images} 
  DESTINATION Documentation/PartGraphics_Documentat_images
)

IF( DOXYGENDOC )

FIND_PACKAGE( Doxygen )

IF( DOXYGEN_FOUND )
    IF( DOXYGEN_DOT_FOUND )
      ADD_SUBDIRECTORY( Doxygen )
    ELSE( DOXYGEN_DOT_FOUND )
      MESSAGE( SEND_ERROR 
        "Dot MUST be installed (visit http://www.graphviz.org/)" )
    ENDIF( DOXYGEN_DOT_FOUND )
  ELSE( DOXYGEN_FOUND )
    MESSAGE( SEND_ERROR
      "Doxygen MUST be installed 
        (visit http://www.stack.nl/~dimitri/doxygen/)" )
  ENDIF( DOXYGEN_FOUND )
ENDIF( DOXYGENDOC )

