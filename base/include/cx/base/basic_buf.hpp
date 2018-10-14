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

    basic_buf(void) noexcept : _rd_pos(0), _wr_pos(0) {}

    basic_buf(pointer_type ptr, const size_type sz)
        : _block(ptr, sz), _rd_pos(0), _wr_pos(0) {}

    basic_buf(const size_type sz) : _block(sz), _rd_pos(0), _wr_pos(0) {}

    basic_buf(const basic_buf &rhs)
        : _block(rhs._block), _rd_pos(rhs._rd_pos), _wr_pos(rhs._wr_pos) {}

    basic_buf(basic_buf &&rhs) noexcept
        : _block(std::move(rhs._block)), _rd_pos(std::move(rhs._rd_pos)),
          _wr_pos(std::move(rhs._wr_pos)) {
        rhs._block = nullptr;
        rhs._rd_pos = rhs._wr_pos = 0;
    }

    basic_buf &operator=(const basic_buf &rhs) {
        basic_buf nb(rhs);
        swap(nb);
        return *this;
    }

    basic_buf &operator=(basic_buf &&rhs) noexcept {
        basic_buf nb(std::forward<basic_buf>(rhs));
        swap(nb);
        return *this;
    }

    void swap(basic_buf &rhs) {
        _block.swap(rhs._block);
        std::swap(_rd_pos, rhs._rd_pos);
        std::swap(_wr_pos, rhs._wr_pos);
    }

    void reserve(const size_type n) {
        if (n <= 0)
            return;

        if (space() >= n)
            return;

        _block.reserve(size() + n);

        auto size = size();
        std::memmove(base(), rd_ptr(), size);

        _rd_pos = 0;
        _wr_pos = size;
    }

    pointer_type base(void) { return static_cast<pointer_type>(_block.base()); }

    pointer_type rd_ptr(void) { return base() + _rd_pos; }
    pointer_type wr_ptr(void) { return base() + _wr_pos; }

    size_type rd_ptr(const size_type n) {
        size_type offset =
            (n >= 0 ? std::min(n, size()) : std::max(n, _rd_pos * -1));
        _rd_pos += offset;
        return offset;
    }

    size_type wr_ptr(const size_type n) {
        size_type offset =
            (n >= 0 ? std::min(n, space()) : std::max(n, size() * -1));
        _wr_pos += offset;
        return offset;
    }

    /**
     * @brief
     *
     * @return size_type
     */
    size_type size(void) { return _wr_pos - _rd_pos; }

    /**
     * @brief
     *
     * @return size_type
     */
    size_type space(void) { return capacity() - _wr_pos; }

    /**
     * @brief
     *
     * @return size_type
     */
    size_type capacity(void) { return _block.capacity(); }

    void clear(void) { _rd_pos = _wr_pos = 0; }

private:
    size_type _rd_pos;
    size_type _wr_pos;
    block_type _block;
};

template <typename ElementType, typename AllocatorType,
          typename SizeType = std::ptrdiff_t>
class rc_block {
public:
    using element_type = ElementType;
    using size_type = SizeType;

    rc_block(void) : _ctrl_block(nullptr) {}

    rc_block(const rc_block &rhs) : _ctrl_block(rhs._ctrl_block) {
        if (_ctrl_block)
            _ctrl_block->ref_count.fetch_add(1);
    }

    rc_block(rc_block &&rhs) : _ctrl_block(rhs._ctrl_block) {
        rhs._ctrl_block = nullptr;
    }

    rc_block(const size_type n) { reserve(n); }

    ~rc_block(void) {
        if (_ctrl_block) {
            _ctrl_block->release();
        }
        _ctrl_block = nullptr;
    }

    void *base(void) { return _ctrl_block ? _ctrl_block->base() : nullptr; }
    size_type capacity(void) { return _ctrl_block ? _ctrl_block->size : 0; }

    void swap(rc_block &rhs) { swap(_ctrl_block, _rhs.ctrl_block); }
    void reserve(const size_type n) {
        size_type buffer_size = sizeof(element_type) * n;

        if (_ctrl_block && _ctrl_block->ref_count.load() == 1 &&
            _ctrl_block->size >= n) {
            return;
        }

        ctrl_block *pold = _ctrl_block;

        _ctrl_block = reinterpret_cast<ctrl_block *>(
            AllocatorType::alloc(sizeof(ctrl_block) + buffer_size));
        new (_ctrl_block) ctrl_block(n);
        if (pold) {
            memcpy(_ctrl_block->base(), pold->base(), buffer_size);
            pold->release();
        }
    }

private:
    struct ctrl_block {
        ctrl_block(const size_type n) : ref_count(1), size(n) {}
        void *base(void) { return this + 1; }
        void release(void) {
            int cnt = _ctrl_block->ref_count.fetch_sub(1);
            assert(cnt > 0);
            if (cnt == 1) {
                AllocatorType::free(_ctrl_block);
            }
        }
        std::atomic<int> ref_count;
        size_type size;
    };
    ctrl_block *_ctrl_block;
};

template <typename ElementType, typename SizeType = std::ptrdiff_t>
class fixed_block {
public:
    using element_type = ElementType;
    using size_type = SizeType;
    using pointer_type = element_type *;

    fixed_block(pointer_type *ptr, size_type sz)
        : _ptr(ptr), _size(sz) {}

    void *base(void) { return _ptr; }
    size_type capacity(void) { return _size; }

    void reserve(const size_type n) {
        if (_size < n ){
            throw std::bad_alloc();
        }
    }

private:
    pointer_type _ptr;
    size_type _size;
};

} // namespace cx

#endif