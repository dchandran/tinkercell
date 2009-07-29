SET(LIBRARY_OUTPUT_PATH ${C_LIBRARY_OUTPUT_PATH})

ADD_LIBRARY( langevin 
  mtrand.c
  langevin.c
)

ADD_LIBRARY( ssa 
  mtrand.c
  ssa.c
)

ADD_LIBRARY( runsteadystate 
  SHARED
  runsteadystate.c 
)

ADD_LIBRARY( runcvode 
  SHARED
  runcvode.c 
)

ADD_LIBRARY( ga_bistable
  mtrand.c
  ga.c
  ga_bistable.c
)


ADD_LIBRARY( runssa 
  SHARED
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
  runbistablega.c 
)


ADD_LIBRARY( runvaluesattime 
  SHARED
  runvaluesattime.c
)

ADD_LIBRARY( tabasco_like 
  SHARED
  tabasco_like.c
)

ADD_LIBRARY( runpy 
  SHARED
  runpy.c 
)

TARGET_LINK_LIBRARIES( runpy
  runpy
  ${PYTHON_LIBRARIES}
)

