#include <gtest/gtest.h>
#if defined(ENABLE_LIB_BOOST)
#include <boost/any.hpp>
namespace ns = boost;
#else
#include <any>
namespace ns = std;
#endif
#include "tests/gprintf.hpp"

namespace {

class any_value {
public:
    any_value( int val ) //noexcept( noexcept( std::declval< boost::any >())) 
    {
        _type = type::integer;
        _value = val;
    }
    any_value( const char* str ) {
        _type = type::string;
        _value = std::string(str);
    }
    any_value( std::initializer_list<any_value>&& l ) {
        _type = type::list;
        std::vector< any_value > values;
        for (const any_value& av : l) {
          values.emplace_back(av);
        }
        _value = values;
    }

    void print() const {
        switch( _type ){
            case type::integer:
                gprintf( "integer %d\n" , ns::any_cast<int>(_value));
                break;
            case type::string:
				gprintf( "string %s\n" , ns::any_cast<std::string>(_value).c_str());
                break;
            case type::list:
				gprintf( "List Begin %s\n" , _value.type().name() );
                {  
                    std::vector< any_value > l = ns::any_cast<std::vector< any_value >>(_value);
                    for ( const any_value& nr : l ) {
                        nr.print();
                    }
                }
				gprintf( "List End\n" );
                break;
        }
    }
private:
    enum class type {
        integer , 
        string , 
        list ,
    };
    type _type;
    ns::any _value;
};

void initialize_list_traversal( std::initializer_list<any_value>&& l  ) {
    for ( const any_value& nr : l ) {
        nr.print();
    }
}

}

TEST( multiple_types_initialize_list , usage ){
    initialize_list_traversal( { 0 , "str" , { 3 , "1" } , { { "1" , 2} }} );
}
