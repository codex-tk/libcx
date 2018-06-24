project( tests )

include_directories( ${CMAKE_SOURCE_DIR} )

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_SOURCE_DIR}/echo" "*.cpp" )

add_executable( echo ${SRCS} )

find_package( ZLIB )
if ( ZLIB_FOUND )
    target_link_libraries( echo ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

if(Boost_FOUND)
    target_link_libraries( echo ${Boost_LIBRARIES})
endif()

target_link_libraries( echo cx )