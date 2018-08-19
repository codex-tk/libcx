project(cx.base)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp" )
file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

list(FILTER SRCS EXCLUDE REGEX "test/*")

MESSAGE( STATUS "SRCS " ${SRCS} )

add_library( cx.base STATIC ${HEADERS} ${SRCS} )
add_library( cx.base.shared SHARED ${HEADERS} ${SRCS} )
