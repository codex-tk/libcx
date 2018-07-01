/**
 */
#ifndef __cx_time_time_h__
#define __cx_time_time_h__

#include <cx/cxdefine.hpp>

namespace cx::time {

bool is_leap_year( const int year ) {
    return (( year % 4 == 0 ) && ( year % 100 != 0 )) ||  ( year % 400 == 0 );
}

int days_in_month( const int year, const int month ) {
    static const int days_in_month_tbl [2][13] = {
        { 0 , 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ,
        { 0 , 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ,
    };
    int tblidx = is_leap_year( year ) ? 1 : 0;
    return days_in_month_tbl[tblidx][month];
}

int64_t to_milliseconds( const std::chrono::system_clock::time_point& tp ) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}

std::chrono::system_clock::time_point from_milliseconds( int64_t ms ) {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t( 
        ms / 1000
    ) + std::chrono::milliseconds( ms % 1000 );
    return tp;
}


}

#endif