ADD_SUBDIRECTORY( Doxygen )

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
