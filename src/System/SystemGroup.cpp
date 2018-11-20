/*
 * SystemGroup.cpp
 *
 *  Created on: 30.04.2015
 *      Author: Darius Malysiak
 */

#include "SystemGroup.h"

namespace Lazarus {

SystemGroup::SystemGroup(const std::string& groupname, bool openUpdate)
{
	m_groupname = groupname;
	m_gid = 0;

	if(openUpdate==false)
	{
		getSystemData();
	}
}

SystemGroup::SystemGroup(unsigned int groupid, bool openUpdate)
{
	m_gid = groupid;

	if(openUpdate==false)
	{
		getSystemData();
	}
}

void SystemGroup::printInfo()
{
	printf("Group name: %s GID: %u \n",m_groupname.c_str(),m_gid);
	printf("Group password %s\n",m_group_password.c_str());
	printf("The group has %lu members\n",m_group_members.getSize());
	printf("Printing user info:\n~~~~~~~~~~~~~~~~\n");

	for(Lazarus::FastNCSList<SystemUser*>::Iterator it = m_group_members.getStart(); it != m_group_members.getEnd(); ++it)
	{
		it.getElement()->getm_data()->printInfo();
		printf("~~~~~~~~~~~~~~~~\n");
	}
}

SystemGroup::~SystemGroup()
{
	m_group_members.destroy();
}

FastNCSList<SystemGroup*>* SystemGroup::getAllSystemGroups()
{
	FastNCSList<SystemGroup*>* list = new FastNCSList<SystemGroup*>();

	struct group* group = NULL;

	setgrent();

	while( ( group=getgrent( ) ) != NULL )
	{
		list->appendLast(new SystemGroup(group->gr_gid,true));
	}

	endgrent();
	setgrent();

	//update the user data
	for(FastNCSList<SystemGroup*>::Iterator it = list->getStart();
		it != list->getEnd();++it)
	{
		it.getElement()->getm_data()->getSystemData();
	}

	return list;
}

int SystemGroup::getSystemData()
{
	struct group* gr = NULL;

	//if a username has been set
	if(m_groupname.compare("") != 0)
	{
		gr = getgrnam(m_groupname.c_str());
	}
	else //otherwise use the user id
	{
		gr = getgrgid(m_gid);
		m_groupname = std::string(gr->gr_name);
	}

	if(gr == NULL)
	{
		printf("ERROR: could not get group information\n");
		endgrent();
		return -1;
	}

	//the group password
	m_group_password = std::string(gr->gr_passwd);

	//the group members
	m_group_members.destroy();
	int i=0;
	while(gr->gr_mem[i] != NULL)
	{
		m_group_members.appendLast(new SystemUser(std::string(gr->gr_mem[i])));
		++i;
	}

	endgrent();

	return 0;
}

} /* namespace Lazarus */
