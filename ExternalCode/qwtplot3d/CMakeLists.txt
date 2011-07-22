IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 5)

SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

SET( QWTPLOT3D_SRC
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_axis.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_color.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_coordsys.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_drawable.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_mousekeyboard.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_movements.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_lighting.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_colorlegend.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_plot.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_label.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_types.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_enrichment_std.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_autoscaler.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_io_reader.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_io.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_scale.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_gridmapping.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_parametricsurface.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_function.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_surfaceplot.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_gridplot.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_meshplot.cpp        
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_io_gl2ps.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/3rdparty/gl2ps/gl2ps.c
	)

SET( QWTPLOT3D_HDRS
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_color.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_global.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_types.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_axis.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_coordsys.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_drawable.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_helper.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_label.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_openglhelper.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_colorlegend.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_plot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_enrichment.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_enrichment_std.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_autoscaler.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_autoptr.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_io.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_io_reader.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_scale.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_portability.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_mapping.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_gridmapping.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_parametricsurface.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_function.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_surfaceplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_volumeplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_graphplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_multiplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/3rdparty/gl2ps/gl2ps.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_io_gl2ps.h
)

ADD_DEFINITIONS(-DQT_DLL -DQWT3D_DLL -DQWT3D_MAKEDLL)

QT4_WRAP_CPP( QWTPLOT3D_MOC ${QWTPLOT3D_HDRS} )

ADD_LIBRARY( qwtplot3d
#  STATIC
  SHARED
  ${QWTPLOT3D_SRC}
  ${QWTPLOT3D_MOC}
  )

TARGET_LINK_LIBRARIES( qwtplot3d
  qwtplot3d
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARY}
  )
  
IF (WIN32)
  INSTALL(TARGETS qwtplot3d DESTINATION bin/../)
ELSE(WIN32)
  INSTALL(TARGETS qwtplot3d LIBRARY DESTINATION bin/../)
ENDIF(WIN32)

ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 5)