/**
 * @brief 
 * 
 * @file handler.hpp
 * @author ghtak
 * @date 2018-06-03
 */

#include <memory>
#include <cx/core/allocators.hpp>

namespace cx::core{
    /*!
    */
    template < typename Signature >
    class callable;

    /*!
    */
    template < typename R , typename ... Args >
    class callable < R ( Args... ) >{
    public:
        callable( void ) noexcept {
        }
        virtual ~callable( void ) noexcept {
        }
        virtual R operator()( Args&& ... arg ) = 0;
    };

    template < typename HandlerT , typename R , typename ... Args >
    class callable0 : public callable< R ( Args... ) > {
    public:
        callable0( HandlerT&& h ) 
            : _handler( std::forward< HandlerT >(h))
        {
        }

        virtual ~callable0( void ) override {
        }

        virtual R operator()( Args&& ... args ) override {
            return _handler( std::forward< Args >(args)...);
        }
    private:
        HandlerT _handler;
    };

    template < typename Signature >
    class function;

    template < typename R , typename ... Args >
    class function < R ( Args... ) >{
    public:
        template < typename T >
        using callable_impl = cx::core::callable0< T , R , Args ... >;
        /*! default construct */
        function( void );
        /*! copy construct */
        function( const function& rhs );
        /*! 
            function< R (Args...) > lhs( ... );
            if ( not exist  function( function& rhs ) ) 
                // match -> template < typename HandlerT > function( HandlerT&& handler );
                function< R (Args...) > rhs(lhs); 
        */
        function( function& rhs );
        /*! move construct */
        function( function&& rhs );

        /*! ctor from callable */
        template < typename HandlerT >
        function( HandlerT&& handler );
        
        /*! typically assign operation */
        function& operator=( const function& rhs );

        /*! 
            function< R (Args...) > lhs( ... );
            function< R (Args...) > rhs( ... );
            if ( not exist  operator=( function& rhs ) ) 
                // match -> template < typename HandlerT > function& operator=( HandlerT&& handler );
                lhs = rhs; 
        */
        function& operator=( function& rhs );

        /*! move assign */
        function& operator=( function&& rhs );

        /*! assign from callable */
        template < typename HandlerT >
        function& operator=( HandlerT&& handler );
        
        /*! */
        R operator()( Args&& ... args ) const ;

        /*! */
        void swap( function& rhs );
    private:
        std::shared_ptr< cx::core::callable< R ( Args... ) >> _callable;
    };

#define CX_FUNCTION_TEST
#if defined( CX_FUNCTION_TEST )
    namespace test {
        constexpr int function_ctor = 0;
        constexpr int function_copy_ctor = 1;
        constexpr int function_copy_ctor_l = 2;
        constexpr int function_move_ctor = 3;
        constexpr int function_callable_ctor = 4;

        constexpr int function_assign = 5;
        constexpr int function_assign_l = 6;
        constexpr int function_move_assign = 7;
        constexpr int function_callable_assign = 8;

        template < typename TypeT >
        class method_calls{
        public:
            method_calls( void ) = default;
            ~method_calls( void ) = default;
            void call( int ptr ) {
                if ( _calls.find(ptr) == _calls.end() )
                    _calls[ptr] = 0;
                _calls[ptr] = _calls[ptr] + 1;
            }
            int call_counts( int ptr ) { return _calls.find(ptr) == _calls.end() ? 0 : _calls[ptr]; }
            void reset( void ) { _calls.clear(); }
            static method_calls& instance() {
                static method_calls ins;
                return ins;
            }
        private:
            std::map< int , int > _calls;
        };
    }
#define CALL_RECORD( arg ) cx::core::test::method_calls< function >::instance().call( arg );
#else
#define CALL_RECORD( arg )
#endif

    

    template < typename R , typename ... Args >
    function < R ( Args... ) >::function( void )
    {
        CALL_RECORD( test::function_ctor );
    }

    template < typename R , typename ... Args >
    function < R ( Args... ) >::function( const function& rhs )
        : _callable( rhs._callable )
    {
        CALL_RECORD( test::function_copy_ctor );
    }

    template < typename R , typename ... Args >
    function < R ( Args... ) >::function( function& rhs )
        : _callable( rhs._callable )
    {
        CALL_RECORD( test::function_copy_ctor_l );
    }

    template < typename R , typename ... Args >
    function < R ( Args... ) >::function( function&& rhs )
        : _callable( std::move(rhs._callable) )
    {
        CALL_RECORD( test::function_move_ctor );
    }



    template < typename R , typename ... Args >
    template < typename HandlerT >
    function < R ( Args... ) >::function( HandlerT&& handler )
        : _callable( std::allocate_shared< callable_impl<HandlerT> >( 
                            cx::core::basic_allocator<callable_impl<HandlerT>>() ,
                            std::forward< HandlerT>(handler))) 
    {
        CALL_RECORD( test::function_callable_ctor );
    }

    template < typename R , typename ... Args >
    function < R ( Args... ) >& function < R ( Args... ) >::operator=( const function& rhs ){
        CALL_RECORD( test::function_assign );
        _callable = rhs._callable;
        return *this;
    }

    template < typename R , typename ... Args >
    function < R ( Args... ) >& function < R ( Args... ) >::operator=( function& rhs ){
        CALL_RECORD( test::function_assign_l );
        _callable = rhs._callable;
        return *this;
    }

    template < typename R , typename ... Args >
    function < R ( Args... ) >& function < R ( Args... ) >::operator=( function&& rhs ){
        CALL_RECORD( test::function_move_assign );
        _callable = std::move(rhs._callable);
        return *this;
    }

    template < typename R , typename ... Args >
    template < typename HandlerT >
    function < R ( Args... ) >& function < R ( Args... ) >::operator=( HandlerT&& handler ){
        CALL_RECORD( test::function_callable_assign );
        _callable = std::allocate_shared< callable_impl<HandlerT> >( 
                            cx::core::basic_allocator<callable_impl<HandlerT> >() ,
                            std::forward< HandlerT>(handler));
        return *this;
    }

    template < typename R , typename ... Args >
    R function < R ( Args... ) >::operator()( Args&& ... args ) const {
        if ( _callable ) {
            return (*_callable)( std::forward< Args >( args )... );
        }
        return R();
    }

    template < typename R , typename ... Args >
    void function < R ( Args... ) >::swap( function& rhs ){
        _callable.swap(rhs._callable);
    }
}