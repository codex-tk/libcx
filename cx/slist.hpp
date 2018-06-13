/**
 */

#ifndef __cx_slist_h__
#define __cx_slist_h__

namespace cx {

	template < typename T > class slist {
	public:
		slist(void);

		slist( slist&& rhs );

		~slist(void);

		template <class U> void swap(slist<U> &rhs);

		/**
		@return if empty list 0 else 1
		*/
		int add_tail(T *node);
		
		/**
		@return if empty list 0 else 1
		*/
		int add_tail(slist<T>&& rhs);

		T *head(void);

		T *remove_head(void);

	private:
		T *_head;
		T *_tail;
	};

	template <typename T>
	slist<T>::slist(void)
		: _head(nullptr), _tail(nullptr) {
	}

	template <typename T>
	slist<T>::slist(slist<T>&& rhs)
		: _head(rhs._head), _tail(rhs._tail)
	{
		rhs._head = rhs._tail = nullptr;
	}
	

	template <typename T>
	slist<T>::~slist(void) {
	}

	template <typename T>
	template <class U>
	void slist<T>::swap(slist<U> &rhs) {
		std::swap(_head, rhs._head);
		std::swap(_tail, rhs._tail);
	}

	template <typename T>
	int slist<T>::add_tail(T *node) {
		int ret = 0;
		if (_head == nullptr) {
			_head = _tail = node;
		} else {
			_tail->next(node);
			_tail = node;
			ret = 1;
		}
		node->next(nullptr);
		return ret;
	}

	template <typename T>
	int slist<T>::add_tail(slist<T>&& rhs) {
		int ret = 0;
		if (_head == nullptr) {
			_head = rhs._head;
		} else {
			_tail->next(rhs._head);		ret = 1;
			ret = 1;
		}
		_tail = rhs._tail;
		rhs._head = rhs._tail = nullptr;
		return ret;
	}

	template <typename T>
	T *slist<T>::head(void) {
		return _head;
	}

	template <typename T>
	T *slist<T>::remove_head(void) {
		T *head = _head;
		if (_head == _tail) {
			_head = _tail = nullptr;
		} else {
			_head = static_cast<T *>(_head->next());
		}
		return head;
	}
}

#endif