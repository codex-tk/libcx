/**
 * @brief
 *
 * @file basic_buffer.hpp
 * @author ghtak
 * @date 2018-08-26
 */

#ifndef __cx_base_basic_buffer_h__
#define __cx_base_basic_buffer_h__

#include <cx/base/defines.hpp>
namespace cx::internal{

	class basic_buffer_allocator {
	public:
		static void free(void* ptr) {
			std::free(ptr);
		}
		static void* alloc(const std::size_t size) {
			return std::malloc(size);
		}
	};

	/**
	 * @brief
	 *
	 * @tparam AllocatorType
	 */
	template < typename T, typename AllocatorType, typename IntType >
	class block {
	public:
		using value_type = T;
		using int_type = IntType;
		block(const int_type size)
			: _ref_count(1), _size(size) {}

		value_type* base(void) const noexcept { 
			return const_cast<value_type*>(
				reinterpret_cast<const value_type*>(this + 1)); 
		}
		int_type size(void) const noexcept { return _size; }

		int use_count(void) const noexcept { return _ref_count.load(); }
		int add_ref(void) noexcept { return _ref_count.fetch_add(1); }
		int release(void) {
			int cnt = _ref_count.fetch_sub(1);
			assert(cnt > 0);
			if (cnt == 1) {
				AllocatorType::free(this);
			}
			return cnt;
		}
	private:
		std::atomic<int> _ref_count;
		int_type _size;
	};
}

namespace cx {
	/**
	 * @brief
	 *
	 * @tparam T
	 * @tparam AllocatorType
	 */
	template < typename T, typename AllocatorType = internal::basic_buffer_allocator, typename IntType = std::int32_t>
	class basic_buffer {
	public:
		using value_type = T;
		using int_type = IntType;
		using block_type = internal::block<value_type, AllocatorType, int_type>;
		basic_buffer(void) noexcept
			:_block(nullptr), _rd_pos(0), _wr_pos(0) {}

		basic_buffer(const int_type sz)
			: basic_buffer()
		{
			_block = reinterpret_cast<block_type*>(
				AllocatorType::alloc(sizeof(block_type) + (sizeof(value_type) * sz)
				));
			_block = new (_block) block_type(sz);
		}

		basic_buffer(const basic_buffer& rhs)
			: _block(rhs._block), _rd_pos(rhs._rd_pos), _wr_pos(rhs._wr_pos) {
			if (_block)
				_block->add_ref();
		}

		basic_buffer(basic_buffer&& rhs) noexcept
			: _block(std::move(rhs._block))
			, _rd_pos(std::move(rhs._rd_pos))
			, _wr_pos(std::move(rhs._wr_pos)) {
			rhs._block = nullptr;
			rhs._rd_pos = rhs._wr_pos = 0;
		}

		basic_buffer& operator=(const basic_buffer& rhs) {
			basic_buffer nb(rhs);
			swap(nb);
			return *this;
		}

		basic_buffer& operator=(basic_buffer&& rhs) noexcept {
			basic_buffer nb(std::forward<basic_buffer>(rhs));
			swap(nb);
			return *this;
		}

		~basic_buffer(void) {
			if (_block)
				_block->release();
		}

		block_type* block(void) {
			return _block;
		}

		value_type* base(void) { return _block ? _block->base() : nullptr; }

		int_type size(void) { return _block ? _block->size() : 0; }

		value_type* rdptr(void) const { return _block ? _block->base() + _rd_pos : nullptr; }

		int_type rdptr(int_type n) {
			int_type offset = (n >= 0 ?
				std::min(n, rdsize())
				: std::max(n, _rd_pos * -1));
			_rd_pos += offset;
			return offset;
		}

		int_type rdsize(void) const { return _wr_pos - _rd_pos; }

		value_type* wrptr(void) { return _block ? _block->base() + _wr_pos : nullptr; }

		int_type wrptr(int_type n) {
			int_type offset = (n >= 0 ?
				std::min(n, wrsize())
				: std::max(n, rdsize() * -1));
			_wr_pos += offset;
			return offset;
		}

		int_type wrsize(void) { return size() - _wr_pos; }

		void reserve(const int_type sz) {
			if (sz < 0)
				return;

			if (wrsize() >= sz)
				return;

			if ((size() - rdsize() >= sz) &&
				_block && _block->use_count() == 1) {
				std::memmove(base(), rdptr(), rdsize());
				_wr_pos = rdsize();
				_rd_pos = 0;
				return;
			}
			basic_buffer nb(rdsize() + sz);
			memcpy(nb.wrptr(), rdptr(), rdsize());
			nb.wrptr(rdsize());
			swap(nb);
		}

		void swap(basic_buffer& rhs) {
			std::swap(_block, rhs._block);
			std::swap(_rd_pos, rhs._rd_pos);
			std::swap(_wr_pos, rhs._wr_pos);
		}

		void clear(void) {
			_rd_pos = _wr_pos = 0;
		}

		value_type* prepare(const int_type sz) {
			reserve(sz);
			return wrptr();
		}

		void commit(const int_type sz) {
			wrptr(sz);
		}

		void consume(const int_type sz) {
			rdptr(sz);
			if (rdsize() == 0) {
				_rd_pos = _wr_pos = 0;
			}
		}

		int write(const void* ptr, const int_type sz) {
			memcpy(prepare(sz), ptr, sz);
			commit(sz);
			return sz;
		}
	private:
		block_type* _block;
		int_type _rd_pos;
		int_type _wr_pos;
	};

	template < typename T, typename AllocatorType, typename IntType >
	basic_buffer<T, AllocatorType, IntType> deepcopy(basic_buffer<T, AllocatorType, IntType>& rhs) {
		basic_buffer<T, AllocatorType, IntType> nb(rhs.rdsize());
		memcpy(nb.wrptr(), rhs.rdptr(), rhs.rdsize());
		nb.wrptr(rhs.rdsize());
		return nb;
	}

	template < typename T, typename AllocatorType, typename IntType >
	basic_buffer<T, AllocatorType, IntType>& operator<<(basic_buffer<T, AllocatorType, IntType>& buf, const char* msg) {
		IntType len = static_cast<IntType>(strlen(msg) + 1);
		buf.reserve(len);
		buf.write(msg, len);
		return buf;
	}
}

#endif