cmake_minimum_required(VERSION 3.0.0)
project(libcx VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  # vs option change
  add_compile_options("/wd4819")
  add_compile_options("/wd4127")

  # for googletest
  foreach (flag_var
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
    string(REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
  endforeach()

  string(REGEX REPLACE "/O2" "Od"
    CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG}")

  add_definitions( /std:c++latest )
  add_definitions( -DGTEST_LANG_CXX11=1 )
else()
  #-std=c++1z 
  add_definitions( -Wall -std=c++17 -o2 -g -ggdb -fconcepts )
endif()

find_package( ZLIB )
if ( ZLIB_FOUND )
  include_directories( ${ZLIB_INCLUDE_DIRS} )
  #target_link_libraries( test ${ZLIB_LIBRARIES} )
  add_definitions( -DENABLE_LIB_Z )
endif( ZLIB_FOUND )

find_package(Boost 1.60.0 COMPONENTS filesystem )
if(Boost_FOUND)
	message(STATUS  ${Boost_INCLUDE_DIRS} )	
    set(Boost_USE_STATIC_LIBS        ON) # only find static libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    OFF)
    include_directories(${Boost_INCLUDE_DIRS})
    #target_link_libraries( test ${Boost_LIBRARIES})
    add_definitions( -DENABLE_LIB_BOOST )
endif()

add_subdirectory( googletest )
add_subdirectory( cx )
add_subdirectory( tests )
add_subdirectory( apps/echo )
add_subdirectory( apps/ping )