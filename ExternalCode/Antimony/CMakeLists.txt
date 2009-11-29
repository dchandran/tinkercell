SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES(
	${TINKERCELL_SOURCE_DIR}/ExternalCode/Antimony/src
)

SET( ANTIMONY_SRC
	src/Antimony.cpp
    src/antimony_api.cpp
    src/antimony.tab.cpp
	src/dnastrand.cpp
	src/event.cpp
	src/formula.cpp
	src/module.cpp
	src/reactantlist.cpp
	src/reaction.cpp
	src/registry.cpp
	src/sbmlx.cpp
	src/stringx.cpp
	src/typex.cpp
	src/userfunction.cpp
	src/variable.cpp
)
   
#ADD_DEFINITIONS(-DNSBML)
ADD_DEFINITIONS(-DLIB_EXPORTS)

ADD_LIBRARY ( antimony
  SHARED
  ${ANTIMONY_SRC}
)


TARGET_LINK_LIBRARIES( antimony
  antimony
  sbml
)

IF ( WIN32 )
  INSTALL(TARGETS antimony DESTINATION bin/../)
ELSE ( WIN32 )
  INSTALL(TARGETS antimony LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )
