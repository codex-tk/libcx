/**
 * @brief
 *
 * @file utils.hpp
 * @author ghtak
 * @date 2018-08-19
 */
#ifndef __cx_base_utils_h__
#define __cx_base_utils_h__

#include <cx/base/defines.hpp>

namespace cx {

	/**
	 * @brief
	 *
	 * @param p
	 * @param sz
	 * @return uint16_t
	 */
	uint16_t checksum(void* p, int sz);

	/**
	 * @brief
	 *
	 * @param name
	 * @return std::string
	 */
	std::string demangle(const char *name);

	template <typename T>
	std::string type_name(const T& t) {
		return demangle(typeid(t).name());
	}

	/**
	 * @brief 
	 * 
	 * @tparam T 
	 */
	template < typename T >
	class clazz {
	public:
		template< typename U > using mem_ptr_t = U T::*;

		template< typename S > struct mem_func_helper;
		template< typename R, typename ... Args > struct mem_func_helper< R(Args...)> {
			using type = R(T::*)(Args...);
		};

		template< typename S > using mem_func_ptr_t = typename mem_func_helper<S>::type;

		/**
		 * @brief 
		 * 
		 * @tparam U 
		 * @param ptr_to_member 
		 * @return std::ptrdiff_t 
		 */
		template< typename U >
		static std::ptrdiff_t offset_of(const mem_ptr_t<U> ptr_to_member) {
			const T* const pthis = nullptr;
			const uint8_t *const member = static_cast<const uint8_t *>(
				static_cast<const void *>(&(pthis->*ptr_to_member)));
			std::ptrdiff_t offset(member - static_cast<const uint8_t *>(
				static_cast<const void *>(pthis)));
			return offset;
		}

		/**
		 * @brief 
		 * 
		 * @tparam U 
		 * @param member 
		 * @param ptr_to_member 
		 * @return T* 
		 */
		template<typename U>
		static T* container_of(U *member, const mem_ptr_t<U> ptr_to_member) {
			return static_cast<T*>(
				static_cast<void*>(
					static_cast<uint8_t*>(
						static_cast<void*>(member)) - offset_of(ptr_to_member)
					)
				);
		}

		/**
		 * @brief 
		 * 
		 * @tparam U 
		 * @param member 
		 * @param ptr_to_member 
		 * @return const T* 
		 */
		template<typename U>
		inline const T *container_of(const U *member, const mem_ptr_t<U> ptr_to_member)
		{
			return static_cast<const T*>(
				static_cast<const void*>(
					static_cast<const uint8_t*>(static_cast<const void*>(member)) - offset_of(ptr_to_member)
					)
				);
		}
	};


	// [from boost::intrinsic][[
	template<class Parent, class Member>
	std::ptrdiff_t offset_of(const Member Parent::*ptr_to_member) {
		const Parent * const parent = nullptr;
		const char *const member = static_cast<const char*>(
			static_cast<const void*>(&(parent->*ptr_to_member)));
		std::ptrdiff_t val(member - static_cast<const char*>(
			static_cast<const void*>(parent)));
		return val;
	}

	template<class Parent, class Member>
	inline Parent* container_of(Member *member, const Member Parent::* ptr_to_member)
	{
		return static_cast<Parent*>(
			static_cast<void*>(
				static_cast<char*>(
					static_cast<void*>(member)) - offset_of(ptr_to_member)
				)
			);
	}

	template<class Parent, class Member>
	inline const Parent *container_of(const Member *member, const Member Parent::* ptr_to_member)
	{
		return static_cast<const Parent*>(
			static_cast<const void*>(
				static_cast<const char*>(static_cast<const void*>(member)) - offset_of(ptr_to_member)
				)
			);
	}
	// ]]

	/**
	 * @brief 
	 * 
	 * @param ptr 
	 * @param size 
	 * @param space 
	 * @param crlf 
	 * @return std::string 
	 */
	std::string hex_str(void* ptr, const int size, const int space = 4, const int crlf = 16);
}

#endif