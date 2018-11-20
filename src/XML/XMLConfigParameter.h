/*
 * XMLConfigParameter.h
 *
 *  Created on: Feb 10, 2014
 *      Author: Darius Malysiak
 */

#ifndef XMLCONFIGPARAMETER_H_
#define XMLCONFIGPARAMETER_H_

#include "../BaseObject.h"
#include <string>

namespace Lazarus {

class XMLConfigParameter: public Lazarus::BaseObject {
public:
	XMLConfigParameter();
	XMLConfigParameter(int type, std::string name);
	virtual ~XMLConfigParameter();

	const std::string& getName() const;
	int getType();

protected:
	int m_type;
	std::string m_name;
};

} /* namespace Lazarus */
#endif /* XMLCONFIGPARAMETER_H_ */
