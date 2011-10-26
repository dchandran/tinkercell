include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( simplemain simplemain.cpp)
target_link_libraries( simplemain TinkerCellCore )

qt4_wrap_cpp( testPlugin1_moc SamplePlugin1.h )
add_executable( testPlugin1 SamplePlugin1.cpp ${testPlugin1_moc} )
target_link_libraries( testPlugin1 TinkerCellCore )

qt4_wrap_cpp( testPlugin2_moc SamplePlugin2.h )
add_executable( testPlugin2 SamplePlugin2.cpp ${testPlugin2_moc} )
target_link_libraries( testPlugin2 TinkerCellCore )

