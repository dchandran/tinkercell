CONFIGURE_FILE ( 
  ${TINKERCELL_SOURCE_DIR}/Documentation/Doxygen/Doxyfile.txt.in 
  ${TINKERCELL_BINARY_DIR}/Documentation/Doxygen/Doxyfile.txt 
) 

ADD_CUSTOM_TARGET(doc ALL 
  ${DOXYGEN_EXECUTABLE} 
  ${TINKERCELL_BINARY_DIR}/Documentation/Doxygen/Doxyfile.txt 
  DEPENDS 
  ${TINKERCELL_BINARY_DIR}/Documentation/Doxygen/Doxyfile.txt 
) 

