FILE(GLOB BOOST_REGEX_SRC regex/src/*.cpp)

IF (WIN32 AND NOT MINGW)
	MESSAGE("if you are building in Debug mode, please set the CMAKE_BUILD_TYPE to Debug in the CMake GUI. This is a Visual Studio issue when using Boost.")
	SET( boost_library_name libboost_regex-vc100-mt-1_48)
	IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		SET( boost_library_name libboost_regex-vc100-mt-gd-1_48)
	ENDIF()
ELSE (WIN32 AND NOT MINGW)
	SET( boost_library_name boost_regex)
ENDIF (WIN32 AND NOT MINGW)

ADD_LIBRARY ( ${boost_library_name}
	#   SHARED
	  STATIC
	  ${BOOST_REGEX_SRC}
	)

