/**
 */
#ifndef __cx_log_tag_source_h__
#define __cx_log_tag_source_h__

#include <cx/log/source.hpp>

namespace cx::log {

    class tag_source : public basic_source<tag_source> {
    public:
        tag_source( const char* name ) 
            :_name(name) {}
            
        virtual const char* name(void){
            return _name;
        }
    private:
        const char* _name;
    };

}

#endif