SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

FILE( GLOB SOSLIB_SRC src/*.cpp )

ADD_LIBRARY ( sbmlsolver
   SHARED
#  STATIC
  ${SOSLIB_SRC}
)

TARGET_LINK_LIBRARIES( sbmlsolver
  sbmlsolver
  sbml
)

IF ( WIN32 )
  INSTALL(TARGETS sbmlsolver DESTINATION bin/../)
ELSE ( WIN32 )
  INSTALL(TARGETS sbmlsolver LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

