/*
 * XMLConfigMarkISection.h
 *
 *  Created on: Feb 11, 2014
 *      Author: clustro
 */

#ifndef XMLCONFIGMARKISECTION_H_
#define XMLCONFIGMARKISECTION_H_

#include "../BaseObject.h"
#include "XMLConfigMarkIParameters.h"
#include "../DataStructures/FastNCSList.h"

#include <unordered_map>

//stack iterators
#define XMARK1_UINT_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIUInt*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_INT_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIInt*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_ULONG_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULong*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_LONG_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILong*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_ULONGLONG_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULongLong*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_LONGLONG_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILongLong*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_DOUBLE_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIDouble*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_FLOAT_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIFloat*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_STRING_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIString*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))->getParam()

#define XMARK1_UINTARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIUIntArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_INTARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIIntArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_ULONGARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULongArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_LONGARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILongArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_ULONGLONGARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULongLongArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_LONGLONGARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILongLongArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_DOUBLEARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIDoubleArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_FLOATARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIFloatArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_STRINGARRAY_CAST(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIStringArray*)(ITERATOR.getElement()->getm_data()->getParameter(#PARAM)))

//heap iterators
#define XMARK1_UINT_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIUInt*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_INT_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIInt*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_ULONG_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULong*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_LONG_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILong*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_ULONGLONG_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULongLong*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_LONGLONG_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILongLong*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_DOUBLE_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIDouble*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_FLOAT_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIFloat*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define XMARK1_STRING_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIString*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))->getParam()

#define XMARK1_UINTARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIUIntArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_INTARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIIntArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_ULONGARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULongArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_LONGARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILongArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_ULONGLONGARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIULongLongArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_LONGLONGARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkILongLongArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_DOUBLEARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIDoubleArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_FLOATARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIFloatArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))
#define XMARK1_STRINGARRAY_CAST_H(PARAM, ITERATOR) ((Lazarus::XMLConfigMarkIStringArray*)(ITERATOR->getElement()->getm_data()->getParameter(#PARAM)))


namespace Lazarus {

class XMLConfigMarkISection: public Lazarus::BaseObject {
public:
	XMLConfigMarkISection(const std::string& name);
	virtual ~XMLConfigMarkISection();

	//this method takes ownership over the parameter
	void addParameter(XMLConfigParameter* param);

	/* will throw a std::out_of_range exception if no element could be found
	 * the returned parameter must be casted into the correct subtype via 'getType()'
	 */
	XMLConfigParameter* getParameter(const std::string& name);

	const std::string& getName();

	Lazarus::FastNCSList<XMLConfigParameter*>* getParameters();

private:
	std::string m_name;
	Lazarus::FastNCSList<XMLConfigParameter*> m_params;

	std::unordered_map<std::string,XMLConfigParameter*> m_parameter_map;
};

} /* namespace Lazarus */
#endif /* XMLCONFIGMARKISECTION_H_ */

