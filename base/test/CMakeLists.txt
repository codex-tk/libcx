project(cx.base.test)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../include )

file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

add_executable(cx.base.test ${SRCS} )

find_package( ZLIB )
if ( ZLIB_FOUND )
    target_link_libraries( cx.base.test ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

if(Boost_FOUND)
    target_link_libraries( cx.base.test ${Boost_LIBRARIES})
endif()

target_link_libraries( cx.base.test cx.base gtest )

