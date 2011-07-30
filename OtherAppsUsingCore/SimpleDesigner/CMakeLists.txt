SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/muparser
	${TINKERCELL_SOURCE_DIR}/c
	${TINKERCELL_SOURCE_DIR}/c/sbml_sim
	${TINKERCELL_SOURCE_DIR}/OtherAppsUsingCore/SimpleDesigner
	${TINKERCELL_SOURCE_DIR}/Core/fileIO
	${TINKERCELL_SOURCE_DIR}/Core/plugins
	${TINKERCELL_SOURCE_DIR}/Core/coding
	${TINKERCELL_SOURCE_DIR}/Core/interpreters
	${TINKERCELL_SOURCE_DIR}/Core/plots
 )

QT4_WRAP_CPP( SIMPLEDESIGNER_MOC SimpleDesigner.h)
QT4_ADD_RESOURCES( SIMPLEDESIGNER_QRC simpledesigner.qrc )

IF( APPLE AND BUILD_BUNDLE )
  
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING
      ${TINKERCELL_VERSION}
  )
  SET( MACOSX_BUNDLE_VERSION
      ${TINKERCELL_VERSION}
  )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING
      Version ${TINKERCELL_VERSION}
  )

  ADD_EXECUTABLE( SimpleDesigner
    MACOSX_BUNDLE
    SimpleDesigner.cpp
    ${SIMPLEDESIGNER_MOC}
    ${SIMPLEDESIGNER_QRC}
  )

  TARGET_LINK_LIBRARIES( SimpleDesigner
    TinkerCellCore
    sbml
	sbml_sim
    ${QT_LIBRARIES}
  )
  
  INSTALL(TARGETS SimpleDesigner 
    BUNDLE DESTINATION bin/../
    RUNTIME DESTINATION bin/../)

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( SimpleDesigner
      SimpleDesigner.cpp
      ${SIMPLEDESIGNER_MOC}
      ${SIMPLEDESIGNER_QRC}
    )

    TARGET_LINK_LIBRARIES( SimpleDesigner
      TinkerCellCore
      sbml
      sbml_sim
      ${QT_LIBRARIES}
    )
    
  ELSE( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    IF( WIN32 )
	
	    ADD_EXECUTABLE( SimpleDesigner WIN32 
        SimpleDesigner.cpp
        ${SIMPLEDESIGNER_MOC}
        ${SIMPLEDESIGNER_QRC}
       )

      TARGET_LINK_LIBRARIES( SimpleDesigner
        TinkerCellCore
        sbml
        sbml_sim
        ${QT_LIBRARIES}
      )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )
