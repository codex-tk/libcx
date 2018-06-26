/**
 * @brief buffer
 *
 * @file buffer.hpp
 * @author ghtak
 * @date 2018-05-12
 */
#include <cx/cxdefine.hpp>

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
	template < typename T, typename AllocatorType >
	class block {
	public:
		block(const int32_t size)
			: _ref_count(1), _size(size) {}

		T* base(void) const noexcept { return const_cast<T*>(reinterpret_cast<const T*>(this + 1)); }
		int32_t size(void) const noexcept { return _size; }

		int32_t use_count(void) const noexcept { return _ref_count.load(); }
		int32_t add_ref(void) noexcept { return _ref_count.fetch_add(1); }
		int32_t release(void) {
			int32_t cnt = _ref_count.fetch_sub(1);
			assert(cnt > 0);
			if (cnt == 1) {
				AllocatorType::free(this);
			}
			return cnt;
		}
	private:
		std::atomic<int32_t> _ref_count;
		int32_t _size;
	};
}

namespace cx {
	/**
	 * @brief
	 *
	 * @tparam
	 * @tparam Allocator
	 */
	template < typename T, typename AllocatorType = internal::basic_buffer_allocator >
	class basic_buffer {
	public:
		using block_type = internal::block<T, AllocatorType>;
		basic_buffer(void)
			:_block(nullptr), _rd_pos(0), _wr_pos(0) {}

		basic_buffer(const std::size_t sz)
			: basic_buffer()
		{
			_block = reinterpret_cast<block_type*>(
				AllocatorType::alloc(sizeof(block_type) + (sizeof(T) * sz)
				));
			_block = new (_block) block_type(sz);
		}

		basic_buffer(const basic_buffer& rhs)
			: _block(rhs._block), _rd_pos(rhs._rd_pos), _wr_pos(rhs._wr_pos) {
			if (_block)
				_block->add_ref();
		}

		basic_buffer(basic_buffer&& rhs)
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

		basic_buffer& operator=(basic_buffer&& rhs) {
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

		T* base(void) { return _block ? _block->base() : nullptr; }
		int size(void) { return _block ? _block->size() : 0; }

		T* rdptr(void) { return _block ? _block->base() + _rd_pos : nullptr; }
		int rdptr(int n) {
			int offset = (n >= 0 ?
				std::min(n, rdsize())
				: std::max(n, _rd_pos * -1));
			_rd_pos += offset;
			return offset;
		}
		int rdsize(void) { return _wr_pos - _rd_pos; }

		T* wrptr(void) { return _block ? _block->base() + _wr_pos : nullptr; }
		int wrptr(int n) {
			int offset = (n >= 0 ?
				std::min(n, wrsize())
				: std::max(n, rdsize() * -1));
			_wr_pos += offset;
			return offset;
		}
		int wrsize(void) { return size() - _wr_pos; }

		void reserve(const int sz) {
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

		T* prepare(const int sz) {
			reserve(sz);
			return wrptr();
		}
		void commit(const int sz) {
			wrptr(sz);
		}
		void consume(const int sz) {
			rdptr(sz);
			if (rdsize() == 0) {
				_rd_pos = _wr_pos = 0;
			}
		}

	private:
		block_type* _block;
		int _rd_pos;
		int _wr_pos;
	};

	template < typename T , typename AllocatorType >
	basic_buffer<T, AllocatorType> deepcopy(basic_buffer<T, AllocatorType>& rhs) {
		basic_buffer<T, AllocatorType> nb(rhs.rdsize());
		memcpy(nb.wrptr(), rhs.rdptr(), rhs.rdsize());
		nb.wrptr(rhs.rdsize());
		return nb;
	}
}