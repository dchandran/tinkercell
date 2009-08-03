SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

SUBDIRS( cvode
#  nvec_par
  nvec_ser
  sundials
  )

ADD_LIBRARY( odesim
 STATIC
 ${CVODE_SRC}
 ${SUNDIALS_SRC}
 ${NVECSER_SRC}
 ${TINKERCELL_SOURCE_DIR}/c/cvodesim.c
 )

#TARGET_LINK_LIBRARIES( odesim
#  odesim
#  langevin
#  ssa
#  ga_bistable
#  runcvode
#  runvaluesattime
#  runsteadystate
#)