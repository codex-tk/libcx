project(cx.io.test)

include_directories( ${CMAKE_SOURCE_DIR}/base/include )
include_directories( ${CMAKE_SOURCE_DIR}/io/include )

file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

add_executable(cx.io.echo ${SRCS})

target_link_libraries( cx.io.echo cx.base cx.io )