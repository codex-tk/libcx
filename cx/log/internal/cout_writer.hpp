/**
 */
#ifndef __cx_log_cout_writer_h__
#define __cx_log_cout_writer_h__

#include <cx/log/writer.hpp>

namespace cx::log {

    class cout_writer : public cx::log::writer {
    public:
        virtual void operator()( const cx::log::record& , const cx::basic_buffer<char>& formatted ) {
            std::cout << formatted.rdptr() << std::endl;
        }
    public:
        static std::shared_ptr<writer> instance(void){
            static std::shared_ptr<writer> writer_ptr = std::make_shared<cout_writer>();
            return writer_ptr;
        }
    };
}

#endif