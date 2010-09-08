SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

IF ( UNIX )
    ADD_CUSTOM_COMMAND ( 
        TARGET tinkercellapi POST_BUILD
        COMMAND source doSwig.sh
    )
ENDIF ( UNIX )

ADD_DEFINITIONS(-DNO_FPRINTF_OUTPUT)
ADD_LIBRARY( ode
  STATIC
  eigen.c
  cvodesim.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim/mat.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim/neldermead.c
)

TARGET_LINK_LIBRARIES( ode ode lapack blas f2c sundials_cvode sundials_nvecserial )

SET_TARGET_PROPERTIES(ode PROPERTIES LINKER_LANGUAGE C)

ADD_LIBRARY( ssa
	STATIC
	mtrand.c
	ssa.c
	cells_ssa.c
)

SET_TARGET_PROPERTIES(ssa PROPERTIES LINKER_LANGUAGE C)
IF ( WIN32 )
  INSTALL(TARGETS ssa ode DESTINATION lib)
ELSE ( WIN32 )
  INSTALL(TARGETS ssa ode ARCHIVE DESTINATION lib)
ENDIF( WIN32 )

SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

ADD_LIBRARY( runsteadystate 
  SHARED
  runsteadystate.c
)
TARGET_LINK_LIBRARIES( runsteadystate runsteadystate tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS runsteadystate DESTINATION plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS runsteadystate LIBRARY DESTINATION plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( runcvode 
  SHARED
  runcvode.c
)

TARGET_LINK_LIBRARIES( runcvode runcvode tinkercellapi )

ADD_LIBRARY( runssa 
  SHARED
  runssa.c
)

TARGET_LINK_LIBRARIES( runssa runssa tinkercellapi )

ADD_LIBRARY( runloops 
  SHARED
  runLoops.c
)

TARGET_LINK_LIBRARIES( runloops runloops tinkercellapi )

ADD_LIBRARY( loadFullBindingKinetics 
  SHARED
  loadFullBindingKinetics.c
)

TARGET_LINK_LIBRARIES( loadFullBindingKinetics loadFullBindingKinetics tinkercellapi )

ADD_LIBRARY( runbistablega 
  SHARED
  runbistablega.c
)

TARGET_LINK_LIBRARIES( runbistablega runbistablega tinkercellapi )

ADD_LIBRARY( runvaluesattime 
  SHARED
  runvaluesattime.c
)

TARGET_LINK_LIBRARIES( runvaluesattime runvaluesattime tinkercellapi )

ADD_LIBRARY( tabasco_like 
  SHARED
  tabasco_like.c
)

TARGET_LINK_LIBRARIES( tabasco_like tabasco_like tinkercellapi )

ADD_LIBRARY( parametercorrelation 
  SHARED
  parametercorrelation.c
)

TARGET_LINK_LIBRARIES( parametercorrelation parametercorrelation tinkercellapi )

#ADD_SUBDIRECTORY( lpsolve )

INCLUDE_DIRECTORIES(
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/shared/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/colamd/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/bfp_LUSOL
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/bfp_LUSOL/LUSOL
)

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
)

TARGET_LINK_LIBRARIES( lpsolve lpsolve tinkercellapi )

#SET_TARGET_PROPERTIES(lpsolve PROPERTIES FLAGS "-DINLINE=static -O3 -DBFP_CALLMODEL=__stdcall -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -DExcludeNativeInverse")
#SET_TARGET_PROPERTIES(lpsolve PROPERTIES LINK_FLAGS "-DINLINE=static -O3 -DBFP_CALLMODEL=__stdcall -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine -DExcludeNativeInverse")

##
## CPack
##


IF ( WIN32 )
  INSTALL(TARGETS runssa DESTINATION plugins/c)
  INSTALL(TARGETS runcvode DESTINATION plugins/c)
  INSTALL(TARGETS runloops DESTINATION plugins/c)
  INSTALL(TARGETS loadFullBindingKinetics DESTINATION plugins/c)
#  INSTALL(TARGETS runbistablega DESTINATION plugins/c)
#  INSTALL(TARGETS runvaluesattime DESTINATION plugins/c)
#  INSTALL(TARGETS parametercorrelation DESTINATION plugins/c)
  INSTALL(TARGETS tabasco_like DESTINATION plugins/c)
  INSTALL(TARGETS lpsolve DESTINATION plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS runssa LIBRARY DESTINATION plugins/c)
  INSTALL(TARGETS runcvode LIBRARY DESTINATION plugins/c)
  INSTALL(TARGETS runloops LIBRARY DESTINATION plugins/c)
  INSTALL(TARGETS loadFullBindingKinetics LIBRARY DESTINATION plugins/c)
#  INSTALL(TARGETS runbistablega LIBRARY DESTINATION plugins/c)
#  INSTALL(TARGETS runvaluesattime LIBRARY DESTINATION plugins/c)
#  INSTALL(TARGETS parametercorrelation LIBRARY DESTINATION plugins/c)
  INSTALL(TARGETS tabasco_like LIBRARY DESTINATION plugins/c)
  INSTALL(TARGETS lpsolve LIBRARY DESTINATION plugins/c)
ENDIF( WIN32 )

FILE( GLOB C_SRC "*.c" "*.h" )

INSTALL( FILES ${C_SRC}
  DESTINATION c
  COMPONENT C_SOURCE
)

FILE( GLOB CVODE_HDRS ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/cvode/*.h )

INSTALL( FILES ${CVODE_HDRS}
  DESTINATION c/cvode
  COMPONENT CVODE
)

FILE( GLOB 
SUNDIALS_HDRS ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/sundials/*.h 
SUNDIALS_HDRS ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/sundials/*.h 
)

INSTALL( FILES ${SUNDIALS_HDRS}
  DESTINATION c/sundials
  COMPONENT CVODE
)

FILE( GLOB NVECTOR_HDRS ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/nvector/*.h )

INSTALL( FILES ${NVECTOR_HDRS}
  DESTINATION c/nvector
  COMPONENT CVODE
)

FILE( GLOB C_ICON_FILES icons/*.png icons/*.PNG )

INSTALL( FILES ${C_ICON_FILES}
  DESTINATION plugins/c
  COMPONENT plugins
)

ADD_SUBDIRECTORY( sbml_sim )

