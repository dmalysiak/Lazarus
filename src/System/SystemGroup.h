/*
 * SystemGroup.h
 *
 *  Created on: 30.04.2015
 *      Author: Darius Malysiak
 */

#ifndef SYSTEMGROUP_H_
#define SYSTEMGROUP_H_

#include "../BaseObject.h"
#include "../DataStructures/FastNCSList.h"
#include "SystemUser.h"

#include <grp.h>

namespace Lazarus {

/**
 * Keep in mind that each SystemGroup object will contain a list of the associated users, i.e. a set
 * of SystemUser objects. This may result in a serious memory overhead if its not used with caution.
 * */

class SystemGroup: public BaseObject {
public:

	/**
	 * The value of openUpdate should be set to true IFF a SystemGroup object is created within
	 * a context surrounded by setgrent and endgrent|[setgrent]!!
	 * Otherwise loops may turn into infinite loops!
	 * If openUpdate is true the corresponding group data must be acquired explicitly by calling getSystemData!!
	 * */
	SystemGroup(const std::string& groupname, bool openUpdate=false);

	/**
	 * The value of openUpdate should be set to true IFF a SystemGroup object is created within
	 * a context surrounded by setgrent and endgrent|[setgrent]!!
	 * Otherwise loops may turn into infinite loops!
	 * If openUpdate is true the corresponding group data must be acquired explicitly by calling getSystemData!!
	 * */
	SystemGroup(unsigned int groupid, bool openUpdate=false);

	virtual ~SystemGroup();

	/**
	 * Will read the systems data for the current group id or groupname. if both, groupname and group id are set,
	 * this function will attempt to read the data via the current groupname.
	 * Returns -1 in case of errors, 0 otherwise.
	 **/
	int getSystemData();

	/**
	 * Prints information about the group to the CLI.
	 * */
	void printInfo();

	/**
	 * Returns a list of all readable system groups. Depending on system local security settings it may skip certain
	 * groups.
	 * */
	static FastNCSList<SystemGroup*>* getAllSystemGroups();

	std::string m_groupname;
	std::string m_group_password;
	gid_t m_gid;
	FastNCSList<SystemUser*> m_group_members;

};

} /* namespace Lazarus */

#endif /* SYSTEMGROUP_H_ */
