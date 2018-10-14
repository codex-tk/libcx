/**
 * @file basic_buf.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2018-10-15
 *
 * @copyright Copyright (c) 2018
 *
 */

#ifndef __cx_base_basic_buf_h__
#define __cx_base_basic_buf_h__

#include <cx/base/defines.hpp>

namespace cx {

template <typename ElementType, typename BlockType,
          typename SizeType = std::ptrdiff_t>
class basic_buf {
public:
    using element_type = ElementType;
    using pointer_type = element_type *;
    using block_type = BlockType;
    using size_type = SizeType;

    basic_buf(void) noexcept : _rd_ptr(nullptr), _wr_ptr(nullptr) {}

    void reserve(const size_type n) {
        if (n <= 0)
            return;

        auto size = size();
        std::memmove(base(), _rd_ptr, size);

        _rd_ptr = base();
        _wr_ptr = base() + size;

        if (space() >= n)
            return;

        _block.reserve(n, size);

        _rd_ptr = base();
        _wr_ptr = base() + size;
    }

    pointer_type base(void) { return _block.base(); }

    pointer_type rd_ptr(void) { return _rd_ptr; }
    pointer_type wr_ptr(void) { return _wr_ptr; }

    size_type rd_ptr(const size_type n) {
        size_type offset = (n >= 0 ? std::min(n, size())
                                   : std::max(n, (_rd_ptr - base()) * -1));
        _rd_ptr += offset;
        return offset;
    }

    size_type wr_ptr(const size_type n) {
        size_type offset =
            (n >= 0 ? std::min(n, space()) : std::max(n, size() * -1));
        _wr_ptr += offset;
        return offset;
    }

    /**
     * @brief
     *
     * @return size_type
     */
    size_type size(void) { return _wr_ptr - _rd_ptr; }

    /**
     * @brief
     *
     * @return size_type
     */
    size_type space(void) { return _block.capacity() - (_wr_ptr - base()); }

    /**
     * @brief
     *
     * @return size_type
     */
    size_type capacity(void) { return _block.capacity(); }

    void clear(void){
        _rd_ptr =_wr_ptr = base();
    }
private:
    pointer_type _rd_ptr;
    pointer_type _wr_ptr;
    block_type _block;
};

} // namespace cx

#endif