/*
 * GridStructure.h
 *
 *  Created on: 23.01.2013
 *      Author: Darius Malysiak
 */

#ifndef GRIDSTRUCTURE_H_
#define GRIDSTRUCTURE_H_

#include "BaseObject.h"

#include <vector>

namespace Lazarus
{

class GridStructure : public BaseObject{
public:
	GridStructure();
	virtual ~GridStructure();

	void setDimensions(std::vector<unsigned long> dims);
	void printStructure() const;

	CLASS_RTTI(Lazarus::GridStructure)

private:
	unsigned long* mp_dimensions;
	unsigned long m_dimension_count;
};

}

#endif /* GRIDSTRUCTURE_H_ */
