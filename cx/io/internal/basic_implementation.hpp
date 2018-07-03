/**
 */
#ifndef __cx_io_detail_basic_implementation_h__
#define __cx_io_detail_basic_implementation_h__

#include <thread>
#include <cx/error.hpp>

namespace cx::io::internal{

    class basic_implementation {
    protected:
        ~basic_implementation(void){}
    public:
		basic_implementation(void) noexcept
			: _thread_id(std::this_thread::get_id())
			, _in_loop(false) {}

        void begin_loop(void){
            _thread_id = std::this_thread::get_id();
            _in_loop = true;
        }

        void end_loop(void){
            _in_loop = false;
        }

        bool in_loop(void){
            if ( _thread_id == std::this_thread::get_id() )
                return _in_loop;
            return false;
        }

        std::error_code last_error( const std::error_code& ec ) {
            auto old = _error_code;
            _error_code = ec;
            return old;
        }

        std::error_code last_error(void) {
            return _error_code;
        }

        struct scoped_loop{
            scoped_loop( basic_implementation& impl )
                :_impl(impl)
            {
                _impl.begin_loop();   
            }

            ~scoped_loop(void){
                _impl.end_loop();
            }
            basic_implementation& _impl;   
        };
    private:
        std::thread::id _thread_id;
        bool _in_loop;
        std::error_code _error_code;
    };
}

#endif