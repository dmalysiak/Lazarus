/*
 * GridStructure.cpp
 *
 *  Created on: 23.01.2013
 *      Author: Darius Malysiak
 */

#include "GridStructure.h"
#include "Toolbox.h"

#include <stdio.h>

namespace Lazarus
{


GridStructure::GridStructure() {

	m_dimension_count = 0;
	mp_dimensions = NULL;

}

GridStructure::~GridStructure() {

	DELETE_ARRAY_NULL_CHECKING(mp_dimensions);

}

void GridStructure::printStructure() const
{
	printf("******** printing grid structure ********\n");

	for(unsigned int i=0;i<m_dimension_count;i++)
	{
		printf("dimension ");
		printf(CSV_UINT_FORMAT,i);
		printf(" has ");
		printf(CSV_ULONG_FORMAT,mp_dimensions[i]);
		printf("slots\n");
	}

	printf("*****************************************\n");
}

void GridStructure::setDimensions(std::vector<unsigned long> dims)
{
	DELETE_ARRAY_NULL_CHECKING(mp_dimensions);

	m_dimension_count = dims.size();
	mp_dimensions = new unsigned long[dims.size()];

	for(unsigned int i=0;i<m_dimension_count;i++)
	{
		mp_dimensions[i] = dims.at(i);
	}

}

}

