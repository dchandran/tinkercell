QT4_WRAP_CPP( DefaultPluginsMenu_MOC DefaultPluginsMenu.h )

QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )

ADD_EXECUTABLE( tinkercell 
  Main.cpp
  ${DefaultPluginsMenu_MOC}
  DefaultPluginsMenu.cpp
  ${TINKERCELL_QRC}
)

TARGET_LINK_LIBRARIES( tinkercell
  TinkerCellCore
  )
