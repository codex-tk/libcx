#include "tests/gprintf.hpp"
#include <cx/io/async_layer.hpp>

TEST( async_layer , bs ) {
    cx::io::async_layer layer;
    cx::io::async_layer::handle_ptr hptr = layer.make_shared_handle();
}