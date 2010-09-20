#FILE( GLOB LPSOLVE_SRC *.c )
#ADD_SUBDIRECTORY( colamd )
#ADD_SUBDIRECTORY( shared )
#ADD_SUBDIRECTORY( bfp )

ADD_DEFINITIONS(-DINLINE=static) 
IF (WIN32)
	ADD_DEFINITIONS(-DBFP_CALLMODEL=__stdcall)
ENDIF(WIN32)
ADD_DEFINITIONS(-DYY_NEVER_INTERACTIVE)
ADD_DEFINITIONS(-DPARSER_LP)
#ADD_DEFINITIONS(-DINVERSE_ACTIVE=INVERSE_LUSOL)
ADD_DEFINITIONS(-DRoleIsExternalInvEngine)
#ADD_DEFINITIONS(-DExcludeNativeInverse)

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/shared/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/colamd/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/bfp/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/bfp/bfp_LUSOL
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/bfp/bfp_LUSOL/LUSOL
)

SET( LPSOLVE_SRC
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_MDO.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/shared/commonlib.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/colamd/colamd.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/shared/mmio.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/shared/myblas.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/ini.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_rlp.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_crash.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/bfp/bfp_LUSOL/lp_LUSOL.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/bfp/bfp_LUSOL/LUSOL/lusol.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_Hash.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_lib.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_wlp.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_matrix.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_mipbb.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_MPS.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_params.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_presolve.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_price.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_pricePSE.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_report.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_scale.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_simplex.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_SOS.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/lp_utils.c
  ${TINKERCELL_SOURCE_DIR}/ExternalCode/lpsolve/yacc_read.c
)

ADD_LIBRARY( lp_solve
  STATIC
  ${LPSOLVE_SRC}
)

