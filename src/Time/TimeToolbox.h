/*
 * TimeToolbox.h
 *
 *  Created on: 22.03.2013
 *      Author: Darius Malysiak
 */

#ifndef TIMETOOLBOX_H_
#define TIMETOOLBOX_H_

#include "../Config.h"
#include <string>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdint.h>

#include "../Serializable.h"

//---------------------- defines
extern struct timespec* __GLOBAL_TIMER0_TICK_A;
extern struct timespec* __GLOBAL_TIMER0_TICK_B;
extern struct timespec* __GLOBAL_TIMER0_DIFF;
extern long double __avg_time0_;
extern unsigned long __step_counter0___;
extern struct timespec* __GLOBAL_TIMER1_TICK_A;
extern struct timespec* __GLOBAL_TIMER1_TICK_B;
extern struct timespec* __GLOBAL_TIMER1_DIFF;
extern long double __avg_time1_;
extern unsigned long __step_counter1___;
extern struct timespec* __GLOBAL_TIMER2_TICK_A;
extern struct timespec* __GLOBAL_TIMER2_TICK_B;
extern struct timespec* __GLOBAL_TIMER2_DIFF;
extern long double __avg_time2_;
extern unsigned long __step_counter2___;
extern struct timespec* __GLOBAL_TIMER3_TICK_A;
extern struct timespec* __GLOBAL_TIMER3_TICK_B;
extern struct timespec* __GLOBAL_TIMER3_DIFF;
extern long double __avg_time3_;
extern unsigned long __step_counter3___;
extern struct timespec* __GLOBAL_TIMER4_TICK_A;
extern struct timespec* __GLOBAL_TIMER4_TICK_B;
extern struct timespec* __GLOBAL_TIMER4_DIFF;
extern long double __avg_time4_;
extern unsigned long __step_counter4___;
extern struct timespec* __GLOBAL_TIMER5_TICK_A;
extern struct timespec* __GLOBAL_TIMER5_TICK_B;
extern struct timespec* __GLOBAL_TIMER5_DIFF;
extern long double __avg_time5_;
extern unsigned long __step_counter5___;
extern struct timespec* __GLOBAL_TIMER6_TICK_A;
extern struct timespec* __GLOBAL_TIMER6_TICK_B;
extern struct timespec* __GLOBAL_TIMER6_DIFF;
extern long double __avg_time6_;
extern unsigned long __step_counter6___;
extern struct timespec* __GLOBAL_TIMER7_TICK_A;
extern struct timespec* __GLOBAL_TIMER7_TICK_B;
extern struct timespec* __GLOBAL_TIMER7_DIFF;
extern long double __avg_time7_;
extern unsigned long __step_counter7___;


	//#define FILL_TICK(tick) clock_gettime(CLOCK_REALTIME, tick);
    #define FILL_TICK(tick) clock_gettime(CLOCK_MONOTONIC, tick);

	//------------------------------------ global timer TIC/TOC (we nee to use two-level macro expansion)
	#define GLOBAL_TIC(NUM) GLOBAL_TIC___(NUM)
	#define GLOBAL_TIC___(NUM) __GLOBAL_TIMER##NUM##_TICK_A = Lazarus::TimeToolbox::createTick(); \
				__GLOBAL_TIMER##NUM##_TICK_B = Lazarus::TimeToolbox::createTick(); \
				__GLOBAL_TIMER##NUM##_DIFF = Lazarus::TimeToolbox::createTick(); \
				FILL_TICK(__GLOBAL_TIMER##NUM##_TICK_A);

	#define GLOBAL_TOC(NUM) GLOBAL_TOC___(NUM)
	#define GLOBAL_TOC___(NUM) FILL_TICK(__GLOBAL_TIMER##NUM##_TICK_B); \
				Lazarus::TimeToolbox::tickDiff(__GLOBAL_TIMER##NUM##_TICK_A,__GLOBAL_TIMER##NUM##_TICK_B,__GLOBAL_TIMER##NUM##_DIFF); \
				printf("Process took = %.9Lf s\n",Lazarus::TimeToolbox::getTime(__GLOBAL_TIMER##NUM##_DIFF)); \
				Lazarus::TimeToolbox::freeTicks(&__GLOBAL_TIMER##NUM##_TICK_A,&__GLOBAL_TIMER##NUM##_TICK_B); \
				FREE_NULL_CHECKING(__GLOBAL_TIMER##NUM##_DIFF);

	//-----------------------

	#define GLOBAL_TIC_(NUM) GLOBAL_TIC__(NUM)
	#define GLOBAL_TIC__(NUM) FILL_TICK(__GLOBAL_TIMER##NUM##_TICK_A);

	#define GLOBAL_TOC_(NUM) GLOBAL_TOC__(NUM)
	#define GLOBAL_TOC__(NUM) FILL_TICK(__GLOBAL_TIMER##NUM##_TICK_B); \
				Lazarus::TimeToolbox::tickDiff(__GLOBAL_TIMER##NUM##_TICK_A,__GLOBAL_TIMER##NUM##_TICK_B,__GLOBAL_TIMER##NUM##_DIFF); \
				printf("Process took = %.9Lf s\n",Lazarus::TimeToolbox::getTime(__GLOBAL_TIMER##NUM##_DIFF));

	//-----------------------
	#define GLOBAL_INIT_TIME_AVG(NUM) GLOBAL_INIT_TIME_AVG___(NUM)
	#define GLOBAL_INIT_TIME_AVG___(NUM) __GLOBAL_TIMER##NUM##_TICK_A = Lazarus::TimeToolbox::createTick(); \
								__GLOBAL_TIMER##NUM##_TICK_B = Lazarus::TimeToolbox::createTick(); \
								__GLOBAL_TIMER##NUM##_DIFF = Lazarus::TimeToolbox::createTick(); \
							__avg_time##NUM##_ = 0; \
							__step_counter##NUM##___ = 0; \

	#define GLOBAL_AVG_TOC(NUM) GLOBAL_AVG_TOC_(NUM)
	#define GLOBAL_AVG_TOC_(NUM)	FILL_TICK(__GLOBAL_TIMER##NUM##_TICK_B); \
					Lazarus::TimeToolbox::tickDiff(__GLOBAL_TIMER##NUM##_TICK_A,__GLOBAL_TIMER##NUM##_TICK_B,__GLOBAL_TIMER##NUM##_DIFF); \
					__avg_time##NUM##_ += Lazarus::TimeToolbox::getTime(__GLOBAL_TIMER##NUM##_DIFF); \
					__step_counter##NUM##___++;

	#define GLOBAL_SUM_AVG_TIME_UP(NUM) GLOBAL_SUM_AVG_TIME_UP_(NUM)
	#define GLOBAL_SUM_AVG_TIME_UP_(NUM)	printf("Process took an average time of %.9Lf s\n",__avg_time##NUM##_ / (long double)__step_counter##NUM##___); \
			Lazarus::TimeToolbox::freeTicks(&__GLOBAL_TIMER##NUM##_TICK_A,&__GLOBAL_TIMER##NUM##_TICK_B); \
			FREE_NULL_CHECKING(__GLOBAL_TIMER##NUM##_DIFF);

	#define GLOBAL_INIT_TIME_AVG_(NUM) GLOBAL_INIT_TIME_AVG__(NUM)
	#define GLOBAL_INIT_TIME_AVG__(NUM) 	__GLOBAL_TIMER##NUM##_TICK_A = Lazarus::TimeToolbox::createTick(); \
							__GLOBAL_TIMER##NUM##_TICK_B = Lazarus::TimeToolbox::createTick(); \
							__GLOBAL_TIMER##NUM##_DIFF = Lazarus::TimeToolbox::createTick(); \
							__avg_time##NUM##_ = 0; \
							__step_counter##NUM##___ = 0; \

	//------------------------------------ local timer TIC/TOC
	#define TIC struct timespec* a = Lazarus::TimeToolbox::createTick(); \
				struct timespec* b = Lazarus::TimeToolbox::createTick(); \
				struct timespec* diff = Lazarus::TimeToolbox::createTick(); \
				FILL_TICK(a);

	#define TOC FILL_TICK(b); \
				Lazarus::TimeToolbox::tickDiff(a,b,diff); \
				printf("Process took = %.9Lf s (%s:%s:%d)\n",Lazarus::TimeToolbox::getTime(diff),__FILE__,__func__,__LINE__); \
				Lazarus::TimeToolbox::freeTicks(&a,&b); \
				FREE_NULL_CHECKING(diff);

	#define TOCLOG(logbook) FILL_TICK(b); \
				Lazarus::TimeToolbox::tickDiff(a,b,diff); \
				char buffer[1000]; \
				sprintf(buffer,"Process took = %.9Lf s (%s:%s:%d)\n",Lazarus::TimeToolbox::getTime(diff),__FILE__,__func__,__LINE__); \
				logbook.addEntry(buffer); \
				Lazarus::TimeToolbox::freeTicks(&a,&b); \
				FREE_NULL_CHECKING(diff);

	#define TIC_ FILL_TICK(a);

	#define TOC_ FILL_TICK(b); \
				Lazarus::TimeToolbox::tickDiff(a,b,diff); \
				printf("Process took = %.9Lf s (%s:%s:%d)\n",Lazarus::TimeToolbox::getTime(diff),__FILE__,__func__,__LINE__);

	#define TOC_LOG(logbook) FILL_TICK(b); \
				Lazarus::TimeToolbox::tickDiff(a,b,diff); \
				char buffer[1000]; \
				sprintf(buffer,"Process took = %.9Lf s (%s:%s:%d)\n",Lazarus::TimeToolbox::getTime(diff),__FILE__,__func__,__LINE__); \
				logbook.addEntry(buffer);

	#define INIT_TIME_AVG 	struct timespec* a = Lazarus::TimeToolbox::createTick(); \
							struct timespec* b = Lazarus::TimeToolbox::createTick(); \
							struct timespec* diff = Lazarus::TimeToolbox::createTick(); \
							long double avg_time = 0; \
							unsigned long step_counter___ = 0; \

    #define AVG_TIC FILL_TICK(a);

	#define AVG_TOC	FILL_TICK(b); \
					Lazarus::TimeToolbox::tickDiff(a,b,diff); \
					avg_time += Lazarus::TimeToolbox::getTime(diff); \
					step_counter___++;

	#define SUM_AVG_TIME_UP	printf("Process took an average time of %.9Lf s\n",avg_time / (long double)step_counter___); \
			Lazarus::TimeToolbox::freeTicks(&a,&b); \
			FREE_NULL_CHECKING(diff);

	#define SUM_AVG_TIME_UP_LOG	char buffer[1000]; \
			sprintf(buffer,"Process took an average time of %.9Lf s (%s:%s:%d)\n",Lazarus::TimeToolbox::getTime(diff),__FILE__,__func__,__LINE__); \
			logbook.addEntry(buffer); \
			Lazarus::TimeToolbox::freeTicks(&a,&b); \
			FREE_NULL_CHECKING(diff);

	#define INIT_TIME_AVG_ 	a = Lazarus::TimeToolbox::createTick(); \
							b = Lazarus::TimeToolbox::createTick(); \
							diff = Lazarus::TimeToolbox::createTick(); \
							avg_time = 0; \
							step_counter___ = 0; \


namespace Lazarus
{

class Timestamp : public Serializable
{

public:

	Timestamp()
	{
		m_epoch=0;
		m_month=0;
		m_day=0;
		m_year=0;
		m_hour=0;
		m_second=0;
		m_minute=0;
		m_ms=0;
		m_us=0;
		m_ns=0;
	}

	Timestamp(Timestamp* t)
	{
		m_epoch=t->m_epoch;
		m_month=t->m_month;
		m_day=t->m_day;
		m_year=t->m_year;
		m_hour=t->m_hour;
		m_second=t->m_second;
		m_minute=t->m_minute;
		m_ms=t->m_ms;
		m_us=t->m_us;
		m_ns=t->m_ns;
	}

	/**
	 * Prints the timestamp attributes to the console.
	 * */
	void print();

	void serialize()
	{
		resetBuffer();
		registerElement<unsigned int>(9);
		registerElement<unsigned long long>(1);
		allocateBuffer();

		//serialize the attributes
		this->addElement(m_epoch);
		this->addElement(m_month);
		this->addElement(m_day);
		this->addElement(m_year);
		this->addElement(m_hour);
		this->addElement(m_second);
		this->addElement(m_minute);
		this->addElement(m_ms);
		this->addElement(m_us);
		this->addElement(m_ns);
	}

	void deserialize()
	{
		m_ns = this->getUInt();
		m_us = this->getUInt();
		m_ms = this->getUInt();
		m_minute = this->getUInt();
		m_second = this->getUInt();
		m_hour = this->getUInt();
		m_year = this->getUInt();
		m_day = this->getUInt();
		m_month = this->getUInt();
		m_epoch = this->getULongLong();

		Lazarus::Serializable::resetBuffer();
	}

	//seconds since unix epoch
	unsigned long long m_epoch;
	//normal dates
	unsigned int m_month;
	unsigned int m_day;
	unsigned int m_year;
	unsigned int m_hour;
	unsigned int m_second;
	unsigned int m_minute;
	unsigned int m_ms;
	unsigned int m_us;
	unsigned int m_ns;

	/**
	 * This will simply subtract the members of ts from the corresponding ones
	 * in current object. If one member difference would yield a negative value the
	 * resulting diff will be clamped to 0.
	 * It will return a new object.
	 * */
	Timestamp operator-(const Timestamp& ts)
	{
		Timestamp diff;

		if(this->m_epoch > ts.m_epoch)
			diff.m_epoch = this->m_epoch - ts.m_epoch;

		if(this->m_month > ts.m_month)
			diff.m_month = this->m_month - ts.m_month;
		if(this->m_day > ts.m_day)
			diff.m_day = this->m_day - ts.m_day;
		if(this->m_year > ts.m_year)
			diff.m_year = this->m_year - ts.m_year;
		if(this->m_hour > ts.m_hour)
			diff.m_hour = this->m_hour - ts.m_hour;
		if(this->m_second > ts.m_second)
			diff.m_second = this->m_second - ts.m_second;
		if(this->m_minute > ts.m_minute)
			diff.m_minute = this->m_minute - ts.m_minute;
		if(this->m_ms > ts.m_ms)
			diff.m_ms = this->m_ms - ts.m_ms;
		if(this->m_us > ts.m_us)
			diff.m_us = this->m_us - ts.m_us;
		if(this->m_ns > ts.m_ns)
			diff.m_ns = this->m_ns - ts.m_ns;

		return diff;
	}

	/**
	 * This will return a new object.
	 */
	Timestamp operator+(const Timestamp& ts)
	{
		Timestamp sum;

		sum.m_epoch = this->m_epoch + ts.m_epoch;
		sum.m_month = this->m_month + ts.m_month;
		sum.m_day = this->m_day + ts.m_day;
		sum.m_year = this->m_year + ts.m_year;
		sum.m_hour = this->m_hour + ts.m_hour;
		sum.m_second = this->m_second + ts.m_second;
		sum.m_minute = this->m_minute + ts.m_minute;
		sum.m_ms = this->m_ms + ts.m_ms;
		sum.m_us = this->m_us + ts.m_us;
		sum.m_ns = this->m_ns + ts.m_ns;

		return sum;
	}

};

class TimeToolbox {
public:
	TimeToolbox();
	virtual ~TimeToolbox();

	struct date
	{
		date()
		{
			m_hour=0;
			m_minute=0;
			m_second=0;
			m_ms=0;
			m_us=0;

			m_day=0;
			m_month=0;
			m_year=0;

			m_days_in_year=0;
			m_day_in_week=0;
			m_daylight_saving_time=0;

			m_gmt_offset=0;

			m_epoch_s=0;
			m_epoch_us=0;
		}

		unsigned int m_hour;
		unsigned int m_minute;
		unsigned int m_second;
		unsigned int m_ms;
		unsigned long long m_us;

		unsigned int m_day;
		unsigned int m_month;
		unsigned int m_year;

		unsigned int m_days_in_year;
		unsigned int m_day_in_week;
		int m_daylight_saving_time;//1=summer time, 0= winter time, -1= system decides(careful!!)

		long long int m_gmt_offset;/* Seconds east of UTC.  */

		//these values are epoch relative, they can be used to calculate the difference between to dates.
		unsigned long long m_epoch_s;
		unsigned long long m_epoch_us;
	};

	/**
	 * Returns the timer (CLOCK_MONOTONIC) resolution in ns.
	 * */
	static unsigned long long int getTimerResolution();

	/**
	 * Returns the time in ts in s-units.
	 * */
	static long double getTime(struct timespec* ts);

	/**
	 * Calculates the difference between start and end, stores the result in diff.
	 * Use this method to calculate the difference between ticks.
	 * */
	static void tickDiff(struct timespec* start, struct timespec* end, struct timespec* diff);

	/**
	 * Will calculate the time difference by using the epoch values. The result will be stored in
	 * the epoch attributes of diff.
	 * Be aware that this won't take DST into account!
	 * */
	static void dateDiff(struct date& start, struct date& end, struct date& diff);

	static struct timespec* createTick();
	static void freeTicks(struct timespec** a, struct timespec** b);
	static unsigned long long int rdtsc(void); //__cdecl for windows

	/**
	 * Updates the attributes in parameter out to the current time.
	 * */
	static void getLocalTime(struct date& out);

	/**
	 * Converts the date within in to epoch values in out, this also implies that in and out can
	 * refer to the same object since only the epoch attributes will be modified.
	 * */
	static void convertDateToEpoch(struct date& in,struct date& out);

	/**
	 * Converts the epoch within in to date values in out, this also implies that in and out can
	 * refer to the same object since only the date attributes will be modified.
	 * */
	static void convertEpochToDate(struct date& in,struct date& out);

	/**
	 * Returns a timestamp which is empty in case of errors. If gmt_zone==true all time values are in GMT+0,
	 * otherwise the time is given in the local timezone.
	 * */
	static Timestamp getTimeStamp(bool gmt_zone = false);

	/**
	 * Updates a timestamp which is empty in case of errors. If gmt_zone==true all time values are in GMT+0,
	 * otherwise the time is given in the local timezone.
	 * */
	static int updateTimeStamp(Timestamp& ts, bool gmt_zone = false);


	/**
	 * Will return the CPU time which the thread used so far in microseconds.
	 * */
	static unsigned long long thread_time();

};

}

#endif /* TIMETOOLBOX_H_ */
