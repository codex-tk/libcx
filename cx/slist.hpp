/**
 */

#ifndef __cx_slist_h__
#define __cx_slist_h__

#include <cx/cxdefine.hpp>

namespace cx {

	template < typename T > class slist {
	public:
		slist(void) noexcept;

		slist(slist&& rhs);

		~slist(void) = default;

		template <class U> void swap(slist<U> &rhs);

		/**
		@return if empty list 0 else 1
		*/
		void add_tail(T *node);

		/**
		@return if empty list 0 else 1
		*/
		void add_tail(slist<T>&& rhs);

		T *head(void);

		T *remove_head(void);

		bool empty(void);

	private:
		T *_head;
		T *_tail;
	};

	template <typename T>
	slist<T>::slist(void) noexcept
		: _head(nullptr), _tail(nullptr) {}

	template <typename T>
	slist<T>::slist(slist<T>&& rhs)
		: _head(rhs._head), _tail(rhs._tail)
	{
		rhs._head = rhs._tail = nullptr;
	}

	template <typename T>
	template <class U>
	void slist<T>::swap(slist<U> &rhs) {
		std::swap(_head, rhs._head);
		std::swap(_tail, rhs._tail);
	}

	template <typename T>
	void slist<T>::add_tail(T *node) {
		assert( node != nullptr );
		if ( node == nullptr ) return;
		if (_head == nullptr) {
			_head = _tail = node;
		} else {
			_tail->next(node);
			_tail = node;
		}
		node->next(nullptr);
	}

	template <typename T>
	void slist<T>::add_tail(slist<T>&& rhs) {
		if (_head == nullptr) {
			_head = rhs._head;
		} else {
			_tail->next(rhs._head);
		}
		_tail = rhs._tail;
		rhs._head = rhs._tail = nullptr;
	}

	template <typename T>
	T *slist<T>::head(void) {
		return _head;
	}

	template <typename T>
	T *slist<T>::remove_head(void) {
		assert( _head != nullptr );
		if ( _head == nullptr )
			return nullptr;

		T *head = _head;
		if (_head == _tail) {
			_head = _tail = nullptr;
		} else {
			_head = static_cast<T *>(_head->next());
		}
		return head;
	}

	template <typename T>
	bool slist<T>::empty(void) {
		return _head == nullptr;
	}
}

#endif