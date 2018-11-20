/*
 * InterfaceToolbox.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef INTERFACETOOLBOX_H_
#define INTERFACETOOLBOX_H_

#include "../BaseObject.h"
#include "../DataStructures/FastNCSList.h"

#include "NetworkInterface.h"

#include <string>

namespace Lazarus {

class InterfaceToolbox: public Lazarus::BaseObject {
public:
	InterfaceToolbox();
	virtual ~InterfaceToolbox();

	/*
	 * Returns a list of network interfaces, including the loopback interface
	 */
	static FastNCSList<NetworkInterface*>* getAllNICs();

	/*
	 * Returns a list of network interfaces, excluding the loopback interface
	 */
	static FastNCSList<NetworkInterface*>* getAllRealNICs();

	/*
	 * Returns the first among the found network interfaces, excluding the loopback interface.
	 * If no real NIC could be found this method returns NULL.
	 */
	static NetworkInterface* getFirstRealNIC();

	/*
	 * Returns the NIC identified by the given name.
	 * If no real NIC could be found this method returns NULL.
	 */
	static NetworkInterface* getNIC(const std::string& nic_name);

	static char* get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);
	static unsigned int get_port(const struct sockaddr *sa);
	static char* get_net_mask(const struct sockaddr *sa, char *s, size_t maxlen);
};

} /* namespace Lazarus */
#endif /* INTERFACETOOLBOX_H_ */
