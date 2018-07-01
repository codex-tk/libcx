/**
 */
#ifndef __cx_log_sink_h__
#define __cx_log_sink_h__

#include <cx/log/filter.hpp>
#include <cx/log/formatter.hpp>
#include <cx/log/writer.hpp>

namespace cx::log {

	class sink {
	public:
		explicit sink(std::shared_ptr<cx::log::filter> filter,
			std::shared_ptr<cx::log::formatter> formatter)
			: _filter(filter),
			_formatter(formatter) {}

		void put(const cx::log::record& record) {
			if ((*_filter)(record)) {
				_buf.clear();
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