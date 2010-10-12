SET(QT_QTCORE Versions/4/QtCore)
SET(QT_QTGUI Versions/4/QtGui)
SET(QT_QTXML Versions/4/QtXml)
SET(QT_QTOPENGL Versions/4/QtOpenGL)

CONFIGURE_FILE( do_name_change.sh
  ${TINKERCELL_BINARY_DIR}/bin/do_name_change.sh
  @ONLY
)
