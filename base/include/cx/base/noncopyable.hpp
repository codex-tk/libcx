/**
 * @brief
 *
 * @file noncopyable.hpp
 * @author ghtak
 * @date 2018-08-26
 */

#ifndef __cx_base_noncopyable_h__
#define __cx_base_noncopyable_h__

namespace cx {

      /**
       * @brief 
       * 
       */
	class noncopyable{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:
		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable&) = delete;
	};

}

#endif