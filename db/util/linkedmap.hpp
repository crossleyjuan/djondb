/*
 * =====================================================================================
 *
 *       Filename:  linkedmap.hpp
 *
 *    Description:  this implements a map that keeps the insertion order
 *
 *        Version:  1.0
 *        Created:  11/26/2012 10:15:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */ 
#ifndef LINKEDMAP_INCLUDED_HPP
#define LINKEDMAP_INCLUDED_HPP 

#include <map>
#include <list>

using namespace std;

template <class K, class V>
class linkediterator: 
	std::iterator< std::forward_iterator_tag, K > {
		public:
			struct T {
				K first;
				V second;
			};

			linkediterator(typename list<K>::iterator itkeys, map<K, V> elements);
			linkediterator(const linkediterator& orig);
			~linkediterator();

			T* operator->() const;
			linkediterator& operator++();
			linkediterator operator++(int);

			bool operator==(linkediterator b) const;
			bool operator!=(linkediterator b) const;

		private:
			typename list<K>::iterator _iterKeys;
			map<K, V> _elements;
			K _currentKey;
			V _currentValue;
	};

template <class K, class V>
class LinkedMap
{
	private:

	public:
		LinkedMap();
		virtual ~LinkedMap();

		void add(const K& key, const V& val);
		void erase(const K& key);
		bool containsKey(const K& key) const;

		typedef linkediterator<K, V> iterator;

		iterator begin();
		iterator end();
		void clear();

		typedef typename map<K,V>::size_type size_type;

		size_type size() const;

		iterator operator[](const K& key);
		iterator get(const K& key);

	protected:

	private:
		map<K, V> _elements;
		list<K> _keys;
		int _top;
		bool (*_keyComparator)(K, K);


};

template <class K, class V>
linkediterator<K, V>::linkediterator(typename list<K>::iterator itkeys, map<K, V> elements) {
	_iterKeys = itkeys;
	_elements = elements;
	_currentKey = *_iterKeys;
	_currentValue = elements[_currentKey];
}

template <class K, class V>
linkediterator<K, V>::linkediterator(const linkediterator& orig) {
	this->_iterKeys = orig._iterKeys;
	this->_elements = orig._elements;
	this->_currentKey = orig._currentKey;
	this->_currentValue = orig._currentValue;
}

template <class K, class V>
typename linkediterator<K, V>::T* linkediterator<K, V>::operator->() const {
	linkediterator<K, V>::T* r = new T();
	r->first = _currentKey;
	r->second = _currentValue;

	return r;
}

template <class K, class V>
linkediterator<K, V>::~linkediterator() {
}

template <class K, class V>
linkediterator<K, V>& linkediterator<K, V>::operator++() {
	_iterKeys++;
	_currentKey = *_iterKeys;
	_currentValue = _elements[_currentKey];

	return *this;
}

template <class K, class V>
linkediterator<K, V> linkediterator<K, V>::operator++(int) {
	_iterKeys++;
	_currentKey = *_iterKeys;
	_currentValue = _elements[_currentKey];

	return *this;
}

template <class K, class V>
bool linkediterator<K, V>::operator==(linkediterator b) const {
	return (b._currentKey == this->_currentKey);
}

template <class K, class V>
bool linkediterator<K, V>::operator!=(linkediterator b) const {
	return (b._currentKey != _currentKey);
}

	template <class K, class V>
LinkedMap<K, V>::LinkedMap()
{
}

	template <class K, class V>
LinkedMap<K, V>::~LinkedMap()
{
}

template <class K, class V>
typename LinkedMap<K, V>::iterator LinkedMap<K,V>::begin() {
	linkediterator<K,V> iterator(_keys.begin(), _elements);
	return iterator;
}

template <class K, class V>
typename LinkedMap<K, V>::iterator LinkedMap<K,V>::end() {
	linkediterator<K,V> iterator(_keys.end(), _elements);
	return iterator;
}

template <class K, class V>
void LinkedMap<K,V>::clear() {
	_keys.clear();
	_elements.clear();
}

template <class K, class V>
typename LinkedMap<K, V>::size_type LinkedMap<K, V>::size() const {
	return _elements.size();
}

	template <class K, class V>
void LinkedMap<K, V>::erase(const K& key)
{
	// look at http://www.parashift.com/c++-faq-lite/templates.html#faq-35.18 (Whoa new for me)
	typename map<K,V>::iterator i = _elements.find(key);
	if (i != _elements.end()) {
		_elements.erase(i);
	}

	typename list<K>::iterator ik = _keys.begin();
	while (ik != _keys.end()) {
		K k = *ik;
		if (k == key) {
			_keys.erase(ik);
			break;
		}
		ik++;
	}

}

template <class K, class V>
void LinkedMap<K, V>::add(const K& key, const V& val) {
	_keys.push_back(key);
	_elements.insert(pair<K, V> (key, val));
}

template <class K, class V>
typename LinkedMap<K, V>::iterator LinkedMap<K,V>::get(const K& key) {
	typename map<K,V>::iterator i = _elements.find(key);
	return i;
}

template <class K, class V>
typename LinkedMap<K, V>::iterator LinkedMap<K,V>::operator [](const K& key) {
	return get(key);
}

template <class K, class V>
bool LinkedMap<K, V>::containsKey(const K& key) const {
	typename map<K,V>::const_iterator i = _elements.find(key);
	if (i != _elements.end()) {
		return true;
	} else {
		return false;
	}
}
#endif /* LINKEDMAP_INCLUDED_HPP */
