SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

ADD_SUBDIRECTORY(API)

ADD_LIBRARY( ssa
	STATIC
	mtrand.c
	ssa.c
	cells_ssa.c
)
 
IF ( WIN32 )
  INSTALL(TARGETS ssa DESTINATION c)
ELSE ( WIN32 )
  INSTALL(TARGETS ssa ARCHIVE DESTINATION c)
ENDIF( WIN32 )

IF( WIN32 AND NOT UNIX)
   OPTION( CMAKE_WIN32 "This is a Windows OS" ON )
ELSE( WIN32 AND NOT UNIX)
   OPTION( CMAKE_WIN32 "This is a Windows OS" OFF )
ENDIF( WIN32 AND NOT UNIX)

ADD_LIBRARY( runsteadystate 
  SHARED
  runsteadystate.c
)

TARGET_LINK_LIBRARIES( runsteadystate runsteadystate tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS runsteadystate DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS runsteadystate LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( runcvode 
  SHARED
  runcvode.c
)

TARGET_LINK_LIBRARIES( runcvode runcvode tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS runcvode DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS runcvode LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( runssa 
  SHARED
  runssa.c
)

TARGET_LINK_LIBRARIES( runssa runssa tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS runssa DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS runssa LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( runloops 
  SHARED
  runLoops.c
)

TARGET_LINK_LIBRARIES( runloops runloops tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS runloops DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS runloops LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( loadFullBindingKinetics 
  SHARED
  loadFullBindingKinetics.c
)

TARGET_LINK_LIBRARIES( loadFullBindingKinetics loadFullBindingKinetics tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS loadFullBindingKinetics DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS loadFullBindingKinetics LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( runbistablega 
  SHARED
  runbistablega.c
)

TARGET_LINK_LIBRARIES( runbistablega runbistablega tinkercellapi )

#IF ( WIN32 )
#  INSTALL(TARGETS runbistablega DESTINATION Plugins/c)
#ELSE ( WIN32 )
#  INSTALL(TARGETS runbistablega LIBRARY DESTINATION Plugins/c)
#ENDIF( WIN32 )

ADD_LIBRARY( runvaluesattime 
  SHARED
  runvaluesattime.c
)

TARGET_LINK_LIBRARIES( runvaluesattime runvaluesattime tinkercellapi )

#IF ( WIN32 )
#  INSTALL(TARGETS runvaluesattime DESTINATION Plugins/c)
#ELSE ( WIN32 )
#  INSTALL(TARGETS runvaluesattime LIBRARY DESTINATION Plugins/c)
#ENDIF( WIN32 )

ADD_LIBRARY( tabasco_like 
  SHARED
  tabasco_like.c
)

TARGET_LINK_LIBRARIES( tabasco_like tabasco_like tinkercellapi )

IF ( WIN32 )
  INSTALL(TARGETS tabasco_like DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS tabasco_like LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

ADD_LIBRARY( parametercorrelation 
  SHARED
  parametercorrelation.c
)

TARGET_LINK_LIBRARIES( parametercorrelation parametercorrelation tinkercellapi )

#IF ( WIN32 )
#  INSTALL(TARGETS parametercorrelation DESTINATION Plugins/c)
#ELSE ( WIN32 )
#  INSTALL(TARGETS parametercorrelation LIBRARY DESTINATION Plugins/c)
#ENDIF( WIN32 )

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

IF ( WIN32 )
  INSTALL(TARGETS lpsolve DESTINATION Plugins/c)
ELSE ( WIN32 )
  INSTALL(TARGETS lpsolve LIBRARY DESTINATION Plugins/c)
ENDIF( WIN32 )

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
    tinkercellapi
    ${PYTHON_LIBRARIES}
  )
  
  IF ( WIN32 )
    INSTALL(TARGETS runpy DESTINATION Plugins/c)
  ELSE ( WIN32 )
    INSTALL(TARGETS runpy LIBRARY DESTINATION Plugins/c)
  ENDIF( WIN32 )

  
ENDIF(PYTHONLIBS_FOUND)


##
## CPack
##

FILE( GLOB C_SRC "*.c" "*.h" )

INSTALL( FILES ${C_SRC}
  DESTINATION c
  COMPONENT C_SOURCE
)

FILE( GLOB API_SRC "API/*.h" "API/*.c")

INSTALL( FILES ${API_SRC}
  DESTINATION c/API
  COMPONENT C_API
)

FILE( GLOB API_PYTHON_SRC "API/Python/*.h" "API/Python/*.c")

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

FILE( GLOB C_ICON_FILES icons/*.png icons/*.PNG)

INSTALL( FILES ${C_ICON_FILES}
  DESTINATION Plugins/c
  COMPONENT Plugins
)

