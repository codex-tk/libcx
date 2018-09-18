project(cx.io)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../base/include )

file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp" )
file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

list(FILTER SRCS EXCLUDE REGEX "test/*")
list(FILTER SRCS EXCLUDE REGEX "sample/*")

MESSAGE( STATUS "cx.io SRCS " ${SRCS} )
add_library( cx.io STATIC ${HEADERS} ${SRCS} )
#add_library( cx.io.shared SHARED ${HEADERS} ${SRCS} )

target_link_libraries( cx.io cx.base )