/*
 * UnionFind.h
 *
 *  Created on: Jan 25, 2017
 *      Author: Darius Malysiak
 */

#ifndef CORE_DATASTRUCTURES_UNIONFIND_H_
#define CORE_DATASTRUCTURES_UNIONFIND_H_

#include "../BaseObject.h"
#include "../DataStructures/FastKTuple.h"

#include <iostream>

namespace Lazarus {

/**
 * An array/(compressed)intree implementation of the union find structure.
 * */

class UnionFind: public BaseObject {
public:
	UnionFind(bool use_tree = true, bool use_compression=true);
	virtual ~UnionFind();

	/**
	 * Initializes the structure for n elements (0,....,1)
	 * */
	void init(unsigned long long n);

	/**
	 * Union of elements i and j.
	 * */
	void union_(long long int i, long long int j);

	/**
	 * Find the representative label for i. For array implementations its complexity equals O(1), while for tree mode it equals O(log(n))
	 * */
	inline long long int find(long long i)
	{
		if(m_use_tree == false)
			return m_r[i];
		else
		{
			//traverse the tree in order to reach the root
			long long root = 0;
			long long pred = i;
			long long pred_pred = m_pred[i];
			while(pred != pred_pred)
			{
				pred = pred_pred;
				pred_pred = m_pred[pred_pred];
			}
			root = pred;

			//again but this time chain elements directly to root
			if(m_use_compression == true)
			{
				pred = i;
				pred_pred = m_pred[i];
				while(pred != pred_pred)
				{
					m_pred[pred] = root;

					pred = pred_pred;
					pred_pred = m_pred[pred_pred];

				}
			}

			return root;
		}

		return 0;
	}

	/**
	 * Returns the number of unique classes.
	 * */
	inline long long getClassCount()
	{
		if(m_normalized == false)
			m_unique_class_count = normalizeRepresentatives();

		return m_unique_class_count;
	}

	/**
	 * Find the normalized representative label for i. For array implementations its complexity equals O(1), while for tree mode it equals O(log(n))
	 * */
	inline long long int findNormalized(long long i)
	{
		//if required normalize the labels
		if(m_normalized == false)
			m_unique_class_count = normalizeRepresentatives();


		if(m_use_tree == false)
			return m_r_norm_lookup[i];//get entry from lookup table
		else
		{
			return m_r_norm_tree_lookup[i];//get entry from lookup table
		}

		return 0;
	}

	/**
	 * Returns the size of the elements equivalence class. Will return 0 in case of tree implementations.
	 * */
	inline unsigned long long int getSize(long long i)
	{
		if(m_use_tree == false)
			return m_sizes[m_r[i]];

		return 0;
	}

	/**
	 * Returns the class of equivalent labels, the representative label will always be the first in array. One should not forget to delete the pointer.
	 * Returns NULL in case that no initialization occurred.
	 * In case of a tree implementation this method will return NULL.
	 * */
	FastKTuple<long long int>* getClass(unsigned long long i);

private:
	//---------------- vars for the array implementation

	//contains the representative labels
	FastKTuple<long long int> m_r;
	//contains the normalized representative labels
	FastKTuple<long long int> m_r_norm;
	FastKTuple<long long int> m_r_norm_lookup;
	//contains the size of the labels equivalence set
	FastKTuple<unsigned long long int> m_sizes;
	//contains the intree pointer for each label, using this structure allows one to have O(1) complexity for 'find' since it can use m_elements
	FastKTuple<long long int> m_next;

	//---------------- vars for the tree implementation
	//contains the predecessor labels
	FastKTuple<long long int> m_pred;
	//contains the rank of the i'th subtree
	FastKTuple<long long int> m_rank;
	//contains the normalized predecessor label
	FastKTuple<long long int> m_r_norm_tree;
	FastKTuple<long long int> m_r_norm_tree_lookup;


	/**
	 * Performs a sequential renumbering of the representatives. Returns the number of unique representatives.
	 * Returns -2 in case that no initialization occurred.
	 * */
	long long normalizeRepresentatives();

	bool m_initialized;
	bool m_use_tree;
	bool m_normalized;
	bool m_use_compression;

	long long m_unique_class_count;
};

} /* namespace Lazarus */

#endif /* CORE_DATASTRUCTURES_UNIONFIND_H_ */
