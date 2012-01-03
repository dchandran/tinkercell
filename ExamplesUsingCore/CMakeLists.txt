INCLUDE_DIRECTORIES( BEFORE ${CMAKE_CURRENT_SOURCE_DIR} )

#create SimpleMainProgram and link it to the TinkerCellCore library
ADD_EXECUTABLE( SimpleMainProgram SimpleMainProgram.cpp )
TARGET_LINK_LIBRARIES( SimpleMainProgram TinkerCellCore )

#this is required for all header files that defines a GUI class
QT4_WRAP_CPP( MyFirstExtension_moc MyFirstExtension.h )
#create MyFirstExtension and link it to the TinkerCellCore library  
ADD_EXECUTABLE( MyFirstExtension MyFirstExtension.cpp ${MyFirstExtension_moc} )
TARGET_LINK_LIBRARIES( MyFirstExtension TinkerCellCore )

#these are two other example program
ADD_SUBDIRECTORY(SimpleDesigner)
ADD_SUBDIRECTORY(MultiCell)
ADD_SUBDIRECTORY(ExtensionUsingOntologies)

