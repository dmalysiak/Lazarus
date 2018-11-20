/*
 * Tracer.h
 *
 *  Created on: 17.11.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_SCHEDULING_TRACER_H_
#define CORE_SCHEDULING_TRACER_H_

#include "../BaseObject.h"
#include "../BasicBuffer.h"
#include "../DataStructures/FastKTuple.h"

#include <sys/reg.h>
#include <sys/user.h>

namespace Lazarus {

class Tracer: public BaseObject {
public:

	static const char* SYSCALLS_LOOKUP[243];

	Tracer();
	virtual ~Tracer();

	void printError(long int error);

	/**
	 * Attach to the specified pid.
	 **/
	long int attach(pid_t pid);
	long int seize(pid_t pid);
	long int detach();
	long int interrupt();
	long int continue_();
	long int kill();
	long int continueToNextSyscall();
	long int singleStep();

	/**
	 * Page-aligned data access. Returns up to n bytes of read data. An error will be indicated by printing an error message and setting the boolean var.
	 * On Linux system data and text memory are being shared. data must have a size of at least n.
	 * */
	long int getData(unsigned long long address, const BasicBuffer& data, unsigned long long n, bool& error);
	/**
	 * Page-aligned data access. Sets up to n words.
	 * On Linux system data and text memory are being shared. data must have a size of at least n.
	 * */
	long int setData(unsigned long long address, const BasicBuffer& data, unsigned long long n);


	/**
	 * Prints the system call to the console.
	 * */
	static void decodeSystemCall(unsigned char code);

	/**
	 * Page-aligned data access. Returns a word. An error will be indicated by printing an error message and setting the boolean var.
	 * On Linux system data and text memory are being shared.
	 * */
	long int getText(unsigned long long address, bool& error);
	/**
	 * Page-aligned data access. Sets a word. An error will be indicated by -1;
	 * On Linux system data and text memory are being shared.
	 * */
	long int setText(unsigned long long address, long word);

	/**
	 * Page-aligned data access. Returns a word. An error will be indicated by printing an error message and setting the boolean var.
	 * On Linux system data and text memory are being shared.
	 * */
	long int getData(unsigned long long address, bool& error);
	/**
	 * Page-aligned data access. Sets a word.
	 * On Linux system data and text memory are being shared.
	 * */
	long int setData(unsigned long long address, long word);

	/**
	 * Page-aligned data access. Returns a word. An error will be indicated by printing an error message and setting the boolean var.
	 * Returns a word from the program's USER area.
	 * */
	long int getUser(unsigned long long address, bool& error);
	/**
	 * Page-aligned data access. Sets a word.
	 * Sets a word in the program's USER area.
	 * */
	long int setUser(unsigned long long address, long word);

	/**
	 * Keep in mind to free the returned memory. Returns a NULL pointer in case of errors.
	 * */
	struct user_regs_struct* getRegs();

	/**
	 * Returns -1 in case of errors.
	 * */
	long int setRegs(const struct user_regs_struct& regs);

	/**
	 * Keep in mind to free the returned memory. Returns a NULL pointer in case of errors.
	 * */
	struct user_fpregs_struct* getFPRegs();

	/**
	 * Returns -1 in case of errors.
	 * */
	long int setFPRegs(const struct user_fpregs_struct& regs);

	/**
	 * Keep in mind to free the returned memory. Returns a NULL pointer in case of errors.
	 * */
	struct user_fpxregs_struct* getFPXRegs();

	/**
	 * Returns -1 in case of errors.
	 * */
	long int setFPXRegs(const struct user_fpxregs_struct& regs);

private:
	pid_t m_pid;
	bool m_attached;
};

} /* namespace Lazarus */

#endif /* CORE_SCHEDULING_TRACER_H_ */
