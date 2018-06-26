/**
 * @brief buffer
 * 
 * @file buffer.hpp
 * @author ghtak
 * @date 2018-05-12
 */
#include <cx/cxdefine.hpp>

namespace cx {

namespace internal{

    class basic_buffer_allocator {
    public:
        static void free(void* ptr){
            std::free(ptr);
        }
        static void* alloc(const std::size_t size){
            return std::malloc(size);
        }
    };

    /**
     * @brief 
     * 
     * @tparam AllocatorType 
     */
    template < typename T , typename AllocatorType >
    class control_block{
    public:
        control_block(const int32_t size) 
            : _ref_count(1) , _size(size){}

        T* base(void) const noexcept { return static_cast<T>(this+1);}
        int32_t size(void) const noexcept { return _size; }

        int32_t use_count(void) const noexcept {return _ref_count.load();}
        int32_t add_ref(void) noexcept { return _ref_count.fetch_add(); }
        int32_t release(void){
            int32_t cnt = _ref_count.fetch_dec(); 
            if(cnt==1){
                AllocatorType::free(this);
            }
            return cnt;
        }
    private:
        std::atomic<int32_t> _ref_count;
        int32_t _size;
    };
}

    /**
     * @brief 
     * 
     * @tparam  
     * @tparam Allocator 
     */
    template < typename Type , typename AllocatorType = internal::basic_buffer_allocator >
    class basic_buffer {
    public:
        basic_buffer(void)
            :_block(nullptr),_rd_pos(0),_wr_pos(0){}
    private:
        internal::control_block<Type,AllocatorType>* _block;
        int32_t _rd_pos;
        int32_t _wr_pos;
    };
}