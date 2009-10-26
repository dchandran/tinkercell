SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

SUBDIRS( cvode
#  nvec_par
  nvec_ser
  sundials
  )

ADD_LIBRARY( odesim
 SHARED
 ${CVODE_SRC}
 ${SUNDIALS_SRC}
 ${NVECSER_SRC}
 ${TINKERCELL_SOURCE_DIR}/c/cvodesim.c
 )
 
IF ( WIN32 )
  INSTALL(TARGETS odesim DESTINATION c)
ELSE ( WIN32 )
  INSTALL(TARGETS odesim LIBRARY DESTINATION c)
ENDIF( WIN32 )

#TARGET_LINK_LIBRARIES( odesim
#  odesim
#  langevin
#  ssa
#  ga_bistable
#  runcvode
#  runvaluesattime
#  runsteadystate
#)


FILE( GLOB CVODE_SRC_CVODE_SRC
  "cvode/*.h"
  "cvode/*.c" 
)

INSTALL( FILES ${CVODE_SRC_CVODE_SRC}
  DESTINATION c/cvode_src/cvode
  COMPONENT CVODE
)

FILE( GLOB cvode_src_cvode_fmix 
  "cvode/fmix/*.h" 
  "cvode/fmix/*.c")

INSTALL( FILES ${cvode_src_cvode_fmix} 
  DESTINATION c/cvode_src/cvode/fmix
  COMPONENT CVODE
)

FILE( GLOB cvode_src_nvec_par
  "nvec_par/*.h" 
  "nvec_par/*.c")

INSTALL( FILES ${cvode_src_nvec_par} 
  DESTINATION c/cvode_src/nvec_par 
  COMPONENT CVODE
)

FILE( GLOB cvode_src_nvec_ser
  "nvec_ser/*.h" 
  "nvec_ser/*.c")

INSTALL( FILES ${cvode_src_nvec_ser} 
  DESTINATION c/cvode_src/nvec_ser 
  COMPONENT CVODE
)

FILE( GLOB cvode_src_sundials 
  "sundials/*.h"
  "sundials/*.c"
)

INSTALL( FILES ${cvode_src_sundials} 
  DESTINATION c/cvode_src/sundials
  COMPONENT CVODE
)
