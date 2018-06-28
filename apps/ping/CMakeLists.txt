project( ping )

include_directories( ${CMAKE_SOURCE_DIR} )

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_SOURCE_DIR}/apps/ping" "*.cpp" )

add_executable( ping ${SRCS} )

find_package( ZLIB )
if ( ZLIB_FOUND )
    target_link_libraries( ping ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

if(Boost_FOUND)
    target_link_libraries( ping ${Boost_LIBRARIES})
endif()

target_link_libraries( ping cx )