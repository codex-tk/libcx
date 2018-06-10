/**
 *
 */
#ifndef __cx_stdext_removable_priority_queue_h__
#define __cx_stdext_removable_priority_queue_h__

#include <cx/cxdefine.hpp>

namespace cx::stdext {

	template <typename T, typename Contaner = std::vector<T>, typename Cmp = std::less<T>>
	class removable_priority_queue
		: public std::priority_queue<T, Contaner, Cmp> {
	public:
		bool remove(const T &value) {
			auto it = std::find(this->c.begin(), this->c.end(), value);
			if (it != this->c.end()) {
				this->c.erase(it);
				std::make_heap(this->c.begin(), this->c.end(), this->comp);
				return true;
			} else {
				return false;
			}
		}
	};
} // namespace cx::stdext

#endif