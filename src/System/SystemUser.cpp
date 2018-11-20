/*
 * SystemUser.cpp
 *
 *  Created on: 30.04.2015
 *      Author: Darius Malysiak
 */

#include "SystemUser.h"

#include <grp.h>
#include <errno.h>

//#define _GNU_SOURCE
#include <crypt.h>


namespace Lazarus {

SystemUser::SystemUser(const std::string& username, bool openUpdate)
{
	m_username = username;
	m_user_id = 0;
	m_default_group_id = 0;
	m_encrypted_user_pw = "";

	m_last_pw_change = 0;
	m_minimum_time_between_change = 0;
	m_maximum_time_between_change = 0;
	m_days_pw_change_warning = 0;
	m_days_account_inactive = 0;
	m_days_until_account_expires = 0;

	if(openUpdate==false)
	{
		getSystemData();
	}
}

SystemUser::SystemUser(unsigned int uid, bool openUpdate)
{
	m_username = "";
	m_user_id = uid;
	m_default_group_id = 0;
	m_encrypted_user_pw = "";

	m_last_pw_change = 0;
	m_minimum_time_between_change = 0;
	m_maximum_time_between_change = 0;
	m_days_pw_change_warning = 0;
	m_days_account_inactive = 0;
	m_days_until_account_expires = 0;

	if(openUpdate==false)
	{
		getSystemData();
	}
}

SystemUser::~SystemUser()
{
}

FastNCSList<SystemUser*>* SystemUser::getAllSystemUsers()
{
	FastNCSList<SystemUser*>* list = new FastNCSList<SystemUser*>();

	struct passwd* pw = NULL;

	setpwent();

	while( ( pw=getpwent( ) ) != NULL )
	{
		list->appendLast(new SystemUser(pw->pw_uid,true));
	}

	endpwent();
	setpwent();

	//update the user data
	for(FastNCSList<SystemUser*>::Iterator it = list->getStart();
		it != list->getEnd();++it)
	{
		it.getElement()->getm_data()->getSystemData();
	}

	return list;
}


void SystemUser::printInfo()
{
	printf("Username: %s UID: %d Default GID: %d\n", m_username.c_str(), m_user_id, m_default_group_id);

	printf("GIDs: ");
	for(unsigned int i=0;i<m_user_groups.size();++i)
	{
		printf("%d ",m_user_groups[i]);
	}printf("\n");

	printf("Real name: %s\n",m_realname.c_str());
	printf("User home: %s\n",m_home_dir.c_str());
	printf("User shell: %s\n",m_shell.c_str());
	printf("Encrypted password: %s\n",m_encrypted_user_pw.c_str());
	printf("Password changed %ld days ago, minimal time between changes %ld, "
			"maximal time between changes %ld, %ld days until pw change warning, %ld days until account goes inactive,"
			"%ld days until account expires \n",
			m_last_pw_change,
			m_minimum_time_between_change,
			m_maximum_time_between_change,
			m_days_pw_change_warning,
			m_days_account_inactive,
			m_days_until_account_expires);

}

std::string SystemUser::getEncryptedPassword(const std::string& p, const std::string& id, const std::string& salt)
{
	std::string s_ = "$"+id+"$"+salt+"$";

	const char* c = crypt( p.c_str(), s_.c_str() );

	s_ = std::string(c);
	free((void*)c);

	return s_;
}

int SystemUser::verifyPassword(const std::string& password)
{
	std::string hashedPassword( crypt( password.c_str(), m_encrypted_user_pw.c_str() ) );

	if( hashedPassword.compare(password) == 0)
	{
		return 0;
	}

	return -1;
}

int SystemUser::getSystemData()
{
	struct passwd* pw = NULL;

	//if a username has been set
	if(m_username.compare("") != 0)
	{
		pw = getpwnam(m_username.c_str());
	}
	else //otherwise use the user id
	{
		pw = getpwuid(m_user_id);
		m_username = std::string(pw->pw_name);
	}

	if(pw == NULL)
	{
		printf("ERROR: could not get user information\n");
		endpwent();
		return -1;
	}

	//passwd info
	m_user_id = pw->pw_uid;
	m_default_group_id = pw->pw_gid;

	m_realname = std::string(pw->pw_gecos);
	m_shell = std::string(pw->pw_shell);
	m_home_dir = std::string(pw->pw_dir);


	//get all user groups
	int num=500;
	gid_t groups[num];
	int res = getgrouplist(m_username.c_str(), m_default_group_id, groups, &num);
	if(res < 0)
	{
		printf("ERROR: could not get all GIDs for the user\n");
		endpwent();
		return -1;
	}
	else
	{
		for(int i=0;i<num;++i)
		{
			m_user_groups.push_back(groups[i]);
		}
	}


	struct spwd* sp = getspnam(pw->pw_name);

	if(sp == NULL)
	{
		printf("ERROR:getSystemData could not get shadow data; privileges - (root should work)??\n");
		endpwent();
		return -1;
	}

	//shadow info
	m_last_pw_change = sp->sp_lstchg;
	m_minimum_time_between_change = sp->sp_min;
	m_maximum_time_between_change = sp->sp_max;
	m_days_pw_change_warning = sp->sp_warn;
	m_days_account_inactive = sp->sp_inact;
	m_days_until_account_expires = sp->sp_expire;

	//get the encrypted password
	m_encrypted_user_pw = std::string(sp->sp_pwdp);

	endpwent();

	return 0;
}



} /* namespace SHNetwork */
