/**
*/
#ifndef __cx_log_filter_h__
#define __cx_log_filter_h__

#include <cx/log/log.hpp>
#include <cx/log/internal/tag_source.hpp>

namespace cx::log {

	class filter {
	public:
		filter(void) {}
		virtual ~filter(void) {}
		virtual bool operator()(const cx::log::record&) {
			return true;
		}
	public:
		static std::shared_ptr<filter> null_filter(void) {
			static std::shared_ptr<filter> filter_ptr = std::make_shared<filter>();
			return filter_ptr;
		}
	};

	template < typename AcceptSourceType = cx::log::tag_source >
	class source_filter : public filter {
	public:
		explicit source_filter(cx::log::level lv = cx::log::level::error) : _level(lv) {}
		virtual ~source_filter(void) {}
		virtual bool operator()(const cx::log::record& r) {
			if (r.source->hash() == typeid(AcceptSourceType).hash_code()) {
				return _level > r.level;
			}
			return false;
		}
	private:
		cx::log::level _level;
	};
}

#endif