/**
 */

#ifndef __cx_slist_h__
#define __cx_slist_h__

namespace cx {

	template < typename T > class slist {
	public:
		slist(void);

		~slist(void);

		template <class U> void swap(slist<U> &rhs);

		void add_tail(T *node);

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
	slist<T>::~slist(void) {
	}

	template <typename T>
	template <class U>
	void slist<T>::swap(slist<U> &rhs) {
		std::swap(_head, rhs._head);
		std::swap(_tail, rhs._tail);
	}

	template <typename T>
	void slist<T>::add_tail(T *node) {
		if (_head == nullptr) {
			_head = _tail = node;
		} else {
			_tail->next(node);
			_tail = node;
		}
		node->next(nullptr);
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