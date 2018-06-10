project( tests )

include_directories( ${CMAKE_SOURCE_DIR} )

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_SOURCE_DIR}/tests" "*.cpp" )

add_executable( tests ${SRCS} )
#add_executable( tests main.cpp services.cpp )

find_package( ZLIB )
if ( ZLIB_FOUND )
    target_link_libraries( tests ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

if(Boost_FOUND)
    target_link_libraries( tests ${Boost_LIBRARIES})
endif()

target_link_libraries( tests cx gtest )

#enable_testing()
#add_test(tests tests)
#add_custom_command(TARGET tests
    #POST_BUILD
    #COMMAND ctest -C $<CONFIGURATION> --verbose
    #--output-on-failure
    #)
