FILE(GLOB BOOST_REGEX_SRC regex/src/*.cpp)

ADD_LIBRARY ( boost_regex
#   SHARED
  STATIC
  ${BOOST_REGEX_SRC}
)

