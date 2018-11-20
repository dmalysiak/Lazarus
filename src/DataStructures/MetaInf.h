/*
 * MetaInf.h
 *
 *  Created on: 13.09.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_DATASTRUCTURES_METAINF_H_
#define CORE_DATASTRUCTURES_METAINF_H_

namespace Lazarus {

/**
 * A class / trait which holds meta information.
 * this is only the dummy for all specializations.
 * Setting all member functions to abstract ensures that one has to implement
 * dedicated specializations, since an instantiation of this dummy leads to an abstract class...
 * */
template<typename T>
class MetaInf: public BaseObject {
public:
	static T getMax();
	static T getMin();
	static T getMean();
};

/**
 * Default template specializations with meta informations for primitive data types.
 * */

template<>
class MetaInf<int>: public BaseObject {
public:
	 static int getMax() {return std::numeric_limits<int>::max();}
	 static int getMin() {return std::numeric_limits<int>::min();}
	 static int getMean() {return (getMax()-getMin()) / (int)2;}
};

template<>
class MetaInf<unsigned int>: public BaseObject {
public:
	static unsigned int getMax() {return std::numeric_limits<unsigned int>::max();}
	static unsigned int getMin() {return std::numeric_limits<unsigned int>::min();}
	static unsigned int getMean() {return (getMax()-getMin()) / (unsigned int)2;}
};

template<>
class MetaInf<short>: public BaseObject {
public:
	static short getMax() {return std::numeric_limits<short>::max();}
	static short getMin() {return std::numeric_limits<short>::min();}
	static short getMean() {return (getMax()-getMin()) / (short)2;}
};

template<>
class MetaInf<unsigned short>: public BaseObject {
public:
	static unsigned short getMax() {return std::numeric_limits<unsigned short>::max();}
	static unsigned short getMin() {return std::numeric_limits<unsigned short>::min();}
	static unsigned short getMean() {return (getMax()-getMin()) / (unsigned short)2;}
};

template<>
class MetaInf<char>: public BaseObject {
public:
	 static char getMax() {return std::numeric_limits<char>::max();}
	 static char getMin() {return std::numeric_limits<char>::min();}
	 static char getMean() {return (getMax()-getMin()) / (char)2;}
};

template<>
class MetaInf<unsigned char>: public BaseObject {
public:
	static unsigned char getMax() {return std::numeric_limits<unsigned char>::max();}
	static unsigned char getMin() {return std::numeric_limits<unsigned char>::min();}
	static unsigned char getMean() {return (getMax()-getMin()) / (unsigned char)2;}
};

template<>
class MetaInf<long>: public BaseObject {
public:
	static long getMax() {return std::numeric_limits<long>::max();}
	static long getMin() {return std::numeric_limits<long>::min();}
	static long getMean() {return (getMax()-getMin()) / (long)2;}
};


template<>
class MetaInf<unsigned long>: public BaseObject {
public:
	static unsigned long getMax() {return std::numeric_limits<unsigned long>::max();}
	static unsigned long getMin() {return std::numeric_limits<unsigned long>::min();}
	static unsigned long getMean() {return (getMax()-getMin()) / (unsigned long)2;}
};


template<>
class MetaInf<long long>: public BaseObject {
public:
	static long long getMax() {return std::numeric_limits<long long>::max();}
	static long long getMin() {return std::numeric_limits<long long>::min();}
	static long long getMean() {return (getMax()-getMin()) / (long long)2;}
};

template<>
class MetaInf<unsigned long long>: public BaseObject {
public:
	static unsigned long long getMax() {return std::numeric_limits<unsigned long long>::max();}
	static unsigned long long getMin() {return std::numeric_limits<unsigned long long>::min();}
	static unsigned long long getMean() {return (getMax()-getMin()) / (unsigned long long)2;}
};

template<>
class MetaInf<float>: public BaseObject {
public:
	static float getMax() {return std::numeric_limits<float>::max();}
	static float getMin() {return std::numeric_limits<float>::min();}
	static float getMean() {return (getMax()-getMin()) / (float)2;}
};

template<>
class MetaInf<double>: public BaseObject {
public:
	static double getMax() {return std::numeric_limits<double>::max();}
	static double getMin() {return std::numeric_limits<double>::min();}
	static double getMean() {return (getMax()-getMin()) / (double)2;}
};

} /* namespace Lazarus */

#endif /* CORE_DATASTRUCTURES_METAINF_H_ */
