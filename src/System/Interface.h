/*
 * Interface.h
 *
 *  Created on: Sep 1, 2013
 *      Author: Darius Malysiak
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "../BaseObject.h"

#include <string>

namespace Lazarus {

class Interface: public Lazarus::BaseObject {
public:
	Interface();
	virtual ~Interface();

	void set_dev_filename(std::string dev_filename);
	void set_interface_name(std::string interface_name);

	const std::string get_dev_filename() const;
	const std::string get_interface_name() const;

protected:
	std::string m_interface_name;
	std::string m_dev_filename;
};

} /* namespace Lazarus */
#endif /* INTERFACE_H_ */
