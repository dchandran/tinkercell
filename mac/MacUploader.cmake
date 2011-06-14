SET(QT_QTCORE Versions/4/QtCore)
SET(QT_QTGUI Versions/4/QtGui)
SET(QT_QTXML Versions/4/QtXml)
SET(QT_QTOPENGL Versions/4/QtOpenGL)

CONFIGURE_FILE( 
  ${TINKERCELL_SOURCE_DIR}/mac/CreateApp.sh.in
  ${TINKERCELL_BINARY_DIR}/mac/CreateApp.sh
  @ONLY
)

MESSAGE(STATUS "Source ${TINKERCELL_BINARY_DIR}/mac/CreateApp.sh to build the program, the installer, and upload it to the sourceforge site")
