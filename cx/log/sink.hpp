/**
 */
#ifndef __cx_log_sink_h__
#define __cx_log_sink_h__

#include <cx/log/filter.hpp>
#include <cx/log/formatter.hpp>
#include <cx/log/writer.hpp>
#include <cx/log/internal/string_formatter.hpp>

namespace cx::log {

	class sink {
	public:
		sink(void)
			: _filter(std::make_shared< source_filter<>>()),
			_formatter(cx::log::string_formatter::instance()) {}

		sink(std::shared_ptr<cx::log::filter>&& filter)
			: _filter(filter),
			_formatter(cx::log::string_formatter::instance()) {}

		void put(const cx::log::record& record) {
			_buf.clear();
			if ((*_filter)(record)) {
				(*_formatter)(record, _buf);
				std::for_each(_writers.begin(), _writers.end(), [&](std::shared_ptr<writer>& w) {
					(*w)(record, _buf);
				});
			}
		}

		void add_writer(std::shared_ptr<writer> writer) {
			_writers.push_back(writer);
		}
	private:
		cx::basic_buffer<char> _buf;
		std::shared_ptr<cx::log::filter> _filter;
		std::shared_ptr<cx::log::formatter> _formatter;
		std::vector< std::shared_ptr<writer>> _writers;
	};

}

#endif