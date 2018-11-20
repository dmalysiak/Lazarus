// system includes
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string>
#include <string.h>
#include <omp.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <errno.h>

#include <bitset>

// project includes
#include "Toolbox.h"


namespace Lazarus
{


Testcallee::Testcallee(unsigned int i)
{
	m_i=i;
}

void Testcallee::p(Thread* t,void* param)
{
	printf("my var is %d\n",m_i);
}

//hack
const char* byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}


/**
 * This function reads a CSV token (i.e. value) from the current position within the open file.
 * It is assumed that the file contains the following format:
 * VAL<SEP>VAL<SEP>VAL<TER><LBREAK>
 * The value is inserted into the given string.
 * The function returns 0 in case of a complete value without hitting a TER symbol,
 * 1 in case of hitting a TER symbol, 2 in case of hitting EOF, -1 else
 * */
int Toolbox::readCSVValue(FILE** pFile, std::string& s,char sep,char ter, bool linebreaks_after_terminator)
{
	int c;
	//s = ""; //reset string

	//read chars until ,  ; or EOF found
	while(1){
		c = fgetc(*pFile);
		if(c==sep) return 0;
		if(c==ter)
		{
			if(linebreaks_after_terminator == true)
			{
				fgetc(*pFile); //read ahead one char, i.e. skip line break. Uncomment this if linebreaks are not being used after ;
			}

			return 1;
		}
		if(c==EOF)
		{
			return 2;
		}

		#ifdef DEBUG
		printf("appending %s to %s \n",(char*)&c,s.c_str());
		#endif
		s.append((char*)&c);
	}

	return -1;
}

Toolbox::Toolbox() {

}

Toolbox::~Toolbox() {
	// TODO Auto-generated destructor stub
}

void Toolbox::decodeErrno(int error)
{
	printf("ERROR: %s\n", ERRNO_LOOKUP[error]);

	switch(error)
	{
	case E2BIG: printf("Argument list too long (POSIX.1)\n");break;

	case EACCES: printf("Permission denied (POSIX.1)\n");break;

	case EADDRINUSE: printf("Address already in use (POSIX.1)\n");break;

	case EADDRNOTAVAIL: printf("Address not available (POSIX.1)\n");break;

	case EAFNOSUPPORT: printf("Address family not supported (POSIX.1)\n");break;

	case EAGAIN: printf("Resource temporarily unavailable (may be the same value as EWOULDBLOCK) (POSIX.1)\n");break;

	case EALREADY: printf("Connection already in progress (POSIX.1)\n");break;

	case EBADE: printf("Invalid exchange\n");break;

	case EBADF: printf("Bad file descriptor (POSIX.1)\n");break;

	case EBADFD: printf("File descriptor in bad state\n");break;

	case EBADMSG: printf("Bad message (POSIX.1)\n");break;

	case EBADR: printf("Invalid request descriptor\n");break;

	case EBADRQC: printf("Invalid request code\n");break;

	case EBADSLT: printf("Invalid slot\n");break;

	case EBUSY: printf("Device or resource busy (POSIX.1)\n");break;

	case ECANCELED: printf("Operation canceled (POSIX.1)\n");break;

	case ECHILD: printf("No child processes (POSIX.1)\n");break;

	case ECHRNG: printf("Channel number out of range\n");break;

	case ECOMM: printf("Communication error on send\n");break;

	case ECONNABORTED: printf("Connection aborted (POSIX.1)\n");break;

	case ECONNREFUSED: printf("Connection refused (POSIX.1)\n");break;

	case ECONNRESET: printf("Connection reset (POSIX.1)\n");break;

	case EDEADLK: printf("Resource deadlock avoided (POSIX.1)\n");break;

	//case EDEADLOCK: printf("Synonym for EDEADLK\n");break;

	case EDESTADDRREQ: printf("Destination address required (POSIX.1)\n");break;

	case EDOM: printf("Mathematics argument out of domain of function (POSIX.1, C99)\n");break;

	case EDQUOT: printf("Disk quota exceeded (POSIX.1)\n");break;

	case EEXIST: printf("File exists (POSIX.1)\n");break;

	case EFAULT: printf("Bad address (POSIX.1)\n");break;

	case EFBIG: printf("File too large (POSIX.1)\n");break;

	case EHOSTDOWN: printf("Host is down\n");break;

	case EHOSTUNREACH: printf("Host is unreachable (POSIX.1)\n");break;

	case EIDRM: printf("Identifier removed (POSIX.1)\n");break;

	case EILSEQ: printf("Illegal byte sequence (POSIX.1, C99)\n");break;

	case EINPROGRESS: printf("Operation in progress (POSIX.1)\n");break;

	case EINTR: printf("Interrupted function call (POSIX.1); see signal(7).\n");break;

	case EINVAL: printf("Invalid argument (POSIX.1)\n");break;

	case EIO: printf("Input/output error (POSIX.1)\n");break;

	case EISCONN: printf("Socket is connected (POSIX.1)\n");break;

	case EISDIR: printf("Is a directory (POSIX.1)\n");break;

	case EISNAM: printf("Is a named type file\n");break;

	case EKEYEXPIRED: printf("Key has expired\n");break;

	case EKEYREJECTED: printf("Key was rejected by service\n");break;

	case EKEYREVOKED: printf("Key has been revoked\n");break;

	case EL2HLT: printf("Level 2 halted\n");break;

	case EL2NSYNC: printf("Level 2 not synchronized\n");break;

	case EL3HLT: printf("Level 3 halted\n");break;

	case EL3RST: printf("Level 3 halted\n");break;

	case ELIBACC: printf("Cannot access a needed shared library\n");break;

	case ELIBBAD: printf("Accessing a corrupted shared library\n");break;

	case ELIBMAX: printf("Attempting to link in too many shared libraries\n");break;

	case ELIBSCN: printf("lib section in a.out corrupted\n");break;

	case ELIBEXEC: printf("Cannot exec a shared library directly\n");break;

	case ELOOP: printf("Too many levels of symbolic links (POSIX.1)\n");break;

	case EMEDIUMTYPE: printf("Wrong medium type\n");break;

	case EMFILE: printf("Too many open files (POSIX.1); commonly caused by exceeding the RLIMIT_NOFILE resource limit described in getrlimit(2)\n");break;

	case EMLINK: printf("Too many links (POSIX.1)\n");break;

	case EMSGSIZE: printf("Message too long (POSIX.1)\n");break;

	case EMULTIHOP: printf("Multihop attempted (POSIX.1)\n");break;

	case ENAMETOOLONG: printf("Filename too long (POSIX.1)\n");break;

	case ENETDOWN: printf("Network is down (POSIX.1)\n");break;

	case ENETRESET: printf("Connection aborted by network (POSIX.1)\n");break;

	case ENETUNREACH: printf("Network unreachable (POSIX.1)\n");break;

	case ENFILE: printf("Too many open files in system (POSIX.1); on Linux, this is probably a result of encountering the /proc/sys/fs/file-max limit (see proc(5)).\n");break;

	case ENOBUFS: printf("No buffer space available (POSIX.1 (XSI STREAMS option))\n");break;

	case ENODATA: printf("No message is available on the STREAM head read queue  (POSIX.1)\n");break;

	case ENODEV: printf("No such device (POSIX.1)\n");break;

	case ENOENT: printf("No such file or directory (POSIX.1)\n");break;

	case ENOEXEC: printf("Exec format error (POSIX.1)\n");break;

	case ENOKEY: printf("Required key not available\n");break;

	case ENOLCK: printf("No locks available (POSIX.1)\n");break;

	case ENOLINK: printf("Link has been severed (POSIX.1)\n");break;

	case ENOMEDIUM: printf("No medium found\n");break;

	case ENOMEM: printf("Not enough space (POSIX.1)\n");break;

	case ENOMSG: printf("No message of the desired type (POSIX.1)\n");break;

	case ENONET: printf("Machine is not on the network\n");break;

	case ENOPKG: printf("Package not installed\n");break;

	case ENOPROTOOPT: printf("Protocol not available (POSIX.1)\n");break;

	case ENOSPC: printf("No space left on device (POSIX.1)\n");break;

	case ENOSR: printf("No STREAM resources (POSIX.1 (XSI STREAMS option))\n");break;

	case ENOSTR: printf("Not a STREAM (POSIX.1 (XSI STREAMS option))\n");break;

	case ENOSYS: printf("Function not implemented (POSIX.1)\n");break;

	case ENOTBLK: printf("Block device required\n");break;

	case ENOTCONN: printf("The socket is not connected (POSIX.1)\n");break;

	case ENOTDIR: printf("Not a directory (POSIX.1)\n");break;

	case ENOTEMPTY: printf("Directory not empty (POSIX.1)\n");break;

	case ENOTSOCK: printf("Not a socket (POSIX.1)\n");break;

	case ENOTSUP: printf("Operation not supported (POSIX.1)\n");break;

	case ENOTTY: printf("Inappropriate I/O control operation (POSIX.1)\n");break;

	case ENOTUNIQ: printf("Name not unique on network\n");break;

	case ENXIO: printf("No such device or address (POSIX.1)\n");break;

	//case EOPNOTSUPP: printf("Operation not supported on socket (POSIX.1)\n");break;

	case EOVERFLOW: printf("Value too large to be stored in data type (POSIX.1)\n");break;

	case EPERM: printf("Operation not permitted (POSIX.1)\n");break;

	case EPFNOSUPPORT: printf("Protocol family not supported\n");break;

	case EPIPE: printf("Broken pipe (POSIX.1)\n");break;

	case EPROTO: printf("Protocol error (POSIX.1)\n");break;

	case EPROTONOSUPPORT: printf("Protocol not supported (POSIX.1)\n");break;

	case EPROTOTYPE: printf("Protocol wrong type for socket (POSIX.1)\n");break;

	case ERANGE: printf("Result too large (POSIX.1, C99)\n");break;

	case EREMCHG: printf("Remote address changed\n");break;

	case EREMOTE: printf("Object is remote\n");break;

	case EREMOTEIO: printf("Remote I/O error\n");break;

	case ERESTART: printf("Interrupted system call should be restarted\n");break;

	case EROFS: printf("Read-only filesystem (POSIX.1)\n");break;

	case ESHUTDOWN: printf("Cannot send after transport endpoint shutdown\n");break;

	case ESPIPE: printf("Invalid seek (POSIX.1)\n");break;

	case ESOCKTNOSUPPORT: printf("Socket type not supported\n");break;

	case ESRCH: printf("No such process (POSIX.1)\n");break;

	case ESTALE: printf("Stale file handle (POSIX.1)\n");break;

	case ESTRPIPE: printf("Streams pipe error\n");break;

	case ETIME: printf("Timer expired (POSIX.1 (XSI STREAMS option))  (POSIX.1 says 'STREAM ioctl(2) timeout')\n");break;

	case ETIMEDOUT: printf("Connection timed out (POSIX.1)\n");break;

	case ETXTBSY: printf("Text file busy (POSIX.1)\n");break;

	case EUCLEAN: printf("Structure needs cleaning\n");break;

	case EUNATCH: printf("Protocol driver not attached\n");break;

	case EUSERS: printf("Too many users\n");break;

	//case EWOULDBLOCK: printf("Operation would block (may be same value as EAGAIN) (POSIX.1)\n");break;

	case EXDEV: printf("Improper link (POSIX.1)\n");break;

	case EXFULL: printf("Exchange full\n");break;

	default:
		printf("Unknown errno error\n");
		break;
	};

}

std::string Toolbox::ensureValidPath(const std::string& s)
{
	char buffer[s.size()+2];//must be large enough for the input string + '/' + '\0'
	unsigned int c = 0;

	for(unsigned int i=0;i<s.size();i++)
	{
		//skip any double slashes
		if( i < s.size()-1)
			if(s[i] == '/' && s[i+1] == '/')
				continue;

		buffer[c] = s[i];
		c++;
	}

	//check if the last inserted element does not equal '/'
	if(buffer[c-1] != '/')
	{
		buffer[c] = '/';
		c++;
	}

	buffer[c] = '\0';

	return std::string(buffer);
}

void Toolbox::assertInteger(double val)
{
	assert( val - ceil(val) == 0.0 );
}

void Toolbox::assertInteger(float val)
{
	assert( val - ceil(val) == 0.0f );
}

void Toolbox::assertDivisible(unsigned int a, unsigned int b)
{
	assert( a % b == 0 );
}

void Toolbox::exitThread(void* data)
{
	pthread_exit(NULL);
	return;
}

void Toolbox::dummyThreadTask(Thread*, void* data)
{
	printf("DUMMY TASK\n");
	return;
}

short int Toolbox::swapShortInt(short int i)
{
	if(sizeof(short int) != 2)
	{
		printf("ERROR: could not swap bytes of short int var, size is not 2 bytes: %lu bytes\n", sizeof(short int));
		return 0;
	}

	return (short int)( (i & 0x00FF)<<8 | (i & 0xFF00)>>8 );
}

short unsigned int Toolbox::swapShortInt(short unsigned int i)
{
	if(sizeof(unsigned short int) != 2)
	{
		printf("ERROR: could not swap bytes of unsigned short int var, size is not 2 bytes: %lu bytes\n", sizeof(unsigned short int));
		return 0;
	}

	return (unsigned short int)( (i & 0x00FF)<<8 | (i & 0xFF00)>>8 );
}

int Toolbox::swapInt(int i)
{
	if(sizeof(int) != 4)
	{
		printf("ERROR: could not swap bytes of int var, size is not 4 bytes: %lu bytes\n", sizeof(int));
		return 0;
	}

	return (int)( (i & 0xFF000000) >> 24 | (i & 0x00FF0000)>>8 | (i & 0x0000FF00)<<8 | (i & 0x000000FF)<<24 );
}

unsigned int Toolbox::swapUInt(unsigned int i)
{
	if(sizeof(unsigned int) != 4)
	{
		printf("ERROR: could not swap bytes of unsigned int var, size is not 4 bytes: %lu bytes\n", sizeof(unsigned int));
		return 0;
	}

	return (unsigned int)( (i & 0xFF000000) >> 24 | (i & 0x00FF0000)>>8 | (i & 0x0000FF00)<<8 | (i & 0x000000FF)<<24 );
}

long int Toolbox::swapLongInt(long int i)
{
	if(sizeof(unsigned long int) != 8 && sizeof(unsigned long int) != 4)
	{
		printf("ERROR: could not swap bytes of long int var, size is not 8 or 4 bytes: %lu bytes\n", sizeof(long int));
		return 0;
	}

	if(sizeof(long int)==4)
	{
		return (long int)swapInt(i);
	}
	else
	{
		return (long int)( (i & 0xFF00000000000000)>>56 | (i & 0x00FF000000000000)>>40 | (i & 0x0000FF0000000000)>>24 | (i<<32 & 0x000000FF00000000)>>8 |
				(i & 0x00000000FF000000)<<8 | (i & 0x0000000000FF0000)<<24 | (i & 0x000000000000FF00)<<40 | (i<<32 & 0x00000000000000FF)<<56 );
	}

	//this should not happen
	return 0;
}

unsigned long int Toolbox::swapULongInt(unsigned long int i)
{
	if(sizeof(unsigned long int) != 8 && sizeof(unsigned long int) != 4)
	{
		printf("ERROR: could not swap bytes of unsigned long int var, size is not 8 or 4 bytes: %lu bytes\n", sizeof(unsigned long int));
		return 0;
	}

	if(sizeof(unsigned long int)==4)
	{
		return (unsigned long int)swapUInt(i);
	}
	else
	{
		return (unsigned long int)( (i & 0xFF00000000000000)>>56 | (i & 0x00FF000000000000)>>40 | (i & 0x0000FF0000000000)>>24 | (i<<32 & 0x000000FF00000000)>>8 |
				(i & 0x00000000FF000000)<<8 | (i & 0x0000000000FF0000)<<24 | (i & 0x000000000000FF00)<<40 | (i<<32 & 0x00000000000000FF)<<56 );
	}

	//this should not happen
	return 0;
}

long long int Toolbox::swapLongLongInt(long long int i)
{
	if(sizeof(long long int) != 8)
	{
		printf("ERROR: could not swap bytes of longlong var, size is not 8 bytes: %lu bytes\n", sizeof(long long int));
		return 0;
	}

	return (long int)( (i & 0xFF00000000000000)>>56 | (i & 0x00FF000000000000)>>40 | (i & 0x0000FF0000000000)>>24 | (i<<32 & 0x000000FF00000000)>>8 |
				(i & 0x00000000FF000000)<<8 | (i & 0x0000000000FF0000)<<24 | (i & 0x000000000000FF00)<<40 | (i<<32 & 0x00000000000000FF)<<56 );
}

unsigned long long int Toolbox::swapULongLongInt(unsigned long long int i)
{
	if(sizeof(unsigned long long int) != 8)
	{
		printf("ERROR: could not swap bytes of ulonglong var, size is not 8 bytes: %lu bytes\n", sizeof(unsigned long long int));
		return 0;
	}

	return (unsigned long int)( (i & 0xFF00000000000000)>>56 | (i & 0x00FF000000000000)>>40 | (i & 0x0000FF0000000000)>>24 | (i<<32 & 0x000000FF00000000)>>8 |
					(i & 0x00000000FF000000)<<8 | (i & 0x0000000000FF0000)<<24 | (i & 0x000000000000FF00)<<40 | (i<<32 & 0x00000000000000FF)<<56 );
}

double Toolbox::swapDouble(double d)
{
	if(sizeof(float) != 8)
	{
		printf("ERROR: could not swap bytes of double var, size is not 8 bytes: %lu bytes\n", sizeof(double));
		return 0;
	}

	unsigned long long int* i = (unsigned long long int*)&d;
	unsigned long long int res = (unsigned long long int)
			( (*i & 0xFF00000000000000)>>56 | (*i & 0x00FF000000000000)>>40 | (*i & 0x0000FF0000000000)>>24 | (*i<<32 & 0x000000FF00000000)>>8 |
				(*i & 0x00000000FF000000)<<8 | (*i & 0x0000000000FF0000)<<24 | (*i & 0x000000000000FF00)<<40 | (*i<<32 & 0x00000000000000FF)<<56 );

	memcpy(&d,&res,sizeof(double));

	return d;
}

float Toolbox::swapFloat(float d)
{
	if(sizeof(float) != 4)
	{
		printf("ERROR: could not swap bytes of float var, size is not 4 bytes: %lu bytes\n", sizeof(float));
		return 0;
	}

	unsigned long long int* i = (unsigned long long int*)&d;
	unsigned int res = (unsigned int)( (*i & 0xFF000000) >> 24 | (*i & 0x00FF0000)>>8 | (*i & 0x0000FF00)<<8 | (*i & 0x000000FF)<<24 );

	memcpy(&d,&res,sizeof(float));

	return d;
}


int Toolbox::createEmptyFile(const std::string& file, unsigned long long count, const std::string unit)
{
	std::string command;
	command = std::string("dd if=/dev/zero of=") + file + std::string("  bs=") + unit + std::string("  count=")+Toolbox::ulongLongToString(count);
	return system(command.c_str());
}

int Toolbox::createRandomFile(const std::string& file, unsigned long long count, const std::string unit)
{
	std::string command;
	command = std::string("dd if=/dev/urandom of=") + file + std::string("  bs=") + unit + std::string("  count=")+Toolbox::ulongLongToString(count);
	return system(command.c_str());
}

char* Toolbox::floatToCString(float value, unsigned int digits)
{
    char* output = new char[m_buffer_size];
    sprintf(output, "%.*f",digits, value);
    return output;
}

char* Toolbox::doubleToCString(double value, unsigned int digits)
{
    char* output = new char[m_buffer_size];
    sprintf(output, "%.*f",digits, value);
    return output;
}

char* Toolbox::longDoubleToCString(long double value, unsigned int digits)
{
    char* output = new char[m_buffer_size];
    sprintf(output, "%.*Lf",digits, value);
    return output;
}

char* Toolbox::intToCString(int value)
{
    char* output = new char[m_buffer_size];
    sprintf(output, "%d", value);
    return output;
}

char* Toolbox::longToCString(long value)
{
    char* output = new char[m_buffer_size];
    sprintf(output, "%ld", value);
    return output;
}

double Toolbox::cStringToDouble(char* s)
{
    return strtod( s, NULL );
}

long double Toolbox::cStringToLongDouble(char* s)
{
    return strtold( s, NULL );
}

int Toolbox::cStringToInt(char* s)
{
    return strtol(s, NULL,10);
}

long int Toolbox::cStringToLongInt(char* s)
{
    return strtol(s, NULL,10);
}

long long int Toolbox::cStringToLongLongInt(char* s)
{
    return strtoll(s, NULL,10);
}

unsigned int Toolbox::getExpMetricUInt(const std::string& s)
{
	unsigned int res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToUInt(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToUInt(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToUInt(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToUInt(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToUInt(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToUInt(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToUInt(s);

	return res;
}


int Toolbox::getExpMetricInt(const std::string& s)
{
	int res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToInt(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToInt(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToInt(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToInt(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToInt(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToInt(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToInt(s);

	return res;
}



unsigned long Toolbox::getExpMetricULong(const std::string& s)
{
	unsigned long res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToULong(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToULong(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToULong(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToULong(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToULong(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToULong(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToULong(s);

	return res;
}


long Toolbox::getExpMetricLong(const std::string& s)
{
	long res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToLong(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToLong(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToLong(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToLong(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToLong(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToLong(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToLong(s);

	return res;
}



unsigned long long Toolbox::getExpMetricULongLong(const std::string& s)
{
	unsigned long long res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToULongLong(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToULongLong(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToULongLong(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToULongLong(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToULongLong(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToULongLong(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToULongLong(s);

	return res;
}


long long Toolbox::getExpMetricLongLong(const std::string& s)
{
	long long res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToLongLong(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToLongLong(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToLongLong(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToLongLong(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToLongLong(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToLongLong(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToLongLong(s);

	return res;
}



unsigned short Toolbox::getExpMetricUShort(const std::string& s)
{
	unsigned short res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToUShort(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToUShort(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToUShort(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToUShort(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToUShort(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToUShort(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToUShort(s);

	return res;
}


short Toolbox::getExpMetricShort(const std::string& s)
{
	short res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToShort(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToShort(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToShort(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToShort(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToShort(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToShort(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToShort(s);

	return res;
}



float Toolbox::getExpMetricFloat(const std::string& s)
{
	float res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToFloat(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToFloat(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToFloat(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToFloat(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToFloat(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToFloat(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToULong(s);

	return res;
}


double Toolbox::getExpMetricDouble(const std::string& s)
{
	double res = 0;

	//check if last character is not a number
	char c = *(s.substr(s.size()-1,1).c_str());
	if(c > 57)
	{
		std::string sub = s.substr(0,s.size()-1);

		switch(c)
		{
		case 'k':
		case 'K':
			res = stringToDouble(sub)*1000;
			break;

		case 'm':
		case 'M':
			res = stringToDouble(sub)*1000000;
			break;

		case 'g':
		case 'G':
			res = stringToDouble(sub)*1000000000L;
			break;

		case 't':
		case 'T':
			res = stringToDouble(sub)*1000000000000L;
			break;

		case 'p':
		case 'P':
			res = stringToDouble(sub)*1000000000000000L;
			break;

		case 'e':
		case 'E':
			res = stringToDouble(sub)*1000000000000000000L;
			break;
		}
	}
	else
		res = stringToLong(s);

	return res;
}


std::string Toolbox::doubleToString(double d, unsigned int digits)
{
	char buffer[40];
	sprintf(buffer, "%.*f",digits, d);
	return std::string(buffer);
}

std::string Toolbox::longDoubleToString(long double d, unsigned int digits)
{
	char buffer[90];
	sprintf(buffer, "%.*Lf",digits, d);
	return std::string(buffer);
}

std::string Toolbox::floatToString(float d, unsigned int digits)
{
	char buffer[40];
	sprintf(buffer, "%.*f",digits, d);
	return std::string(buffer);
}

std::string Toolbox::uintToString(unsigned int d)
{
	char buffer[40];
	sprintf(buffer, CSV_UINT_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::ushortToString(unsigned short d)
{
	char buffer[40];
	sprintf(buffer, CSV_USHORT_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::longToString(long d)
{
	char buffer[40];
	sprintf(buffer, CSV_LONG_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::longLongToString(long long d)
{
	char buffer[40];
	sprintf(buffer, CSV_LONGLONG_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::ulongToString(unsigned long d)
{
	char buffer[40];
	sprintf(buffer, CSV_ULONG_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::ulongLongToString(unsigned long long d)
{
	char buffer[40];
	sprintf(buffer, CSV_ULONGLONG_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::intToString(int d)
{
	char buffer[40];
	sprintf(buffer, CSV_INT_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::shortToString(short d)
{
	char buffer[40];
	sprintf(buffer, CSV_SHORT_FORMAT, d);
	return std::string(buffer);
}

std::string Toolbox::size_tToString(size_t d)
{
	char buffer[40];
	sprintf(buffer, CSV_SIZE_T_FORMAT, d);
	return std::string(buffer);
}


long double Toolbox::stringToLongDouble(const std::string& s)
{
	long double val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

double Toolbox::stringToDouble(const std::string& s)
{
	double val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}


float Toolbox::stringToFloat(const std::string& s)
{
	float val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

unsigned int Toolbox::stringToUInt(const std::string& s)
{
	unsigned int val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

unsigned short Toolbox::stringToUShort(const std::string& s)
{
	unsigned short val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

long Toolbox::stringToLong(const std::string& s)
{
	long val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

long long Toolbox::stringToLongLong(const std::string& s)
{
	long long val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

unsigned long Toolbox::stringToULong(const std::string& s)
{
	unsigned long val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

unsigned long long Toolbox::stringToULongLong(const std::string& s)
{
	unsigned long long val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

int Toolbox::stringToInt(const std::string& s)
{
	int val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

short Toolbox::stringToShort(const std::string& s)
{
	short val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

size_t Toolbox::stringTosize_t(const std::string& s)
{
	size_t val = 0.0;
	std::stringstream ss;
	ss << s;
	ss >> val;
	return val;
}

unsigned char Toolbox::stringToUCharB(const std::string& s, int base)
{
	return strtoul( s.c_str(),NULL,base );
}

char Toolbox::stringToCharB(const std::string& s, int base)
{
	return strtol( s.c_str(),NULL,base );
}

unsigned int Toolbox::stringToUIntB(const std::string& s, int base)
{
	return strtoul( s.c_str(),NULL,base );
}

long Toolbox::stringToLongB(const std::string& s, int base)
{
	return strtol( s.c_str(),NULL,base );
}

long long Toolbox::stringToLongLongB(const std::string& s, int base)
{
	return strtoll( s.c_str(),NULL,base );
}

unsigned long Toolbox::stringToULongB(const std::string& s, int base)
{
	return strtoul( s.c_str(),NULL,base );
}

unsigned long long Toolbox::stringToULongLongB(const std::string& s, int base)
{
	return strtoull( s.c_str(),NULL,base );
}

int Toolbox::stringToIntB(const std::string& s, int base)
{
	return strtol( s.c_str(),NULL,base );
}

size_t Toolbox::stringTosize_tB(const std::string& s, int base)
{
	return strtoull( s.c_str(),NULL,base );
}

std::string Toolbox::ucharToStringB(unsigned char d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 8 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_UCHAR_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_UCHAR_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_UCHAR_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_UCHAR_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::charToStringB(char d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 8 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_CHAR_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_CHAR_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_CHAR_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_CHAR_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}


std::string Toolbox::uintToStringB(unsigned int d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 32 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_UINT_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_UINT_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_UINT_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_UINT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::longToStringB(long d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 32 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_LONG_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_LONG_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_LONG_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_INT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::longLongToStringB(long long d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 64 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_LONGLONG_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_LONGLONG_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_LONGLONG_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_INT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::ulongToStringB(unsigned long d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 32 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_ULONG_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_ULONG_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_ULONG_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_INT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::ulongLongToStringB(unsigned long long d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 64 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_ULONGLONG_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_ULONGLONG_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_ULONGLONG_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_INT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::intToStringB(int d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 32 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_INT_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_INT_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_INT_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_INT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

std::string Toolbox::size_tToStringB(size_t d, int base)
{
	char buffer[40];

	switch(base)
	{
	case 2:
		{
			std::string s = std::bitset< 64 >( d ).to_string(); // string conversion
			return s;
		}
	case 8:
		sprintf(buffer, CSV_ULONG_FORMAT_OCT, d);
		break;

	case 10:
		sprintf(buffer, CSV_ULONG_FORMAT, d);
		break;

	case 16:
		sprintf(buffer, CSV_ULONG_FORMAT_HEX, d);
		break;

	default:
		printf("ERROR: unsupported base\n");
		sprintf(buffer, CSV_INT_FORMAT, 0);
		break;

	}

	return std::string(buffer);
}

//counts the number of 'a'-occurrences in s
unsigned int Toolbox::countChar(const std::string& s, char a)
{
	unsigned int counter = 0;
	const char* c_str = s.c_str();

	while(*c_str != '\0')
	{
		if(*c_str == a)
		{
			counter++;
		}

		c_str++;
	}

	return counter;
}

unsigned int Toolbox::countLineBreaks(const std::string& s)
{
	return countChar(s,'\n');
}

long long int Toolbox::getFileSize(const std::string& file)
{
	FILE* fp = fopen64(file.c_str(),"rb");

	if(fp == NULL)
	{
		return -1;
	}

	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);

	fclose(fp);

	return size;
}

long long int Toolbox::getFileSize(FILE* fp)
{
	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);
	return size;
}

bool Toolbox::checkFileExistence(const std::string& file)
{
	if( access( file.c_str(), F_OK ) != -1 )
	{
	    return true;
	}

	return false;
}

int Toolbox::writeArrayToFile(const std::string& filename, unsigned char* buffer, unsigned long long size)
{
	FILE* fp = fopen64(filename.c_str(),"wb");

	if(fp == NULL)
	{
		printf("ERROR: could not open file for writing\n");
		return -1;
	}

	unsigned long long int res = fwrite(buffer,sizeof(unsigned char),size,fp);

	fclose(fp);

	if(res != size)
	{
		printf("ERROR: could not write all data\n");
		return -1;
	}

	return 0;

	/*
	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == true)
	{
		f.write((char*)buffer,size);

		f.close();
	}
	else
	{
		printf("Error: file not found\n");
		return -1;
	}
	return 0;
	*/

}

int Toolbox::writeArrayToTextFile(const std::string& filename, unsigned char* buffer, unsigned long long size)
{
	FILE* fp = fopen64(filename.c_str(),"w");

	if(fp == NULL)
	{
		printf("ERROR: could not open file for writing\n");
		return -1;
	}

	unsigned long long int res = fwrite(buffer,sizeof(unsigned char),size,fp);

	fclose(fp);

	if(res != size)
	{
		printf("ERROR: could not write all data\n");
		return -1;
	}

	return 0;

	/*std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::trunc));

	if(f.is_open() == true)
	{
		f.write((char*)buffer,size);

		f.close();
	}
	else
	{
		printf("Error: file not found\n");
		return -1;
	}

	return 0;*/
}

int Toolbox::appendArrayToFile(const std::string& filename, unsigned char* buffer, unsigned long long size)
{
	FILE* fp = fopen64(filename.c_str(),"ab");

	if(fp == NULL)
	{
		printf("ERROR: could not open file for writing\n");
		return -1;
	}

	unsigned long long int res = fwrite(buffer,sizeof(unsigned char),size,fp);

	fclose(fp);

	if(res != size)
	{
		printf("ERROR: could not write all data\n");
		return -1;
	}

	return 0;

	/*
	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == true)
	{
		f.write((char*)buffer,size);

		f.close();
	}
	else
	{
		printf("Error: file not found\n");
		return -1;
	}
	return 0;
	*/

}

int Toolbox::appendArrayToTextFile(const std::string& filename, unsigned char* buffer, unsigned long long size)
{
	FILE* fp = fopen64(filename.c_str(),"a");

	if(fp == NULL)
	{
		printf("ERROR: could not open file for writing\n");
		return -1;
	}

	unsigned long long int res = fwrite(buffer,sizeof(unsigned char),size,fp);

	fclose(fp);

	if(res != size)
	{
		printf("ERROR: could not write all data\n");
		return -1;
	}

	return 0;

	/*std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::trunc));

	if(f.is_open() == true)
	{
		f.write((char*)buffer,size);

		f.close();
	}
	else
	{
		printf("Error: file not found\n");
		return -1;
	}

	return 0;*/
}

int Toolbox::readLinesToTuple(const std::string& filename, FastKTuple<std::string>& data)
{
	setlocale(LC_ALL,"C");

	FILE* pFile = fopen64(filename.c_str(),"r");
	std::string s;

	s = readFileToString(filename);
	data.clear();

	int size = s.size();
	int lastpos = 0;
	const char* ss = s.c_str();

	//iterate over string until a '\n' occurs
	for(int i=0;i<size;++i)
	{
		if(ss[i] < '\n' )
		{
			//this effectively skips empty lines
			if(size-1 - lastpos > 0)
			{
				//size-1 in order to exclude the line break
				data.push( s.substr(lastpos,size-1 - lastpos) );
			}
			lastpos = size+1;//new offset right after the current line break

		}
	}

	fclose(pFile);

	return 0;
}

int Toolbox::readFileToArray(FILE* file, unsigned char* buffer, unsigned long long bytes, unsigned long long offset)
{
	//get to the offset
	fseeko64(file, offset, SEEK_SET);

	unsigned long long int res = fread(buffer,sizeof(unsigned char),bytes,file);

	//get back to the beginning
	fseeko64(file, 0L, SEEK_SET);

	if(res != bytes)
	{
		return -1;
	}

	return 0;
}

int Toolbox::writeArrayToFile(FILE* file, unsigned char* buffer, unsigned long long size)
{
	if(file == NULL)
	{
		return -1;
	}

	unsigned long long int res = fwrite(buffer,sizeof(unsigned char),size,file);

	if(res != size)
	{
		printf("ERROR: could not write all data\n");
		return -1;
	}

	return 0;
}




//splits the string with delimiter 'a', returns a pointer to a std::vector which contains all substrings
Lazarus::NCSList<std::string>* Toolbox::splitString(const std::string &s, char a)
{
	unsigned int delimiter_index = 0;
	const char* c_str = s.c_str();
	Lazarus::NCSList<std::string>* sub_strings = new Lazarus::NCSList<std::string>();


	unsigned int last_delimiter = 0;

	while(*c_str != '\0')
	{
		if(*c_str == a)
		{

			sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );

			last_delimiter = delimiter_index+1;

		}

		++delimiter_index;
		++c_str;
	}

	//and the last one
	sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );


	return sub_strings;
}

//splits the string with delimiter 'a', returns a pointer to a std::vector which contains all substrings
Lazarus::FastNCSList<std::string>* Toolbox::splitStringF(const std::string &s, char a)
{
	unsigned int delimiter_index = 0;
	const char* c_str = s.c_str();
	Lazarus::FastNCSList<std::string>* sub_strings = new Lazarus::FastNCSList<std::string>();


	unsigned int last_delimiter = 0;

	while(*c_str != '\0')
	{
		if(*c_str == a)
		{

			sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );

			last_delimiter = delimiter_index+1;

		}

		++delimiter_index;
		++c_str;
	}

	//and the last one
	sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );


	return sub_strings;
}


//splits the string with delimiter 'a', returns a pointer to a std::vector which contains all substrings
Lazarus::NCSList<std::string>* Toolbox::splitStringSegment(const std::string &s, char a)
{
	unsigned int delimiter_index = 0;
	const char* c_str = s.c_str();
	bool first_character = true;
	Lazarus::NCSList<std::string>* sub_strings = new Lazarus::NCSList<std::string>();

	bool segment_open = false;

	unsigned int last_delimiter = 0;

	while(*c_str != '\0')
	{
		//the end of a segment
		if(*c_str == '"' && segment_open==true)
		{
			//check if the previous char was a '\',
			//since the segment is open; there is a previous char!
			//if it wasn't; close the segment
			if(*(c_str-1) != '\\')
			{
				segment_open = false;
				++delimiter_index;
				++c_str;
				continue;
			}

		}

		//the beginning of a segment
		if(*c_str == '"' && segment_open==false)
		{
			//check if the previous char was a '\',
			//since this might have been the first character, there couln't be a previous character.
			//if it wasn't; open the segment
			if(first_character == false)
			{
				if(*(c_str-1) != '\\')
				{
					segment_open = true;
				}
			}
			else//if its the first character: open the segment
			{
				segment_open = true;
			}
		}

		//normal parsing outside an open segment
		if(*c_str == a && segment_open==false)
		{

			sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );

			last_delimiter = delimiter_index+1;

		}

		++delimiter_index;
		++c_str;

		first_character = false;
	}

	//and the last one
	sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );


	return sub_strings;
}

//splits the string with delimiter 'a', returns a pointer to a std::vector which contains all substrings
Lazarus::FastNCSList<std::string>* Toolbox::splitStringFSegment(const std::string &s, char a)
{
	unsigned int delimiter_index = 0;
	const char* c_str = s.c_str();
	bool first_character = true;
	Lazarus::FastNCSList<std::string>* sub_strings = new Lazarus::FastNCSList<std::string>();

	bool segment_open = false;

	unsigned int last_delimiter = 0;

	while(*c_str != '\0')
	{
		//the end of a segment
		if(*c_str == '"' && segment_open==true)
		{
			//check if the previous char was a '\',
			//since the segment is open; there is a previous char!
			//if it wasn't; close the segment
			if(*(c_str-1) != '\\')
			{
				segment_open = false;
				++delimiter_index;
				++c_str;
				continue;
			}

		}

		//the beginning of a segment
		if(*c_str == '"' && segment_open==false)
		{
			//check if the previous char was a '\',
			//since this might have been the first character, there couln't be a previous character.
			//if it wasn't; open the segment
			if(first_character == false)
			{
				if(*(c_str-1) != '\\')
				{
					segment_open = true;
				}
			}
			else//if its the first character: open the segment
			{
				segment_open = true;
			}
		}

		//normal parsing outside an open segment
		if(*c_str == a && segment_open==false)
		{

			sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );

			last_delimiter = delimiter_index+1;

		}

		++delimiter_index;
		++c_str;

		first_character = false;
	}

	//and the last one
	sub_strings->appendLast( s.substr(last_delimiter, (delimiter_index+1) -last_delimiter - 1 ) );


	return sub_strings;
}

void Toolbox::deleteChar(std::string& s, char a)
{
	unsigned int current_pos = 0;
	unsigned int string_size = s.length();

	while(current_pos < string_size)
	{
		//if the char occurred, delete it
		if(s.at(current_pos) == a)
		{
			s.erase(current_pos,1);
			string_size--;//update the size but stay at the current position
		}
		else
		{
			current_pos++;//if 'a' did not occur advance one char
		}

	}

}

//counts the number of 'a'-occurrences in f
int Toolbox::countCharF(FILE** f, char a)
{
	long long int size = 0;
	int counter = 0;

	fseeko64(*f, 0L, SEEK_END);
	size = ftello64(*f);
	fseeko64(*f, 0L, SEEK_SET);

	char* buf = new char[size+1];

	long long int res = fread(buf,sizeof(char),size,*f);

	/*if(res != size)
	{
		delete[] buf;
		return NULL;
	}*/

	/**
	 * In case of e.g. tmpfs file systems the determined size might be block aligned e.g. multiples of
	 * 4kb while the file content returned by fread might be smaller.
	 * */
	buf[res] = '\0';

	while(*buf != '\0')
	{
		if(*buf == a)
		{
			counter++;
		}

		buf++;
	}

	free(buf);

	fseeko64(*f, 0L, SEEK_SET);

	return counter;
}

//counts the number of 'a'-occurrences in s
int Toolbox::countLineBreaksF(FILE** f)
{
	return countCharF(f,'\n');
}

std::string Toolbox::slash_terminate_path(const std::string& path)
{
	//if string is already terminated with a slash
	if(path.at( path.size()-1 ) == '/')
	{
		return path;
	}
	else
	{
		return path+std::string("/");
	}

	//this will never happen
	return std::string("ERROR: could not terminate path with slash");
}


/*
 * Converts the contents of a file into a string
 */
std::string Toolbox::readFileToString(const std::string& filename)
{
	char* ss = readFileToCString(filename);

	std::string s(ss);

	delete[] ss;

	return s;
}

/*
 * Converts the contents of a file into a string
 */
char* Toolbox::readFileToCString(const std::string& filename)
{
	FILE* fp = fopen64(filename.c_str(),"r");

	if(fp == NULL)
	{
		printf("ERROR: file '%s' not found\n",filename.c_str());
		return NULL;
	}

	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);

	char* buf = new char[size+1];

	/**
	 * In case of e.g. tmpfs file systems the determined size might be block aligned e.g. multiples of
	 * 4kb while the file content returned by fread might be smaller.
	 * */
	long long int res = fread(buf,sizeof(char),size,fp);

	//if more data has been read
	/*if(res > size)
	{
		delete[] buf;
		return NULL;
	}*/

	fclose(fp);

	/**
	 * In case of e.g. tmpfs file systems the determined size might be block aligned e.g. multiples of
	 * 4kb while the file content returned by fread might be smaller.
	 * */
	buf[res] = '\0';

	return buf;
}

Buffer* Toolbox::readFileToBuffer(const std::string& filename)
{
	FILE* fp = fopen64(filename.c_str(),"rb");

	if(fp == NULL)
	{
		printf("ERROR: file not found\n");
		return NULL;
	}

	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);

	Buffer* buf = new Buffer(size);

	fread(buf->get_mp_data(),sizeof(unsigned char),size,fp);

	fclose(fp);

	return buf;
}


int Toolbox::readTextFileToArray(const std::string& filename, unsigned char* buffer)
{

	FILE* fp = fopen64(filename.c_str(),"r");

	if(fp == NULL)
	{
		printf("ERROR: file not found\n");
		return -1;
	}

	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);

	fread(buffer,sizeof(unsigned char),size,fp);

	fclose(fp);

	return 0;
}

int Toolbox::readFileToArray(const std::string& filename, unsigned char* buffer)
{
	FILE* fp = fopen64(filename.c_str(),"rb");

	if(fp == NULL)
	{
		printf("ERROR: file not found\n");
		return -1;
	}

	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);

	fread(buffer,sizeof(unsigned char),size,fp);

	fclose(fp);

	return 0;
}

int Toolbox::char_peek(FILE** fp)
{
	//advance one character
	const int c = getc(*fp);

	//try to return the character
	if(c != EOF)
	{
		return ungetc(c, *fp);
	}
	else
	{
		return EOF;
	}

}

void Toolbox::writeStringToFile(const std::string& s, const std::string& filename)
{
	writeArrayToFile(filename,(unsigned char*)s.c_str(),s.size());
}

void Toolbox::writeCStringToFile(char* s, const std::string& filename)
{
	writeArrayToFile(filename,(unsigned char*)s,strlen(s));
}

void Toolbox::appendStringToFile(const std::string& s, const std::string& filename)
{
	appendArrayToFile(filename,(unsigned char*)s.c_str(),s.size());
}

void Toolbox::appendCStringToFile(char* s, const std::string& filename)
{
	appendArrayToFile(filename,(unsigned char*)s,strlen(s));
}

void Toolbox::writeStringVectorToFile(Fifo<std::string>& sv, const std::string& filename)
{
	size_t size;

	//create the file (purge any existing one)
	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == false)
	{
		printf("Error: file not found\n");
		return;
	}

	f.close();

	//iterate over all entries
	for(auto it = sv.getStart();it != sv.getEnd();++it)
	{
		size = it.getElement()->getm_data().size();
		appendArrayToFile(filename,(unsigned char*)it.getElement()->getm_data().c_str(),size);
	}

}

void Toolbox::writeStringVectorToFile(std::vector<std::string>& sv, const std::string& filename)
{
	size_t size;

	//create the file (purge any existing one)
	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == false)
	{
		printf("Error: file not found\n");
		return;
	}

	f.close();

	//append the content
	for(unsigned int i=0; i<sv.size(); i++)
	{
		size = sv[i].size();

		appendArrayToFile(filename,(unsigned char*)sv[i].c_str(),size);
	}

}

void Toolbox::writeCStringVectorToFile(std::vector<char*>& sv,const std::string& filename)
{
	size_t size;

	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == false)
	{
		printf("Error: file not found\n");
		return;
	}

	for(unsigned int i=0; i<sv.size(); i++)
	{
		size = strlen(sv[i]);

		appendArrayToFile(filename,(unsigned char*)sv[i],size);
	}

	//close the file
	f.close();
}

void Toolbox::appendStringVectorToFile(std::vector<std::string>& sv,const std::string& filename)
{

	for(unsigned int i=0; i<sv.size(); i++)
	{
		appendArrayToFile(filename,(unsigned char*)sv[i].c_str(),sv[i].size());
	}

}

void Toolbox::appendCStringVectorToFile(std::vector<char*>& sv,const std::string& filename)
{
	size_t size;

	for(unsigned int i=0; i<sv.size(); i++)
	{
		size = strlen(sv[i]);

		appendArrayToFile(filename,(unsigned char*)sv[i],size);
	}
}



void Toolbox::writeStringVectorToFile(FastNCSList<std::string>& sv, const std::string& filename)
{
	size_t size;

	//purge any existing file
	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == false)
	{
		printf("Error: file not found\n");
		return;
	}

	//close the file
	f.close();

	GenNCSListElement<std::string>* element = sv.getFirst();

	while(element != NULL)
	{
		size = element->getm_data().size();

		appendArrayToFile(filename,(unsigned char*)element->getm_data().c_str(),size);

		element = element->getmp_neighbor();
	}

}

void Toolbox::writeCStringVectorToFile(FastNCSList<char*>& sv,const std::string& filename)
{
	size_t size;

	//purge the file
	std::fstream f(filename.c_str(), (std::fstream::out | std::fstream::binary | std::fstream::trunc));

	if(f.is_open() == false)
	{
		printf("Error: file not found\n");
		return;
	}
	//close the file
	f.close();

	GenNCSListElement<char*>* element = sv.getFirst();

	while(element != NULL)
	{
		size = strlen( element->getm_data() );

		appendArrayToFile(filename,(unsigned char*)element->getm_data(),size);

		element = element->getmp_neighbor();
	}

}

void Toolbox::appendStringVectorToFile(FastNCSList<std::string>& sv, const std::string& filename)
{
	size_t size;

	GenNCSListElement<std::string>* element = sv.getFirst();

	while(element != NULL)
	{
		size = element->getm_data().size();

		appendArrayToFile(filename,(unsigned char*)element->getm_data().c_str(),size);

		element = element->getmp_neighbor();
	}
}

void Toolbox::appendCStringVectorToFile(FastNCSList<char*>& sv,const std::string& filename)
{
	size_t size;

	GenNCSListElement<char*>* element = sv.getFirst();

	while(element != NULL)
	{
		size = strlen( element->getm_data() );

		appendArrayToFile(filename,(unsigned char*)element->getm_data(),size);

		element = element->getmp_neighbor();
	}

}

std::string Toolbox::getCurrentDirectory()
{
   char cwd[1024];
   if(getcwd(cwd, sizeof(cwd)) == NULL)
   {
	   printf("ERROR: could not get current working directory\n");
   }
   return std::string(cwd);
}


void Toolbox::sleep_s(unsigned long s)
{
	sleep(s);
}


void Toolbox::sleep_ms(unsigned long ms)
{
	usleep(1000*ms);
}

void Toolbox::sleep_us(unsigned long us)
{
	usleep(us);
}

void Toolbox::sleep_ns(unsigned long ns)
{
	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = ns;

	nanosleep(&tim,NULL);
}


void Toolbox::printSystemConfig()
{
	struct sysinfo info;

	sysinfo(&info);

	printf("%ld Seconds since boot \n"
				 "(%lu ,%lu, %lu ) 1, 5, and 15 minute load averages \n"
				 "%lu Total usable main memory size \n"
				 "%lu Available memory size \n"
				 "%lu Amount of shared memory \n"
				 "%lu Memory used by buffers \n"
				 "%lu Total swap space size \n"
				 "%lu Swap space still available \n"
				 "%us Number of current processes \n"
				 "%lu Total high memory size \n"
				 "%lu Available high memory size \n"
				 "%u Memory unit size in bytes \n",
				info.uptime,             /* Seconds since boot */
				info.loads[0],info.loads[1],info.loads[2],  /* 1, 5, and 15 minute load averages */
				info.totalram,  /* Total usable main memory size */
				info.freeram,   /* Available memory size */
				info.sharedram, /* Amount of shared memory */
				info.bufferram, /* Memory used by buffers */
				info.totalswap, /* Total swap space size */
				info.freeswap,  /* Swap space still available */
				info.procs,    /* Number of current processes */
				info.totalhigh, /* Total high memory size */
				info.freehigh,  /* Available high memory size */
				info.mem_unit   /* Memory unit size in bytes */
				);
}

/**
 * Will return the first CPUs level 0 cache size.
 * */
unsigned long long Toolbox::getCPUCache0Size()
{
	static unsigned long long size = 0;

	if(size == 0)
	{
		//create file string for current cpu
		std::string cache_path = "/sys/devices/system/cpu/cpu"+Toolbox::intToString(0)
				+"/cache/index"+Toolbox::intToString(0);

		std::string text = "";

		text = Toolbox::readFileToString(cache_path+"/size");
		std::replace(text.begin(),text.end(),'\n',' ');
		text.erase( std::remove_if(text.begin(), text.end(), isspace),text.end() );
		size = Toolbox::getExpMetricULongLong(text);

		return size;

	}
	else
		return size;
}

/**
 * Will return the first CPUs level 0 cache line size.
 * */
unsigned long long Toolbox::getCPUCache0LineSize()
{
	static unsigned long long size = 0;

	if(size == 0)
	{
		//create file string for current cpu
		std::string cache_path = "/sys/devices/system/cpu/cpu"+Toolbox::intToString(0)
				+"/cache/index"+Toolbox::intToString(0);

		std::string text = "";

		text = Toolbox::readFileToString(cache_path+"/coherency_line_size");
		std::replace(text.begin(),text.end(),'\n',' ');
		text.erase( std::remove_if(text.begin(), text.end(), isspace),text.end() );
		size = Toolbox::stringToULongLong(text);

		return size;

	}
	else
		return size;
}

/**
 * Will return the first CPUs level 0 cache line partition unit in bytes.
 * */
unsigned long long Toolbox::getCPUCache0LinePartition()
{
	static unsigned long long size = 0;

	if(size == 0)
	{
		//create file string for current cpu
		std::string cache_path = "/sys/devices/system/cpu/cpu"+Toolbox::intToString(0)
				+"/cache/index"+Toolbox::intToString(0);

		std::string text = "";

		text = Toolbox::readFileToString(cache_path+"/pyhsical_line_partition");
		std::replace(text.begin(),text.end(),'\n',' ');
		text.erase( std::remove_if(text.begin(), text.end(), isspace),text.end() );
		size = Toolbox::stringToULongLong(text);

		return size;

	}
	else
		return size;
}

unsigned long long Toolbox::alignToCPUCache0(unsigned long long size)
{
	return size + (getCPUCache0LineSize() - size % getCPUCache0LineSize());
}

unsigned long long Toolbox::getCPUCount()
{
	static unsigned long long cpu_count = 0;

	if(cpu_count == 0)
	{
		//+++++++++++++++++++++++++ CPU
		//determine the number of cpus
		FILE* fp;
		char buffer[32000];
		size_t bytes_read;
		/* Read the entire contents of /proc/cpuinfo into the buffer. */
		fp = fopen("/proc/cpuinfo", "r");
		bytes_read = fread(buffer, 1, sizeof (buffer), fp);
		fclose (fp);

		/* Bail if read failed or if buffer is not big enough. */
		if(bytes_read == 0 || bytes_read == sizeof (buffer))
		{
			printf("ERROR: could not read the CPU information\n");
			return 0;
		}

		/* NUL-terminate the text. */
		buffer[bytes_read] = '\0';
		char* match = buffer;

		while(1)
		{
			/* Locate the line that starts with 'processor'. */
			//locate the line
			match = strstr(match, "processor");

			if (match == NULL)
			{
				//printf("ERROR: could not locate more processor lines\n");
				break;
			}

			cpu_count++;
			match++;
		}
	}

	return cpu_count;
}

const char* Toolbox::ERRNO_LOOKUP[125] =
{
 "NULL:"
,"EPERM"                  //1
,"ENOENT"                 //2
,"ESRCH"                  //3
,"EINTR"                  //4
,"EIO"                    //5
,"ENXIO"                  //6
,"E2BIG"                  //7
,"ENOEXEC"                //8
,"EBADF"                  //9
,"ECHILD"                 //10
,"EAGAIN"                 //11
,"ENOMEM"                 //12
,"EACCES"                 //13
,"EFAULT"                 //14
,"ENOTBLK"                //15
,"EBUSY"                  //16
,"EEXIST"                 //17
,"EXDEV"                  //18
,"ENODEV"                 //19
,"ENOTDIR"                //20
,"EISDIR"                 //21
,"EINVAL"                 //22
,"ENFILE"                 //23
,"EMFILE"                 //24
,"ENOTTY"                 //25
,"ETXTBSY"                //26
,"EFBIG"                  //27
,"ENOSPC"                 //28
,"ESPIPE"                 //29
,"EROFS"                  //30
,"EMLINK"                 //31
,"EPIPE"                  //32
,"EDOM"                   //33
,"ERANGE"                 //34
,"EDEADLK"                //35
,"ENAMETOOLONG"           //36
,"ENOLCK"                 //37
,"ENOSYS"                 //38
,"ENOTEMPTY"              //39
,"ELOOP"                  //40
,"NULL"                   //41
,"ENOMSG"                 //42
,"EIDRM"                  //43
,"ECHRNG"                 //44
,"EL2NSYNC"               //45
,"EL3HLT"                 //46
,"EL3RST"                 //47
,"ELNRNG"                 //48
,"EUNATCH"                //49
,"ENOCSI"                 //50
,"EL2HLT"                 //51
,"EBADE"                  //52
,"EBADR"                  //53
,"EXFULL"                 //54
,"ENOANO"                 //55
,"EBADRQC"                //56
,"EBADSLT"                //57
,"NULL"                   //58
,"EBFONT"                 //59
,"ENOSTR"                 //60
,"ENODATA"                //61
,"ETIME"                  //62
,"ENOSR"                  //63
,"ENONET"                 //64
,"ENOPKG"                 //65
,"EREMOTE"                //66
,"ENOLINK"                //67
,"EADV"                   //68
,"ESRMNT"                 //69
,"ECOMM"                  //70
,"EPROTO"                 //71
,"EMULTIHOP"              //72
,"EDOTDOT"                //73
,"EBADMSG"                //74
,"EOVERFLOW"              //75
,"ENOTUNIQ"               //76
,"EBADFD"                 //77
,"EREMCHG"                //78
,"ELIBACC"                //79
,"ELIBBAD"                //80
,"ELIBSCN"                //81
,"ELIBMAX"                //82
,"ELIBEXEC"               //83
,"EILSEQ"                 //84
,"ERESTART"               //85
,"ESTRPIPE"               //86
,"EUSERS"                 //87
,"ENOTSOCK"               //88
,"EDESTADDRREQ"           //89
,"EMSGSIZE"               //90
,"EPROTOTYPE"             //91
,"ENOPROTOOPT"            //92
,"EPROTONOSUPPORT"        //93
,"ESOCKTNOSUPPORT"        //94
,"EOPNOTSUPP"             //95
,"EPFNOSUPPORT"           //96
,"EAFNOSUPPORT"           //97
,"EADDRINUSE"             //98
,"EADDRNOTAVAIL"          //99
,"ENETDOWN"               //100
,"ENETUNREACH"            //101
,"ENETRESET"              //102
,"ECONNABORTED"           //103
,"ECONNRESET"             //104
,"ENOBUFS"                //105
,"EISCONN"                //106
,"ENOTCONN"               //107
,"ESHUTDOWN"              //108
,"ETOOMANYREFS"           //109
,"ETIMEDOUT"              //110
,"ECONNREFUSED"           //111
,"EHOSTDOWN"              //112
,"EHOSTUNREACH"           //113
,"EALREADY"               //114
,"EINPROGRESS"            //115
,"ESTALE"                 //116
,"EUCLEAN"                //117
,"ENOTNAM"                //118
,"ENAVAIL"                //119
,"EISNAM"                 //120
,"EREMOTEIO"              //121
,"EDQUOT"                 //122
,"ENOMEDIUM"              //123
,"EMEDIUMTYPE"            //124
} ;


}
