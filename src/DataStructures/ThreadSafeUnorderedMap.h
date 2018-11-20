/*
 * ThreadSafeUnorderedMap.h
 *
 *  Created on: 12.06.2013
 *      Author: Darius Malysiak
 */

#ifndef THREADSAFEUNORDEREDMAP_H_
#define THREADSAFEUNORDEREDMAP_H_

#include "../BaseObject.h"

#include <pthread.h>
#include <unordered_map>
#include <stdexcept>
#include <atomic>

namespace Lazarus {

template<typename INDEX_TYPE, typename ELEMENT_TYPE>
class ThreadSafeUnorderedMap: public Lazarus::BaseObject {
public:
	ThreadSafeUnorderedMap()
	{
		pthread_rwlock_init(&m_rw_lock,NULL);
		m_lock_list = false;
	}

	/*
	 * this constructor creates a >new< instance of the class
	 */
	ThreadSafeUnorderedMap(const ThreadSafeUnorderedMap<INDEX_TYPE, ELEMENT_TYPE>& map)
	{
		m_lock_list = false;

		//copy the underlying map
		m_map = *(map.getMap());
		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~ThreadSafeUnorderedMap()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}


	/**
	 * These methods are useful if one decides to externally iterate over the elements and manipulate the lists content.
	 * This avoids the work of writing thread safe methods for all 'unordered_map' methods!
	 * Use them with caution!!
	 */
	void externalRDLock()
	{
		pthread_rwlock_rdlock(&m_rw_lock);
	}

	void externalRDUnlock()
	{
		pthread_rwlock_unlock(&m_rw_lock);
	}

	void externalWRLock()
	{
		pthread_rwlock_wrlock(&m_rw_lock);
	}

	void externalWRUnlock()
	{
		pthread_rwlock_unlock(&m_rw_lock);
	}

	ELEMENT_TYPE getElement(INDEX_TYPE key)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return ELEMENT_TYPE();
		}

		ELEMENT_TYPE element;

		try
		{
			element = m_map.at(key);
		}
		catch(const std::out_of_range &oe)
		{
			printf("ERROR: element not found in map\n");
			pthread_rwlock_unlock(&m_rw_lock);
			throw;
		}

		pthread_rwlock_unlock(&m_rw_lock);

		return element;
	}

	ELEMENT_TYPE operator[](INDEX_TYPE key)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return ELEMENT_TYPE();
		}

		ELEMENT_TYPE element;

		try
		{
			element = m_map.at(key);
		}
		catch(const std::out_of_range &oe)
		{
			printf("ERROR: element not found in map\n");
			pthread_rwlock_unlock(&m_rw_lock);
			throw;
		}

		pthread_rwlock_unlock(&m_rw_lock);

		return element;
	}

	void insertElement(INDEX_TYPE key, ELEMENT_TYPE element)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		m_map[key] = element;

		pthread_rwlock_unlock(&m_rw_lock);
	}

	void clearMap()
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		m_map.clear();

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/*
	 * only call this method if the map contains pointers to objects!
	 */
	void clearMapWithDestructor()
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		for(typename std::unordered_map<INDEX_TYPE,ELEMENT_TYPE>::iterator it = m_map.begin(); it != m_map.end(); ++it)
		{
			//delete the object
			delete it->second;
		}

		m_map.clear();

		pthread_rwlock_unlock(&m_rw_lock);
	}

	void eraseElement(typename std::unordered_map<INDEX_TYPE, ELEMENT_TYPE>::iterator it)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		m_map.erase(it);

		pthread_rwlock_unlock(&m_rw_lock);
	}

	void eraseElement(INDEX_TYPE key)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		m_map.erase(key);

		pthread_rwlock_unlock(&m_rw_lock);
	}

	void eraseElement(typename std::unordered_map<INDEX_TYPE, ELEMENT_TYPE>::iterator first,
			typename std::unordered_map<INDEX_TYPE, ELEMENT_TYPE>::iterator last)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//abort if locked
		if(m_lock_list == true)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		m_map.erase(first,last);

		pthread_rwlock_unlock(&m_rw_lock);
	}

	std::unordered_map<INDEX_TYPE, ELEMENT_TYPE>* getMap()
	{
		return &m_map;
	}

	//setting this value to true locks the list for insert/delete ops
	std::atomic_bool m_lock_list;

private:
	std::unordered_map<INDEX_TYPE, ELEMENT_TYPE> m_map;
	pthread_rwlock_t m_rw_lock;

};

} /* namespace Lazarus */
#endif /* THREADSAFEUNORDEREDMAP_H_ */
