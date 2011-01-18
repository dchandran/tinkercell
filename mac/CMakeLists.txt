SET(QT_QTCORE Versions/4/QtCore)
SET(QT_QTGUI Versions/4/QtGui)
SET(QT_QTXML Versions/4/QtXml)
SET(QT_QTOPENGL Versions/4/QtOpenGL)

CONFIGURE_FILE( 
	create_bundle.sh.in
  ${TINKERCELL_BINARY_DIR}/create_bundle.sh
  @ONLY
)

MESSAGE("run ${TINKERCELL_BINARY_DIR}/create_bundle.sh to make the app")