#ifndef DEFINES_
#define DEFINES_

#include "CMakeConfig.h"

//OSX/Unix/FreeBSD does not support fX64 (also requires -D_FILE_OFFSET_BITS=64 compiler flag)
#ifdef OSX
#define fopen64 fopen
#define fseeko64 fseek
#define ftello64 ftell
#endif

#undef CUDA
#define AMD_ATI
#undef DEBUG
#undef WINDOWS

/**
 * A macro which will print the error text with the current file, line and function as prefix.
 * Requires an inclusion of <iostream> and <string>.
 * */
#define ERROR(X) std::cout<<__FILE__<<":"<<__func__<<":"<<__LINE__<<"::ERROR - "<<std::string(X)<<std::endl;

/**
 * A macro which will print the warning text with the current file, line and function as prefix.
 * Requires an inclusion of <iostream> and <string>.
 * */
#define WARNING(X) std::cout<<__FILE__<<":"<<__func__<<":"<<__LINE__<<"::WARNING - "<<std::string(X)<<std::endl;

/**
 * A macro which will print the info text with the current file, line and function as prefix.
 * Requires an inclusion of <iostream> and <string>.
 * */
#define INFO(X) std::cout<<__FILE__<<":"<<__func__<<":"<<__LINE__<<"::INFO - "<<std::string(X)<<std::endl;

/**
"A word is the amount of data that a machine can process at one time."
"The size of a processors general-purpose registers (GPRs) is equal to its word size."
"Additionally, the size of the C type long is equal to the word size, whereas the size of the int type is sometimes less than that of the word size"

-- Linux Kernel Development, Ch 17 (3rd edition, pg 381)

CAUTION
http://stackoverflow.com/questions/29519068/how-to-determine-processor-word-length-in-c/29519151#comment47194967_29519151
 Downvote: The long type is based on a range of values not the processor's word size. On 8-bit and 16-bit processors,
 the long type would take up more than one processor word in order to meet the range specification. So, by your statement,
 on an 8-bit processor, the long type would have the range of 8 bits, which is smaller than the range of an int.
  Thomas Matthews Apr 8 '15 at 16:17
**/
#define WORD_SIZE_BIT sizeof(long)*CHAR_BIT
#define WORD_SIZE_BYTE sizeof(long)

#define CSV_FLOAT_FORMAT "%.15f"
#define CSV_DOUBLE_FORMAT "%.20lf"
#define CSV_LONG_DOUBLE_FORMAT "%.40Lf"
#define CSV_INT_FORMAT "%d"
#define CSV_SHORT_FORMAT "%hd"
#define CSV_SIZE_T_FORMAT "%lu"
#define CSV_UINT_FORMAT "%u"
#define CSV_USHORT_FORMAT "%hu"
#define CSV_CHAR_FORMAT "%hhd"
#define CSV_UCHAR_FORMAT "%hhu"
#define CSV_LONG_FORMAT "%ld"
#define CSV_ULONG_FORMAT "%lu"
#define CSV_LONGLONG_FORMAT "%lld"
#define CSV_ULONGLONG_FORMAT "%llu"

#define CSV_INT_FORMAT_HEX "%x"
#define CSV_SIZE_T_FORMAT_HEX "%lx"
#define CSV_UINT_FORMAT_HEX "%ux"
#define CSV_CHAR_FORMAT_HEX "%hhx"
#define CSV_UCHAR_FORMAT_HEX "%hhux"
#define CSV_LONG_FORMAT_HEX "%lx"
#define CSV_ULONG_FORMAT_HEX "%lux"
#define CSV_LONGLONG_FORMAT_HEX "%llx"
#define CSV_ULONGLONG_FORMAT_HEX "%llux"

#define CSV_INT_FORMAT_OCT "%o"
#define CSV_SIZE_T_FORMAT_OCT "%lo"
#define CSV_UINT_FORMAT_OCT "%uo"
#define CSV_CHAR_FORMAT_OCT "%hho"
#define CSV_UCHAR_FORMAT_OCT "%hhuo"
#define CSV_LONG_FORMAT_OCT "%lo"
#define CSV_ULONG_FORMAT_OCT "%luo"
#define CSV_LONGLONG_FORMAT_OCT "%llo"
#define CSV_ULONGLONG_FORMAT_OCT "%lluo"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define CPU_SPEED 3600000000//3060000000

#define LOOP_MAX_UNROLL_SIZE 10

#ifndef NULL
#define NULL __null
//reinterpret_cast<void *>(0)
//((void *) 0)
#endif

#define __stdcall __attribute__((stdcall))

#define DELETE_NULL(pePointer) {delete pePointer;pePointer=NULL;}

#define DELETE_ARRAY_NULL(pePointer) {delete[] pePointer; pePointer=NULL;}

#define FREE_NULL(pePointer) {free(pePointer);pePointer=NULL;}

#define DELETE_NULL_CHECKING(pePointer) {if(pePointer!=NULL){delete pePointer;pePointer=NULL;}}

#define DELETE_ARRAY_NULL_CHECKING(pePointer) {if(pePointer!=NULL){delete[] pePointer;pePointer=NULL;}}

#define FREE_NULL_CHECKING(pePointer) {if(pePointer!=NULL){free(pePointer);pePointer=NULL;}}

#define SP_(X) std::shared_ptr(X)
#define WP_(X) std::weak_ptr(X)
#define UP_(X) std::unique_ptr(X)


enum SH_DATA_TYPE {SH_DATA_TYPE_DOUBLE, SH_DATA_TYPE_FLOAT,
		SH_DATA_TYPE_LONG_DOUBLE, SH_DATA_TYPE_INT, SH_DATA_TYPE_LONGLONG, SH_DATA_TYPE_ULONGLONG,
		SH_DATA_TYPE_LONG, SH_DATA_TYPE_ULONG,SH_DATA_TYPE_CHAR,
		SH_DATA_TYPE_SHORT,SH_DATA_TYPE_USHORT,
		SH_DATA_TYPE_UINT, SH_DATA_TYPE_UCHAR, SH_DATA_TYPE_ABSTRACT, SH_DATA_TYPE_UNSPECIFIED};

/**
 * Macros for inclusion of concatenated "define"-strings.
 * INCLUDE_FILE(A,B) will create a string: <AB>
 * E.g.
 *
 * #define A /tools/power/cpupower/lib/cpufreq.h
 * #include INCLUDE_FILE2(LINUX_HEADERS,A)
 *
 * */
#define stringify(x) #x
#define INC_STRING2(a, b) stringify(< ## a ## b ## >)
#define INCLUDE_FILE2(a, b) INC_STRING2(a, b)

/**
 * OpenMP macros
 * */
#define PAR_START(T_COUNT) \
	omp_set_num_threads(T_COUNT); \
	_Pragma("omp parallel")  \
	{

#define PAR_END }

#define GTID omp_get_thread_num()


/*
//---------------- methods for compile-time primitive type determination (not needed for c++11 as it features decltype)
template<int N> struct size_container { char value[N]; };

//function declarations which return different sized size_container's depending on their signature
size_container<1> encode(char);
size_container<2> encode(unsigned char);
size_container<3> encode(signed char);
size_container<4> encode(bool);
size_container<5> encode(int);
size_container<6> encode(unsigned int);
size_container<7> encode(float);
size_container<8> encode(double);
size_container<9> encode(long double);
size_container<10> encode(long int);
size_container<11> encode(unsigned long int);
size_container<12> encode(long long int);
size_container<13> encode(unsigned long long int);
size_container<14> encode(short int);
size_container<15> encode(unsigned short int);


template<int N> struct decode {};
//template specializations of struct decode
template<> struct decode<1> { typedef char type; };
template<> struct decode<2> { typedef unsigned char type; };
template<> struct decode<3> { typedef signed char type; };
template<> struct decode<4> { typedef bool type; };
template<> struct decode<5> { typedef int type; };
template<> struct decode<6> { typedef unsigned int type; };
template<> struct decode<7> { typedef float type; };
template<> struct decode<8> { typedef double type; };
template<> struct decode<9> { typedef long double type; };
template<> struct decode<10> { typedef long int type; };
template<> struct decode<11> { typedef unsigned long int type; };
template<> struct decode<12> { typedef long long int type; };
template<> struct decode<13> { typedef unsigned long long int type; };
template<> struct decode<14> { typedef short int type; };
template<> struct decode<15> { typedef unsigned short int type; };

//sizeof returns the size of the chosen functions return type, decode's template argument then delivers the correct type
#define TYPE_OF(expr) decode< sizeof(encode(expr)) >::type
*/


#endif
