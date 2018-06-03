/**
 * @brief 
 * 
 * @file alloc_hook.hpp
 * @author ghtak
 * @date 2018-06-03
 */

#ifndef __cx_core_basic_allocator_h__
#define __cx_core_basic_allocator_h__
namespace cx::core {

template <typename T>
struct basic_allocator
{
    typedef T value_type;
    basic_allocator() noexcept {} //default ctor not required by STL
    template <typename U>
    basic_allocator(const basic_allocator<U> &) noexcept {}
    template <typename U>
    bool operator==(const basic_allocator<U> &) const noexcept
    {
        return true;
    }
    template <typename U>
    bool operator!=(const basic_allocator<U> &) const noexcept
    {
        return false;
    }
    T *allocate(const size_t n) const
    {
        if (n == 0)
        {
            return nullptr;
        }
        if (n > static_cast<size_t>(-1) / sizeof(T))  
        {  
            throw std::bad_array_new_length();  
        }  
        void* const pv = malloc(n * sizeof(T));  
        if (!pv) { throw std::bad_alloc(); }  
        return static_cast<T*>(pv);  
    }  
    void deallocate(T * const p, size_t) const noexcept  
    {   
        free(p);  
    }  

    template< typename... Args >
    void construct( T* p, Args&&... args ){
        new (p) T( std::forward<Args>(args)... ); 
    }
    void destroy( T* p ) {
        p->~T();
    }
};

template < typename T >
struct async_op_allocator{ 
    using value_type = T;
    using pointer = value_type *;
    async_op_allocator() noexcept {} //default ctor not required by STL
    template <typename U> async_op_allocator(const async_op_allocator<U> &) noexcept {}
    template <typename U> bool operator==(const async_op_allocator<U> &) const noexcept
    {
        return true;
    }
    template <typename U> bool operator!=(const async_op_allocator<U> &) const noexcept
    {
        return false;
    }
    pointer allocate(const size_t n) const {
        gprintf( "Alloc async_op_allocator OP");
        if (n == 0) {
            return nullptr;
        }
        if (n > static_cast<size_t>(-1) / sizeof(T)) {  
            throw std::bad_array_new_length();  
        }  
        void* const pv = malloc(n * sizeof(T));  
        if (!pv) { throw std::bad_alloc(); }  
        return static_cast<T*>(pv);  
    }  
    void deallocate(pointer const p, size_t) const noexcept {   
        gprintf( "Free async_op_allocator OP");
        free(p);  
    }
    template< typename... Args > void construct( pointer p, Args&&... args ){
        new (p) value_type( std::forward<Args>(args)... ); 
    }
    void destroy( pointer p ) {
        p->~value_type();
    }
};


}
#endif