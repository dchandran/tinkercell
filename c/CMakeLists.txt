SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

ADD_LIBRARY( langevin 
  mtrand.c
  langevin.c
)

ADD_LIBRARY( runsteadystate 
  SHARED
  runsteadystate.c 
)

TARGET_LINK_LIBRARIES( runsteadystate
  runsteadystate
  odesim
)

ADD_LIBRARY( runcvode 
  SHARED
  runcvode.c 
)

TARGET_LINK_LIBRARIES( runcvode
  runcvode
  odesim
)


ADD_LIBRARY( runssa 
  SHARED
  mtrand.c
  ssa.c
  runssa.c 
)

ADD_LIBRARY( runloops 
  SHARED
  runLoops.c
)

ADD_LIBRARY( loadFullBindingKinetics 
  SHARED
  loadFullBindingKinetics.c 
)

ADD_LIBRARY( runbistablega 
  SHARED
  mtrand.c
  ga.c
  ga_bistable.c
  runbistablega.c 
)

TARGET_LINK_LIBRARIES( runbistablega
  runbistablega
  odesim
)

ADD_LIBRARY( runvaluesattime 
  SHARED
  mtrand.c
  ssa.c
  runvaluesattime.c
)

TARGET_LINK_LIBRARIES( runvaluesattime
  runvaluesattime
  odesim
)

ADD_LIBRARY( tabasco_like 
  SHARED
  tabasco_like.c
)

#ADD_SUBDIRECTORY( lpsolve )

ADD_LIBRARY( lpsolve
  SHARED
  lpsolve.c
  #${LPSOLVE_SRC}
  #${LPSOLVE_COLAMD_SRC}
  #${LPSOLVE_BFP_SRC}
  #${LPSOLVE_BFPLUSOL_SRC}
  #${LUSOL_SRC}
  #${LPSOLVE_SHARED_SRC}
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

SET_TARGET_PROPERTIES( lpsolve
  PROPERTIES LINK_FLAGS "-DINLINE=static -O3 -DBFP_CALLMODEL=__stdcall -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine")

ADD_LIBRARY( runpy 
  SHARED
  runpy.c 
)

TARGET_LINK_LIBRARIES( runpy
  runpy
  ${PYTHON_LIBRARY}
)


