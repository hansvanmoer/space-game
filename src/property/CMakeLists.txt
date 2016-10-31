#
# Sub project to build the property file parser
#

find_package(BISON 3.0 REQUIRED)
find_package(FLEX 2.5 REQUIRED)

BISON_TARGET(PropertyParser PropertyParser.ypp ${CMAKE_CURRENT_BINARY_DIR}/PropertyParser.cpp)
FLEX_TARGET(PropertyLexer PropertyLexer.lpp  ${CMAKE_CURRENT_BINARY_DIR}/PropertyLexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(PropertyLexer PropertyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(property ${BISON_PropertyParser_OUTPUTS} ${FLEX_PropertyLexer_OUTPUTS} PropertyTask.cpp Property.cpp)