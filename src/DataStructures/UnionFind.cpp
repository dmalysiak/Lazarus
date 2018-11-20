/*
 * UnionFind.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: Darius Malysiak
 */

#include "UnionFind.h"

namespace Lazarus {

UnionFind::UnionFind(bool use_tree, bool use_compression) {
	m_initialized = false;
	m_use_tree = use_tree;
	m_normalized = false;
	m_use_compression = use_compression;
	m_unique_class_count = 0;
}

UnionFind::~UnionFind() {
	// TODO Auto-generated destructor stub
}

/**
 * Initializes the structure for n elements (0,....,1)
 * */
void UnionFind::init(unsigned long long n)
{
	if(m_use_tree == false)
	{
		m_r.clear();
		m_sizes.clear();
		m_next.clear();
		m_r_norm.clear();
		m_r_norm_lookup.clear();
		m_r = FastKTuple<long long int>(n);
		m_next = FastKTuple<long long int>(n);
		m_sizes = FastKTuple<unsigned long long int>(n);
		m_r_norm = FastKTuple<long long int>(n);
		m_r_norm_lookup = FastKTuple<long long int>(n);

		m_initialized = true;

		for(unsigned long long i=0;i<n;++i)
		{
			m_r[i] = i;
			m_sizes[i] = 1;
			m_next[i] = -1;
			m_r_norm[i] = -1;
			m_r_norm_lookup[i] = -1;
		}
	}
	else
	{
		m_pred.clear();
		m_rank.clear();
		m_r_norm_tree.clear();
		m_r_norm_tree_lookup.clear();
		m_r_norm_tree_lookup = FastKTuple<long long int>(n);
		m_r_norm_tree = FastKTuple<long long int>(n);
		m_pred = FastKTuple<long long int>(n);
		m_rank = FastKTuple<long long int>(n);

		m_initialized = true;

		for(unsigned long long i=0;i<n;++i)
		{
			m_pred[i] = i;
			m_rank[i] = 0;
			m_r_norm_tree[i] = -1;
			m_r_norm_tree_lookup[i] = -1;
		}
	}

}

/**
 * Union of elements i and j.
 * */
void UnionFind::union_(long long int i, long long int j)
{
	if(m_use_tree == false)
	{

		i = m_r[i];
		j = m_r[j];

		if(i==j)
			return;

		//this ensures that i always has the smallest list size
		if(m_sizes[i] > m_sizes[j])
		{
			long long temp = i;
			i=j;
			j=temp;
		}

		//traverse the list of i and update the representative label to j
		long long s = i;
		for(unsigned long long k=0;k<m_sizes[i]-1;++k)
		{
			m_r[s] = j;
			s = m_next[s];
		}
		//s will always contain the last element in the list
		m_next[s] = m_next[j];//concatenate list to successor of j due to 'm_sizes[i]-1'
		m_next[j] = s;

		//since j is the current representative label; update its list size,
		//this also illustrates that not all values within the size array represent actual list sizes,
		//yet in the end all active representative (existing/active) labels will have correct size values!
		m_sizes[j] = m_sizes[j]+m_sizes[i];
	}
	else
	{
		i = find(i);
		j = find(j);

		if(i==j)
			return;

		//append the smaller tree as subtree
		if(m_rank[i] > m_rank[j])
		{
			m_pred[j] = i;
		}
		else if(m_rank[j] > m_rank[i])
		{
			m_pred[i] = j;
		}
		else //arbitrary inclusion of tree; here the tree of j is appended to that of i
		{
			m_pred[j] = i;
			m_rank[i]++;
		}


	}
}

FastKTuple<long long int>* UnionFind::getClass(unsigned long long i)
{
	if(m_initialized == false)
	{
		ERROR("not initialized")
		return NULL;
	}

	if(m_use_tree == false)
	{
		FastKTuple<long long int>* res = new FastKTuple<long long int>(m_sizes[m_r[i]]);

		long long s = m_r[i];
		for(unsigned long long k=0;k<m_sizes[m_r[i]];++k)
		{
			res[k] = s;
			s = m_next[s];
		}

		return res;
	}
	else
	{
		return NULL;
	}

	return NULL;
}

long long int UnionFind::normalizeRepresentatives()
{
	if(m_initialized == false)
	{
		ERROR("not initialized")
		return -2;
	}
	if(m_normalized == true)//the lookup table will be computed only once
	{
		return -2;
	}

	if(m_use_tree == false)
	{

		long long counter = -1;
		//iterate over the elements
		for(unsigned int i=0;i<m_sizes.size();++i)
		{
			//if we haven't seen this representative label i.e. this was the first occurrence so far
			if(m_r_norm[ m_r[i] ] == -1)
			{
				counter++;//the next value for a new representative label
				m_r_norm[ m_r[i] ] = counter;//register the new label
			}

			//register the label in the lookup table, note that m_r_norm_tree will definitely contain this entry
			m_r_norm_lookup[ i ] = m_r_norm[ m_r[i] ];

		}

		m_normalized = true;

		return counter;

	}
	else
	{
		long long counter = -1;
		//iterate over the elements
		long long rep = -1;
		for(unsigned int i=0;i<m_pred.size();++i)
		{
			rep = find(i);
			//if we haven't seen this representative label i.e. this was the first occurrence so far
			if(m_r_norm_tree[ rep ] == -1)
			{
				counter++;//the next value for a new representative label
				m_r_norm_tree[ rep ] = counter;//register the new label
			}
			//register the label in the lookup table, note that m_r_norm_tree will definitely contain this entry
			m_r_norm_tree_lookup[i] = m_r_norm_tree[ rep ];
		}

		m_normalized = true;

		return counter;
	}

	return -2;
}

} /* namespace Lazarus */
