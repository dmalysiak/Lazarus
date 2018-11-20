/*
 * Tracer.cpp
 *
 *  Created on: 17.11.2016
 *      Author: Darius Malysiak
 */

#include "Tracer.h"

#include <sys/ptrace.h>

namespace Lazarus {

Tracer::Tracer() {
	m_attached = false;
	m_pid = 0;
}

Tracer::~Tracer() {
	// TODO Auto-generated destructor stub
}

void Tracer::decodeSystemCall(unsigned char code)
{
	printf("Systemcall %s\n", SYSCALLS_LOOKUP[code]);
}

void Tracer::printError(long int error)
{
	switch(error)
	{
	case EPERM : printf("EPERM: The requested process couldn't be traced. Permission denied.\n");
	break;

	case ESRCH : printf("ESRCH: The requested process doesn't exist or is being traced.\n");
	break;

	case EIO : printf("EIO: The request was invalid or read/write was made from/to invalid area of memory.\n");
	break;

	case EFAULT: printf("EFAULT: Read/write was made from/to memory which was not really mapped.\n");
	break;

	default:
		break;
	};
}

long int Tracer::attach(pid_t pid)
{
	if(m_attached == true)
	{
		detach();
	}

	long int res = ptrace(PT_ATTACH, pid, NULL, NULL);

	if(res == 1)
	{
		printError(errno);
		return -1;
	}

	m_pid = pid;
	m_attached = true;

	return res;
}

long int Tracer::seize(pid_t pid)
{
	if(m_attached == true)
	{
		detach();
	}

	long int res = ptrace(PTRACE_SEIZE, pid, NULL, NULL);

	if(res == 1)
	{
		printError(errno);
		return -1;
	}

	m_pid = pid;
	m_attached = true;

	return res;
}

long int Tracer::detach()
{
	if(m_attached == false)
	{
		printf("ERROR: can not detach before attaching\n");
		return -1;
	}

	long int res = ptrace(PT_DETACH, m_pid, NULL, NULL);

	if(res == 1)
	{
		printError(errno);
		return -1;
	}

	m_pid = 0;
	m_attached = false;

	return res;
}

long int Tracer::interrupt()
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PTRACE_INTERRUPT, m_pid, NULL, NULL);

	return res;
}

long int Tracer::continue_()
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PTRACE_CONT, m_pid, NULL, NULL);

	return res;
}

long int Tracer::kill()
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PTRACE_KILL, m_pid, NULL, NULL);

	return res;
}

long int Tracer::continueToNextSyscall()
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PT_SYSCALL, m_pid, NULL, NULL);

	return res;
}

long int Tracer::singleStep()
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PT_STEP, m_pid, NULL, NULL);

	return res;
}


/**
 * Page-aligned data access.
 * */
long int Tracer::getData(unsigned long long address, bool& error)
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PTRACE_PEEKDATA, m_pid, address, NULL);
	if(res == -1)
	  if(errno)
	  {
		printf("ERROR: could not peek data\n");
		printError(errno);
		error = true;
	    return -1;
	  }

	return res;
}

/**
 * Page-aligned data access.
 * */
long int Tracer::setData(unsigned long long address,  long word)
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PTRACE_POKEDATA, m_pid, address, &word);

	return res;
}


/**
 * Page-aligned data access.
 * */
long int Tracer::getUser(unsigned long long address, bool& error)
{
	//struct user* user_space = (struct user*)0;
	long res = ptrace(PTRACE_PEEKUSER, m_pid, address, NULL);

	if(res == -1)
	  if(errno)
	  {
		printf("ERROR: could not peek user\n");
		printError(errno);
		error = true;
		return -1;
	  }

	return res;
}

/**
 * Page-aligned data access.
 * */
long int Tracer::setUser(unsigned long long address,  long word)
{
	//struct user* user_space = (struct user*)0;
	long res = ptrace(PTRACE_POKEUSER, m_pid, address, &word);

	return res;
}



/**
 * Page-aligned data access.
 * */
long int Tracer::getText(unsigned long long address, bool& error)
{
	if(m_attached == false)
	{
		return -1;
	}

	long int word = ptrace(PTRACE_PEEKTEXT, m_pid, address, NULL);
	if(word == -1)
	  if(errno)
	  {
		printf("ERROR: could not peek text\n");
		printError(errno);
		error = true;
	    return -1;
	  }

	return word;
}

/**
 * Page-aligned data access.
 * */
long int Tracer::setText(unsigned long long address,  long word)
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PTRACE_POKETEXT, m_pid, address, &word);

	return res;
}


struct user_regs_struct* Tracer::getRegs()
{
	if(m_attached == false)
	{
		return NULL;
	}
	struct user_regs_struct* regs = new struct user_regs_struct();

	ptrace(PT_GETREGS, m_pid, NULL, regs);

	return regs;
}


long int Tracer::setRegs(const struct user_regs_struct& regs)
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PT_SETREGS, m_pid, NULL, &regs);

	return res;
}

struct user_fpregs_struct* Tracer::getFPRegs()
{
	if(m_attached == false)
	{
		return NULL;
	}
	struct user_fpregs_struct* regs = new struct user_fpregs_struct();

	ptrace(PT_GETFPREGS, m_pid, NULL, regs);

	return regs;
}


long int Tracer::setFPRegs(const struct user_fpregs_struct& regs)
{
	if(m_attached == false)
	{
		return -1;
	}

	long int res = ptrace(PT_SETFPREGS, m_pid, NULL, &regs);

	return res;
}

/**
 * Page-aligned data access. Returns up to n bytes of read data. An error will be indicated by printing an error message and setting the boolean var.
 * On Linux system data and text memory are being shared.
 * */
long int Tracer::getData(unsigned long long address, const BasicBuffer& data, unsigned long long n, bool& error)
{
	if(m_attached == false || data.get_m_length() < n)
	{
		return -1;
	}

	long int res = 0;

	//copy all whole words
	unsigned char* data_p = data.get_mp_data();
	unsigned int chunks = n / WORD_SIZE_BYTE;
	for(unsigned int i = 0;i<chunks;++i)
	{
		//iterate over the words and read them
		res = ptrace(PTRACE_PEEKDATA, m_pid, address + WORD_SIZE_BYTE*i, NULL);

		if(res == -1)
		  if(errno)
		  {
			printf("ERROR: could not peek data\n");
			printError(errno);
			error = true;
			return -1;
		  }

		memcpy(data_p+WORD_SIZE_BYTE*i,&res,WORD_SIZE_BYTE);
	}

	//copy the remaining word fragment
	if(n % WORD_SIZE_BYTE != 0)//still some chars left to copy
	{
		res = ptrace(PTRACE_PEEKDATA, m_pid, address + WORD_SIZE_BYTE*chunks, NULL);

		if(res == -1)
		  if(errno)
		  {
			printf("ERROR: could not peek data\n");
			printError(errno);
			error = true;
			return -1;
		  }

		memcpy(data_p+WORD_SIZE_BYTE*chunks,&res, WORD_SIZE_BYTE - (n/WORD_SIZE_BYTE) * WORD_SIZE_BYTE );
	}


	return res;
}

/**
 * Page-aligned data access. Sets up to n words.
 * On Linux system data and text memory are being shared.
 * */
long int Tracer::setData(unsigned long long address, const BasicBuffer& data, unsigned long long n)
{
	if(m_attached == false || data.get_m_length() < n)
	{
		return -1;
	}

	long int res = 0;

	//copy all whole words
	unsigned char* data_p = data.get_mp_data();
	unsigned int chunks = n / WORD_SIZE_BYTE;
	for(unsigned int i = 0;i<chunks;++i)
	{
		//iterate over the words and write them
		res = ptrace(PTRACE_POKEDATA, m_pid, address + WORD_SIZE_BYTE*i, data_p+WORD_SIZE_BYTE*i);

		if(res == -1)
		  if(errno)
		  {
			printf("ERROR: could not poke data\n");
			printError(errno);
			return -1;
		  }

	}

	//copy the remaining word fragment
	if(n % WORD_SIZE_BYTE != 0)//still some chars left to copy
	{
		//use the res var as a temporary buffer
		memcpy(&res,data_p+WORD_SIZE_BYTE*chunks, WORD_SIZE_BYTE - (n/WORD_SIZE_BYTE) * WORD_SIZE_BYTE );

		res = ptrace(PTRACE_POKEDATA, m_pid, address + WORD_SIZE_BYTE*chunks, &res);

		if(res == -1)
		  if(errno)
		  {
			printf("ERROR: could not poke data\n");
			printError(errno);
			return -1;
		  }

	}

	return res;
}

const char* Tracer::SYSCALLS_LOOKUP[243] =
{
 "NULL"					  //0
,"exit"                   //1
,"fork"                   //2
,"read"                   //3
,"write"                  //4
,"open"                   //5
,"close"                  //6
,"waitpid"                //7
,"creat"                  //8
,"link"                   //9
,"unlink"                 //10
,"execve"                 //11
,"chdir"                  //12
,"time"                   //13
,"mknod"                  //14
,"chmod"                  //15
,"lchown"                 //16
,"break"                  //17
,"oldstat"                //18
,"lseek"                  //19
,"getpid"                 //20
,"mount"                  //21
,"umount"                 //22
,"setuid"                 //23
,"getuid"                 //24
,"stime"                  //25
,"ptrace"                 //26
,"alarm"                  //27
,"oldfstat"               //28
,"pause"                  //29
,"utime"                  //30
,"stty"                   //31
,"gtty"                   //32
,"access"                 //33
,"nice"                   //34
,"ftime"                  //35
,"sync"                   //36
,"kill"                   //37
,"rename"                 //38
,"mkdir"                  //39
,"rmdir"                  //40
,"dup"                    //41
,"pipe"                   //42
,"times"                  //43
,"prof"                   //44
,"brk"                    //45
,"setgid"                 //46
,"getgid"                 //47
,"signal"                 //48
,"geteuid"                //49
,"getegid"                //50
,"acct"                   //51
,"umount2"                //52
,"lock"                   //53
,"ioctl"                  //54
,"fcntl"                  //55
,"sched_get_priority_min" //160
,"sched_rr_get_interval"  //161
,"nanosleep"              //162
,"mremap"                 //163
,"setresuid"              //164
,"getresuid"              //165
,"vm86"                   //166
,"query_module"           //167
,"poll"                   //168
,"nfsservctl"             //169
,"setresgid"              //170
,"getresgid"              //171
,"prctl"                  //172
,"rt_sigreturn"           //173
,"rt_sigaction"           //174
,"rt_sigprocmask"         //175
,"rt_sigpending"          //176
,"rt_sigtimedwait"        //177
,"rt_sigqueueinfo"        //178
,"rt_sigsuspend"          //179
,"pread"                  //180
,"pwrite"                 //181
,"chown"                  //182
,"getcwd"                 //183
,"capget"                 //184
,"capset"                 //185
,"sigaltstack"            //186
,"sendfile"               //187
,"getpmsg"                //188
,"putpmsg"                //189
,"vfork"                  //190
,"ugetrlimit"             //191
,"mmap2"                  //192
,"truncate64"             //193
,"ftruncate64"            //194
,"stat64"                 //195
,"lstat64"                //196
,"fstat64"                //197
,"lchown32"               //198
,"getuid32"               //199
,"getgid32"               //200
,"geteuid32"              //201
,"getegid32"              //202
,"setreuid32"             //203
,"setregid32"             //204
,"getgroups32"            //205
,"setgroups32"            //206
,"fchown32"               //207
,"setresuid32"            //208
,"getresuid32"            //209
,"setresgid32"            //210
,"getresgid32"            //211
,"chown32"                //212
,"setuid32"               //213
,"setgid32"               //214
,"setfsuid32"             //215
,"setfsgid32"             //216
,"pivot_root"             //217
,"mincore"                //218
,"madvise"                //219
,"getdents64"             //220
,"fcntl64"                //221
,"<none>"                 //222
,"security"               //223
,"gettid"                 //224
,"readahead"              //225
,"setxattr"               //226
,"lsetxattr"              //227
,"fsetxattr"              //228
,"getxattr"               //229
,"lgetxattr"              //230
,"fgetxattr"              //231
,"listxattr"              //232
,"llistxattr"             //233
,"flistxattr"             //234
,"removexattr"            //235
,"lremovexattr"           //236
,"fremovexattr"           //237
,"tkill"                  //238
,"sendfile64"             //239
,"futex"                  //240
,"sched_setaffinity"      //241
,"sched_getaffinity"      //242
} ;


} /* namespace Lazarus */
