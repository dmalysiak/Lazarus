/*
 * SystemUser.h
 *
 *  Created on: 30.04.2015
 *      Author: Darius Malysiak
 */

#ifndef SYSTEMUSER_H_
#define SYSTEMUSER_H_

#include "../BaseObject.h"
#include "../DataStructures/FastNCSList.h"

#include <string>
#include <vector>

#include <grp.h>
#include <pwd.h>
#include <shadow.h>

namespace Lazarus {

/**
 * This class is a wrapper for a single user, one can use 'getSystemData' to read all
 * system information for the corresponding user (identified by either username or user id).
 * It is assumed that the system uses the shadow pw system.
 **/

class SystemUser: public Lazarus::BaseObject {
public:

	/**
	 * The value of openUpdate should be set to true IFF a SystemUser object is created within
	 * a context surrounded by setpwent and endpwent|[setpwent]!!
	 * Otherwise loops may turn into infinite loops!
	 * If openUpdate is true the corresponding user data must be acquired explicitly by calling getSystemData!!
	 * */
	SystemUser(const std::string& username, bool openUpdate=false);

	/**
	 * The value of openUpdate should be set to true IFF a SystemUser object is created within
	 * a context surrounded by setpwent and endpwent|[setpwent]!!
	 * Otherwise loops may turn into infinite loops!
	 * If openUpdate is true the corresponding user data must be acquired explicitly by calling getSystemData!!
	 * */
	SystemUser(unsigned int uid, bool openUpdate=false);

	virtual ~SystemUser();

	/**
	 * Prints information about the user to the CLI.
	 * */
	void printInfo();

	/**
	 * Returns a list of all users which exist on the current system.
	 * */
	static FastNCSList<SystemUser*>* getAllSystemUsers();


	/**
	 * Returns the 'readable' hash of the message p, ID determines the used hash function:
	 * 1=MD5, 2a=2y=blowfish, 5=SHA256, 6=SHA512.
	 * salt is the seed for the hash function, contains symbols from [a-zA-Z0-9./] and
	 * is at most 16 chars long.
	 * */
	static std::string getEncryptedPassword(const std::string& p, const std::string& id, const std::string& salt);

	/**
	 * Will read the systems data for the current user id or username. if both, username and user id are set,
	 * this function will attempt to read the data via the current username.
	 * WARNING: do not call this method in a context which surrounds the call with setpwent and endpwent|[setpwent]!!
	 * Otherwise loops may turn into infinite loops!
	 * Depending on system local security settings it may skip certain users.
	 * Returns -1 in case of errors, 0 otherwise.
	 **/
	int getSystemData();

	/**
	 * Returns -1 if the given password does not match the currently active one, returns 0 otherwise.
	 * */
	int verifyPassword(const std::string& password);

	std::string m_username;
	std::string m_realname;
	std::string m_shell;
	std::string m_home_dir;
	__uid_t m_user_id;
	__gid_t m_default_group_id;
	std::vector<__gid_t> m_user_groups;//list of GID to which the user belongs

	/*This string contains the shadow files entry for the current user; keep in mind that this entry
	 * is of the form ID$SALT$HASH. The id represents the hash function:
	 * 1=MD5, 2a=2y=blowfish, 5=SHA256, 6=SHA512.
	 * It may contain 'x','!','LK' or other sequences (distribution dependent) if the account is locked.
	 * If the shadow file could not be accessed this string will be empty.
	 * */
	std::string m_encrypted_user_pw;

	long int m_last_pw_change;		/* Date of last change.  */
	long int m_minimum_time_between_change;		/* Minimum number of days between changes.  */
	long int m_maximum_time_between_change;		/* Maximum number of days between changes.  */
	long int m_days_pw_change_warning;		/* Number of days to warn user to change the password.  */
	long int m_days_account_inactive;		/* Number of days the account may be inactive.  */
	long int m_days_until_account_expires;		/* Number of days since 1970-01-01 until account expires.*/


};

} /* namespace SHNetwork */

#endif /* SYSTEMUSER_H_ */
