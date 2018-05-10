project( cx )
include_directories( ${CMAKE_SOURCE_DIR} )

file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_SOURCE_DIR}/cx" "*.hpp" )
file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_SOURCE_DIR}/cx" "*.cpp" )

add_library( cx STATIC ${HEADERS} ${SRCS} )
#add_library( cx SHARED ${HEADERS} ${SRCS} )