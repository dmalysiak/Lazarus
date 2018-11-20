#ifndef TOOLBOX_H_
#define TOOLBOX_H_

#include "BaseObject.h"
#include "Buffer.h"
#include "DataStructures/NCSList.h"
#include "DataStructures/FastNCSList.h"
#include "DataStructures/FastKTuple.h"
#include "DataStructures/Fifo.h"
#include "Threading/Thread.h"

#include <string>
#include <time.h>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <sstream>

#include <type_traits>

namespace Lazarus
{

class Toolbox  : public BaseObject {

public:

	static const char* ERRNO_LOOKUP[125];

	Toolbox();
	virtual ~Toolbox();

	/**
	 * This method will return true if u is an instance of a
	 * class which is a subclass of T. Otherwise false is being returned.
	 * Works only for pointer types.
	 * */
	template<class T, class U>
	static bool isSubClass(const U* u)
	{
		if( dynamic_cast<T*>(u) == NULL )
			return false;

		return true;
	}

	/**
	 * This method will return true if u is an instance of a
	 * class which is a subclass of T. Otherwise false is being returned.
	 * Works only for references.
	 * */
	template<class T, class U>
	static bool isSubClass(const U& u)
	{
		if( dynamic_cast<T&>(u) == NULL )
			return false;

		return true;
	}

	/**
	 * Prints a readable error message for errno-codes.
	 * */
	static void decodeErrno(int error);

	/**
	 * Replaces any existing '//' with '/' and ensures that the output end with a single /.
	 * */
	static std::string ensureValidPath(const std::string& s);

	//endianness methods
	static short int swapShortInt(short int i);
	static short unsigned int swapShortInt(short unsigned int i);
	static int swapInt(int i);
	static unsigned int swapUInt(unsigned int i);
	static long int swapLongInt(long int i);
	static unsigned long int swapULongInt(unsigned long int i);
	static long long int swapLongLongInt(long long int i);
	static unsigned long long int swapULongLongInt(unsigned long long int i);
	static double swapDouble(double i);
	static float swapFloat(float i);

	//c-string conversion methods
	static char* floatToCString(float value, unsigned int digits = std::numeric_limits< float >::max_digits10);
	static char* doubleToCString(double value, unsigned int digits = std::numeric_limits< double >::max_digits10);
	static char* longDoubleToCString(long double value, unsigned int digits = std::numeric_limits< long double >::max_digits10);
	static char* intToCString(int value);
	static char* longToCString(long value);
	static double cStringToDouble(char* s);
	static long double cStringToLongDouble(char* s);
	static int cStringToInt(char* s);
	static long int cStringToLongInt(char* s);
	static long long int cStringToLongLongInt(char* s);

	//std::string conversion methods
	static std::string doubleToString(double d, unsigned int digits = std::numeric_limits< double >::max_digits10);
	static std::string longDoubleToString(long double d, unsigned int digits = std::numeric_limits< long double >::max_digits10);
	static std::string floatToString(float d, unsigned int digits = std::numeric_limits< float >::max_digits10);
	static std::string uintToString(unsigned int d);
	static std::string ushortToString(unsigned short d);
	static std::string longToString(long d);
	static std::string longLongToString(long long d);
	static std::string ulongToString(unsigned long d);
	static std::string ulongLongToString(unsigned long long d);
	static std::string intToString(int d);
	static std::string shortToString(short d);
	static std::string size_tToString(size_t d);
	static long double stringToLongDouble(const std::string& s);
	static double stringToDouble(const std::string& s);
	static float stringToFloat(const std::string& s);
	static unsigned int stringToUInt(const std::string& s);
	static unsigned short stringToUShort(const std::string& s);
	static long stringToLong(const std::string& s);
	static long long stringToLongLong(const std::string& s);
	static unsigned long stringToULong(const std::string& s);
	static unsigned long long stringToULongLong(const std::string& s);
	static int stringToInt(const std::string& s);
	static short stringToShort(const std::string& s);
	static size_t stringTosize_t(const std::string& s);

	//will add padding zeros in front of the integer number
	template<typename T>
	static std::string numToString(T d, unsigned int digits = 0)
	{
		if(isInteger(d) == false)
		{
			printf("ERROR: can not add digits to non-integer value\n");
			return "";
		}

		//get the amount of digits
		unsigned int digs = countDigits(d);

		std::ostringstream convert;

		//if there are less digits than requested
		if(digs < digits)
			for(unsigned int i=0;i<digits-digs;++i)
				convert << (T)0;

		//add the number
		convert << d;

		return convert.str();
	}


	static unsigned char stringToUCharB(const std::string& s, int base);
	static char stringToCharB(const std::string& s, int base);
	static unsigned int stringToUIntB(const std::string& s, int base);
	static long stringToLongB(const std::string& s, int base);
	static long long stringToLongLongB(const std::string& s, int base);
	static unsigned long stringToULongB(const std::string& s, int base);
	static unsigned long long stringToULongLongB(const std::string& s, int base);
	static int stringToIntB(const std::string& s, int base);
	static size_t stringTosize_tB(const std::string& s, int base);

	static std::string ucharToStringB(unsigned char d, int base);
	static std::string charToStringB(char d, int base);
	static std::string uintToStringB(unsigned int d, int base);
	static std::string longToStringB(long d, int base);
	static std::string longLongToStringB(long long d, int base);
	static std::string ulongToStringB(unsigned long d, int base);
	static std::string ulongLongToStringB(unsigned long long d, int base);
	static std::string intToStringB(int d, int base);
	static std::string size_tToStringB(size_t d, int base);

	//std::string methods
	//counts the number of 'a'-occurrences in s
	static unsigned int countChar(const std::string& s, char a);
	//counts the number of 'a'-occurrences in s
	static unsigned int countLineBreaks(const std::string& s);
	//splits the string with delimiter 'a'
	static Lazarus::FastNCSList<std::string>* splitStringF(const std::string&s, char a);
	static Lazarus::NCSList<std::string>* splitString(const std::string&s, char a);
	/**
	 * splits the string with delimiter 'a' but also keeps segments i.e. <"....."> together.
	 * In a nutshell, this method will split the string along 'a' but ignore any occurence of 'a'
	 * in a segment <"...">. '"' inside a segment can be escaped via '\"', yet this will not
	 * remove the '\"' from the segment.
	 */
	static Lazarus::FastNCSList<std::string>* splitStringFSegment(const std::string&s, char a);
	static Lazarus::NCSList<std::string>* splitStringSegment(const std::string&s, char a);
	//deletes all occurrences of 'a' in the string
	static void deleteChar(std::string& s, char a);

	/**
	 * Returns the number of a occurrences in a file, otherwise -1;
	 * */
	static int countCharF(FILE** f, char a);

	/**
	 * Returns the number of linebreaks in a file, otherwise -1;
	 * */
	static int countLineBreaksF(FILE** f);

	/**
	 * returns a '/'-terminated copy of the given string. If the string is already /-terminated a string copy is returned.
	 */
	static std::string slash_terminate_path(const std::string& path);

	/**
	 * Creates an empty file (i.e. filled with NULL). Returns 0 in case of success, -1 otherwise.
	 * Unit may be the count in bytes or e.g. 1M, 1G etc.
	 * */
	static int createEmptyFile(const std::string& file, unsigned long long count, const std::string unit = std::string("1024"));

	/**
	 * Creates a file filled with random data. Returns 0 in case of success, -1 otherwise.
	 * Unit may be the count in bytes or e.g. 1M, 1G etc.
	 * */
	static int createRandomFile(const std::string& file, unsigned long long count, const std::string unit = std::string("1024"));

	/**
	 * Returns the directory of the executable (i.e. not including the executable and not trailing slash).
	 * */
	static std::string getCurrentDirectory();

	/**
	 * Returns true if the specified file exists, otherwise false
	 * */
	static bool checkFileExistence(const std::string& file);

	/**
	 * -1 in case of errors, filesize otherwise.
	 */
	static long long int getFileSize(const std::string& filename);
	/**
	 * -1 in case of errors, filesize otherwise. The file stream must be opened with 'fopen64' before calling this function.
	 */
	static long long int getFileSize(FILE* file);

	static void printSystemConfig();

	static void exitThread(void* data);
	static void dummyThreadTask(Thread*, void* data);

	static void sleep_s(unsigned long s);
	static void sleep_ms(unsigned long ms);
	static void sleep_us(unsigned long us);
	static void sleep_ns(unsigned long ns);

	static std::string readFileToString(const std::string& filename);

	/**
	 * In case of e.g. tmpfs file systems the determined size might be block aligned e.g. multiples of
	 * 4kb while the file content returned by fread might be smaller.
	 * In other words, the returned char array might be larger than the actual content.
	 * */
	static char* readFileToCString(const std::string& filename);

	static Buffer* readFileToBuffer(const std::string& filename);


	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static unsigned int getExpMetricUInt(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static int getExpMetricInt(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static unsigned long long getExpMetricULongLong(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static long long getExpMetricLongLong(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static unsigned long getExpMetricULong(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static long getExpMetricLong(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static unsigned short getExpMetricUShort(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static short getExpMetricShort(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static float getExpMetricFloat(const std::string& s);

	/**
	 * This function will return the integer value 24000 from strings like e.g. 24k,
	 * i.e. the suffix will be parsed and the value expanded.
	 * Accepted strings are of form NUM|[k|K|m|M|g|G|t|T|p|P|e|E], depending on the functions
	 * return type overflows might occur with respect to the suffix.
	 * */
	static double getExpMetricDouble(const std::string& s);

	/**
	 * This function reads a CSV token (i.e. value) from the current position within the open file.
	 * It is assumed that the file contains the following format:
	 * VAL<SEP>VAL<SEP>VAL<TER><LBREAK>
	 * The value is inserted into the given string.
	 * The function returns 0 in case of a complete value without hitting a TER symbol,
	 * 1 in case of hitting a TER symbol, 2 in case of hitting EOF, -1 else
	 * */
	static int readCSVValue(FILE** pFile, std::string& s,char sep=',',char ter=';', bool linebreaks_after_terminator=true);

	/**
	 * Returns 0 on success, -1 otherwise.
	 */
	static int readLinesToTuple(const std::string& filename, FastKTuple<std::string>& data);
	/**
	 * The array must be preallocated and be large enough. 0 on success, -1 otherwise.
	 */
	static int readFileToArray(const std::string& filename, unsigned char* buffer);
	/**
	 * The array must be preallocated and be large enough. 0 on success, -1 otherwise.
	 */
	static int readTextFileToArray(const std::string& filename, unsigned char* buffer);
	/**
	 * Will overwrite any previous file content.
	 */
	static int writeArrayToFile(const std::string& filename, unsigned char* buffer, unsigned long long size);
	/**
	 * Will overwrite any previous file content.
	 */
	static int writeArrayToTextFile(const std::string& filename, unsigned char* buffer, unsigned long long size);
	/**
	 * Will append to the file.
	 */
	static int appendArrayToFile(const std::string& filename, unsigned char* buffer, unsigned long long size);
	/**
	 * Will append to the file.
	 */
	static int appendArrayToTextFile(const std::string& filename, unsigned char* buffer, unsigned long long size);

	/**
	 * The array must be preallocated and be large enough. 0 on success, -1 otherwise.
	 * The offset is the initial starting point within the file.
	 */
	static int readFileToArray(FILE* file, unsigned char* buffer, unsigned long long bytes, unsigned long long offset = 0);
	/**
	 * This function will write the data to the file depending on how the file handle was obtained.
	 */
	static int writeArrayToFile(FILE* file, unsigned char* buffer, unsigned long long size);

	static int char_peek(FILE** fp);

	/**
	 * Use only true c-strings for the CString-methods!!
	 */
	static void writeStringToFile(const std::string& s, const std::string& filename);
	static void writeCStringToFile(char* s,const std::string& filename);
	static void appendStringToFile(const std::string& s, const std::string& filename);
	static void appendCStringToFile(char* s,const std::string& filename);

	static void writeStringVectorToFile(Fifo<std::string>& sv, const std::string& filename);

	static void writeStringVectorToFile(std::vector<std::string>& sv, const std::string& filename);
	static void writeCStringVectorToFile(std::vector<char*>& sv,const std::string& filename);
	static void appendStringVectorToFile(std::vector<std::string>& sv, const std::string& filename);
	static void appendCStringVectorToFile(std::vector<char*>& sv,const std::string& filename);

	static void writeStringVectorToFile(FastNCSList<std::string>& sv, const std::string& filename);
	static void writeCStringVectorToFile(FastNCSList<char*>& sv,const std::string& filename);
	static void appendStringVectorToFile(FastNCSList<std::string>& sv, const std::string& filename);
	static void appendCStringVectorToFile(FastNCSList<char*>& sv,const std::string& filename);

	static void assertInteger(double val);
	static void assertInteger(float val);
	//asserts that b|a
	static void assertDivisible(unsigned int a, unsigned int b);

	//alignment helpers

	/**
	 * Will return the first CPUs level 0 cache size.
	 * */
	static unsigned long long getCPUCache0Size();

	/**
	 * Will return the first CPUs level 0 cache line size.
	 * */
	static unsigned long long getCPUCache0LineSize();

	/**
	 * Will return the first CPUs level 0 cache line partition unit in bytes.
	 * */
	static unsigned long long getCPUCache0LinePartition();

	/**
	 * Will return a a size aligned to the CPUs level 0 cache line size.
	 * */
	static unsigned long long alignToCPUCache0(unsigned long long size);

	/**
	 * Returns the number of CPUs in the system.
	 * */
	static unsigned long long getCPUCount();

	template<typename T>
	static bool isInteger(T val)
	{
		if(std::is_same<T,int>::value == true || std::is_same<T,unsigned int>::value == true ||
					std::is_same<T,char>::value == true || std::is_same<T,unsigned char>::value == true ||
					std::is_same<T,long int>::value == true || std::is_same<T,long long int>::value == true ||
					std::is_same<T,unsigned long>::value == true || std::is_same<T,unsigned long long>::value == true ||
					std::is_same<T,unsigned short>::value == true || std::is_same<T,size_t>::value == true ||
					std::is_same<T,short>::value == true)
			return true;
		else
			return false;
	}

	template<typename T>
	static unsigned int countDigits(T val)
	{
		unsigned int digits = 1;

		//only works for integer values
		if(isInteger(val) == true)

		{
			for(unsigned int i=10;i<std::numeric_limits<T>::max();i = i*10)
			{
				if( val/i != 0)
					++digits;
				else
					break;
			}
		}
		else
		{
			printf("ERROR: can not count digits for non-integer values\n");
			return 0;
		}

		return digits;
	}

	template<typename T>
	static bool isPrimitive()
	{
		bool is_primitive = false;

		if(std::is_same<T,int>::value == true || std::is_same<T,unsigned int>::value == true ||
			std::is_same<T,char>::value == true || std::is_same<T,unsigned char>::value == true ||
			std::is_same<T,long int>::value == true || std::is_same<T,long long int>::value == true ||
			std::is_same<T,unsigned long>::value == true || std::is_same<T,unsigned long long>::value == true ||
			std::is_same<T,float>::value == true || std::is_same<T,double>::value == true ||
			std::is_same<T,long double>::value == true || std::is_same<T,unsigned short>::value == true ||
			std::is_same<T,short>::value == true)

		{
			is_primitive = true;
		}

		return is_primitive;
	}

	/**
	 * Will set a >large< array to zero faster than memset.
	 * The quantum MUST divide the array size.
	 * */
	static void setArrayVal(unsigned char val, unsigned char* array, unsigned int array_size, unsigned int quantum)
	{
		unsigned char iterations = array_size / quantum;
		unsigned char zeropad[quantum];

		for(unsigned int i=0;i<quantum;i++)
		{
			zeropad[i] = val;
		}

		for(unsigned int i=0;i<iterations;i++)
		{
			memcpy(array+i*quantum, zeropad, quantum);
		}
	}

	/**
	 * Will set a >large< array to zero faster than memset.
	 * The quantum MUST divide the array size.
	 * */
	static void setArrayValParallel(unsigned char val, unsigned char* array, unsigned int array_size, unsigned int quantum)
	{
		unsigned char iterations = array_size / quantum;
		unsigned char zeropad[quantum];

		for(unsigned int i=0;i<quantum;i++)
		{
			zeropad[i] = val;
		}

		#pragma omp parallel for
		for(unsigned int i=0;i<iterations;i++)
		{
			memcpy(array+i*quantum, zeropad, quantum);
		}
	}

	static const unsigned int m_buffer_size = 100;

	CLASS_RTTI(Lazarus::Toolbox)
};

class Testcallee
{
public:
	Testcallee(unsigned int i);

	void p(Thread* t,void* param);

private:
	unsigned int m_i;
};

}

#endif /* TOOLBOX_H_ */
