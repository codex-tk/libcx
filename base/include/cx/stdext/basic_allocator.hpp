/**
 * @brief
 *
 * @file basic_allocator.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_stdext_basic_allocator_h__
#define __cx_stdext_basic_allocator_h__

namespace cx::stdext {

    /**
     * @brief 
     * 
     * @tparam T 
     */
	template <typename T> struct basic_allocator {
		typedef T value_type;

		basic_allocator() noexcept {} //default ctor not required by STL\

		template <typename U> basic_allocator(const basic_allocator<U> &) noexcept {}

		template <typename U> bool operator==(const basic_allocator<U> &) const noexcept {
			return true;
		}

		template <typename U> bool operator!=(const basic_allocator<U> &) const noexcept {
			return false;
		}

		T *allocate(const size_t n) const {
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

		void deallocate(T * const p, size_t) const noexcept {
			free(p);
		}

		template< typename... Args >
		void construct(T* p, Args&&... args) {
			new (p) T(std::forward<Args>(args)...);
		}

		void destroy(T* p) {
			p->~T();
		}
	};
}

#endif