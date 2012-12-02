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

			linkediterator(typename list<K>::iterator itkeys, map<K, V> elements);
			linkediterator(const linkediterator<K, V>& orig);
			~linkediterator();

			const linkediterator* operator->();
			linkediterator& operator++();
			linkediterator operator++(int);

			bool operator==(linkediterator b) const;
			bool operator!=(linkediterator b) const;

			K first;
			V second;
		private:
			typename list<K>::iterator _iterKeys;
			map<K, V> _elements;

	};

template <class K, class V>
class LinkedMap
{
	private:

	public:
		LinkedMap(bool (*keyComparator)(K, K));
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

		V operator[](const K& key);
		V get(const K& key);

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
}

template <class K, class V>
linkediterator<K, V>::linkediterator(const linkediterator<K,V>& orig) {
	this->_iterKeys = orig._iterKeys;
	this->_elements = orig._elements;
}

template <class K, class V>
const linkediterator<K, V>* linkediterator<K, V>::operator->() {
	first = *_iterKeys;
	second = _elements[first];
	return this;
}
/*
template <class K, class V>
const typename linkediterator<K, V>::T* linkediterator<K, V>::operator->() {
	_currentElement->first = *_iterKeys;
	_currentElement->second = _elements[_currentElement->first];

	return _currentElement;
}
*/

template <class K, class V>
linkediterator<K, V>::~linkediterator() {
	/*
	if (_currentElement != NULL) {
		delete _currentElement;
		_currentElement = NULL;
	}
	*/
}

template <class K, class V>
linkediterator<K, V>& linkediterator<K, V>::operator++() {
	_iterKeys++;

	return *this;
}

template <class K, class V>
linkediterator<K, V> linkediterator<K, V>::operator++(int) {
	_iterKeys++;

	return *this;
}

template <class K, class V>
bool linkediterator<K, V>::operator==(linkediterator b) const {
	return (b._iterKeys == this->_iterKeys);
}

template <class K, class V>
bool linkediterator<K, V>::operator!=(linkediterator b) const {
	return (b._iterKeys != _iterKeys);
}

	template <class K, class V>
LinkedMap<K, V>::LinkedMap(bool (*keyComparator)(K, K))
{
	_keyComparator = keyComparator;
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
	typename std::map<K, V>::iterator itelements = _elements.find(key);
	if (itelements != _elements.end()) {
		_elements.erase(itelements);
		// Remove the previous element to be replaced by the new one
		for (typename std::list<K>::iterator itk = _keys.begin(); itk != _keys.end(); itk++) {
			K testKey = *itk;
			if (_keyComparator(key, testKey)) {
				_keys.erase(itk);
				break;
			}
		}
	}
	_keys.push_back(key);
	_elements.insert(pair<K, V> (key, val));
}

template <class K, class V>
V LinkedMap<K,V>::get(const K& key) {
	typename map<K,V>::iterator i = _elements.find(key);
	V result = NULL;
	if (i != _elements.end()) {
		result = i->second;
	}
	return result;
}

template <class K, class V>
V LinkedMap<K,V>::operator [](const K& key) {
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
