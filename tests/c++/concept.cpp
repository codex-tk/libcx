#include <gtest/gtest.h>
#ifdef __GNUG__
template<typename T>
concept bool Addable = requires(T a, T b) { a +  b; };

template < typename T > requires Addable<T>
auto add( T a , T b ) -> decltype( a + b ) {
  return a + b;
}

struct Foo{};
struct Bar{ Bar operator+( const Bar& rhs ) { return Bar{}; } };

TEST( concept , sample0 ) {
  std::cout << add( 0 , 1 ) << std::endl;
  add( Bar{} , Bar{} );
// compile error
// add( Foo{} , Foo{} );
}
#endif