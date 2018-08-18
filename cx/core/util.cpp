#include <cx/core/util.hpp>

namespace cx {

	uint16_t checksum(void* p, int sz) {
		assert(p != nullptr);
		assert(sz >= 0);
		int32_t sum = 0;
		if (p != nullptr) {
			uint16_t *ptr = static_cast<uint16_t *>(p);
			for (; sz > 1; sz -= 2) {
				sum += *ptr;
				++ptr;
			}
			if (sz == 1)
				sum += static_cast<uint16_t>(*reinterpret_cast<uint8_t *>(ptr));
		}
		sum = (sum >> 16) + (sum & 0xffff);
		sum += (sum >> 16);
		return static_cast<uint16_t>(~sum);
	}

}