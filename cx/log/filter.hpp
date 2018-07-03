/**
*/
#ifndef __cx_log_filter_h__
#define __cx_log_filter_h__

#include <cx/log/log.hpp>
#include <cx/log/internal/tag_source.hpp>

namespace cx::log {

	class filter {
	public:
		filter(void) noexcept {}
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

	template < typename AcceptSourceType = cx::log::tag_source
		, typename LevelCompareType = std::greater_equal<cx::log::level>>
	class simple_source_filter : public filter {
	public:
		explicit simple_source_filter(cx::log::level lv = cx::log::level::trace) noexcept
			: _level(lv) {}
		virtual ~simple_source_filter(void) {}
		virtual bool operator()(const cx::log::record& r) {
			if (r.source->hash() == typeid(AcceptSourceType).hash_code()) {
				return _compare(r.level, _level);
			}
			return false;
		}
	private:
		LevelCompareType _compare;
		cx::log::level _level;
	};

	template < typename LevelCompareType = std::greater_equal<cx::log::level>>
	class simple_level_filter : public filter {
	public:
		explicit simple_level_filter(cx::log::level lv = cx::log::level::trace) noexcept
			: _level(lv) {}
		virtual ~simple_level_filter(void) {}
		virtual bool operator()(const cx::log::record& r) {
			return _compare(r.level, _level);
		}
	private:
		LevelCompareType _compare;
		cx::log::level _level;
	};
}

#endif