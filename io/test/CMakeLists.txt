project(cx.io.test)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../base/include )

file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

add_executable(cx.io.test ${SRCS})

find_package( ZLIB )
if ( ZLIB_FOUND )
    target_link_libraries( cx.io.test ${ZLIB_LIBRARIES})
endif( ZLIB_FOUND )

if(Boost_FOUND)
    target_link_libraries( cx.io.test ${Boost_LIBRARIES})
endif()

target_link_libraries( cx.io.test cx.base cx.io gtest )

enable_testing()
add_test(cx.io.test cx.io.test)
add_custom_command(TARGET cx.io.test  POST_BUILD  COMMAND ctest -C $<CONFIGURATION> --verbose --output-on-failure )
