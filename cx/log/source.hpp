/**
 */
#ifndef __cx_log_source_h__
#define __cx_log_source_h__

#include <cx/cxdefine.hpp>

namespace cx::log {

	class source {
	public:
		source(const std::size_t hash) : _hash(hash) {}
		virtual ~source(void) = default;
		virtual const char* name(void) = 0;

		std::size_t hash(void) { return _hash; }
	private:
		std::size_t _hash;
	};

	template < typename SourceType >
	class basic_source : public source {
	public:
		basic_source(void) noexcept
			: source(typeid(SourceType).hash_code()) {}
	};

}

#endif