FILE(GLOB BOOST_REGEX_SRC regex/src/*.cpp)

IF (WIN32 AND NOT MINGW)
	IF(CMAKE_BUILG_TYPE STREQUAL "Debug")
		SET( boost_library_name libboost_regex-vc100-mt-1_48)
	ELSEIF(CMAKE_BUILG_TYPE STREQUAL "Debug")
		SET( boost_library_name libboost_regex-vc100-mt-gd-1_48)
	ENDIF(CMAKE_BUILG_TYPE STREQUAL "Debug")
	MESSAGE("if you are building in Debug mode, please set the CMAKE_BUILD_TYPE to Debug in the CMake GUI. This is a Visual Studio issue when using Boost.")
ELSE (WIN32 AND NOT MINGW)
	SET( boost_library_name boost_regex)
ENDIF (WIN32 AND NOT MINGW)

ADD_LIBRARY ( ${boost_library_name}
	#   SHARED
	  STATIC
	  ${BOOST_REGEX_SRC}
	)

