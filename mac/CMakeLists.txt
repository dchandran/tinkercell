SET(QT_QTCORE QtCore.framework/Versions/4/QtCore)
SET(QT_QTGUI QtGui.framework/Versions/4/QtGui)
SET(QT_QTXML QtXml.framework/Versions/4/QtXml)
SET(QT_QTOPENGL QtOpenGL.framework/Versions/4/QtOpenGL)

CONFIGURE_FILE( do_name_change.sh
  ${TINKERCELL_BINARY_DIR}/bin/do_name_change.sh
  @ONLY
)
