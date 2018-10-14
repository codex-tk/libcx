/**
 * @brief
 *
 * @file basic_lock.hpp
 * @author ghtak
 * @date 2018-09-03
 */

#ifndef __cx_base_basic_lock_h__
#define __cx_base_basic_lock_h__

#include <cx/base/defines.hpp>

namespace cx {

#if defined(CX_PLATFORM_WIN32)
class critical_section {
public:
    critical_section(void) {
        ::InitializeCriticalSectionAndSpinCount(&_cs, 4000);
    }
    ~critical_section(void) { ::DeleteCriticalSection(&_cs); }
    void lock() { ::EnterCriticalSection(&_cs); }
    bool try_lock() { return ::TryEnterCriticalSection(&_cs) == TRUE; }
    void unlock() { return ::LeaveCriticalSection(&_cs); }

private:
    CRITICAL_SECTION _cs;
};
using lock = critical_section;
#else
using lock = std::recursive_mutex;
#endif

} // namespace cx

#endif