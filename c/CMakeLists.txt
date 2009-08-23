SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

IF( WIN32 AND NOT UNIX)
   OPTION( CMAKE_WIN32 "This is a Windows OS" ON )
ELSE( WIN32 AND NOT UNIX)
   OPTION( CMAKE_WIN32 "This is a Windows OS" OFF )
ENDIF( WIN32 AND NOT UNIX)

ADD_LIBRARY( runlangevin
  SHARED
  runlangevin.c
)

INSTALL(TARGETS runlangevin RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( runsteadystate 
  SHARED
  runsteadystate.c
)

INSTALL(TARGETS runsteadystate RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( runcvode 
  SHARED
  runcvode.c
)

INSTALL(TARGETS runcvode RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( runssa 
  SHARED
  runssa.c
)

INSTALL(TARGETS runssa RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( runloops 
  SHARED
  runLoops.c
)

INSTALL(TARGETS runloops RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( loadFullBindingKinetics 
  SHARED
  loadFullBindingKinetics.c
)

INSTALL(TARGETS loadFullBindingKinetics RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( runbistablega 
  SHARED
  runbistablega.c
)

INSTALL(TARGETS runbistablega RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( runvaluesattime 
  SHARED
  runvaluesattime.c
)

INSTALL(TARGETS runvaluesattime RUNTIME DESTINATION Plugins/c)

ADD_LIBRARY( tabasco_like 
  SHARED
  tabasco_like.c
)

INSTALL(TARGETS tabasco_like RUNTIME DESTINATION Plugins/c)

ADD_SUBDIRECTORY( lpsolve )

SET( LPSOLVE_SRC
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_MDO.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/shared/commonlib.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/colamd/colamd.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/shared/mmio.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/shared/myblas.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/ini.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_rlp.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_crash.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/bfp_LUSOL/lp_LUSOL.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/bfp_LUSOL/LUSOL/lusol.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_Hash.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_lib.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_wlp.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_matrix.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_mipbb.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_MPS.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_params.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_presolve.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_price.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_pricePSE.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_report.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_scale.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_simplex.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_SOS.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/lp_utils.c
  ${TINKERCELL_SOURCE_DIR}/c/lpsolve/yacc_read.c
)

ADD_LIBRARY( lpsolve
  SHARED
  lpsolve.c
  ${LPSOLVE_SRC}
  #${LPSOLVE_COLAMD_SRC}
  #${LPSOLVE_BFP_SRC}
  #${LPSOLVE_BFPLUSOL_SRC}
  #${LUSOL_SRC}
  #${LPSOLVE_SHARED_SRC}
)

INSTALL(TARGETS lpsolve RUNTIME DESTINATION Plugins/c)

SET_TARGET_PROPERTIES( lpsolve
  PROPERTIES LINK_FLAGS "-DINLINE=static -O3 -DBFP_CALLMODEL=__stdcall -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine")

IF(PYTHONLIBS_FOUND)

  CONFIGURE_FILE( runpy.c.in 
    ${TINKERCELL_BINARY_DIR}/c/runpy.c
    @ONLY
  )

  ADD_LIBRARY( runpy 
  SHARED
  ${TINKERCELL_BINARY_DIR}/c/runpy.c 
  )
  
  TARGET_LINK_LIBRARIES( runpy
    runpy
    ${PYTHON_LIBRARIES}
  )
  
  INSTALL(TARGETS runpy RUNTIME DESTINATION Plugins/c)
  
ENDIF(PYTHONLIBS_FOUND)


##
## NOTE: what about reorganizing folder structure?
##

FILE( GLOB C_SRC "*.c" "*.h" )

INSTALL( FILES ${C_SRC}
  DESTINATION c
  COMPONENT CVODE
)

FILE( GLOB API_SRC "API/*.h" )

INSTALL( FILES ${API_SRC}
  DESTINATION c/API
  COMPONENT C_API
)

FILE( GLOB API_PYTHON_SRC "API/Python/*.c" )

INSTALL( FILES ${API_PYTHON_SRC}
  DESTINATION c/API/Python
  COMPONENT PYTHON_API
)

FILE( GLOB CVODE_HDRS "cvode/*.h" )

INSTALL( FILES ${CVODE_HDRS}
  DESTINATION c/cvode
  COMPONENT CVODE
)

FILE( GLOB SUNDIALS_HDRS "sundials/*.h" )

INSTALL( FILES ${SUNDIALS_HDRS}
  DESTINATION c/sundials
  COMPONENT CVODE
)

FILE( GLOB NVECTOR_HDRS "nvector/*.h" )

INSTALL( FILES ${NVECTOR_HDRS}
  DESTINATION c/nvector
  COMPONENT CVODE
)

FILE( GLOB CVODE_SRC_CVODE_SRC
  "cvode_src/cvode/*.h"
  "cvode_src/cvode/*.c" 
)

INSTALL( FILES ${CVODE_SRC_CVODE_SRC}
  DESTINATION c/cvode_src/cvode
  COMPONENT CVODE
)

FILE( GLOB cvode_src_cvode_fmix 
  "cvode_src/cvode/fmix/*.h" 
  "cvode_src/cvode/fmix/*.c")

INSTALL( FILES ${cvode_src_cvode_fmix} 
  DESTINATION c/cvode_src/cvode/fmix
  COMPONENT CVODE
)

FILE( GLOB cvode_src_nvec_par
  "cvode_src/nvec_par/*.h" 
  "cvode_src/nvec_par/*.c")

INSTALL( FILES ${cvode_src_nvec_par} 
  DESTINATION c/cvode_src/nvec_par 
  COMPONENT CVODE
)

FILE( GLOB cvode_src_sundials 
  "cvode_src/sundials/*.h"
  "cvode_src/sundials/*.c"
)

INSTALL( FILES ${cvode_src_sundials} 
  DESTINATION c/cvode_src/sundials
  COMPONENT CVODE
)

FILE( GLOB C_ICON_FILES icons/*.png icons/*.PNG)

INSTALL( FILES ${C_ICON_FILES}
  DESTINATION Plugins/c
  COMPONENT Plugins
)

