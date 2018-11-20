/*
 * TimeToolbox.cpp
 *
 *  Created on: 22.03.2013
 *      Author: Darius Malysiak
 */

#include "TimeToolbox.h"

#include <sys/time.h>

//init the global timers
struct timespec* __GLOBAL_TIMER0_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER0_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER0_DIFF = NULL;
long double __avg_time0_ = 0;
unsigned long __step_counter0___ = 0;
struct timespec* __GLOBAL_TIMER1_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER1_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER1_DIFF = NULL;
long double __avg_time1_ = 0;
unsigned long __step_counter1___ = 0;
struct timespec* __GLOBAL_TIMER2_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER2_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER2_DIFF = NULL;
long double __avg_time2_ = 0;
unsigned long __step_counter2___ = 0;
struct timespec* __GLOBAL_TIMER3_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER3_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER3_DIFF = NULL;
long double __avg_time3_ = 0;
unsigned long __step_counter3___ = 0;
struct timespec* __GLOBAL_TIMER4_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER4_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER4_DIFF = NULL;
long double __avg_time4_ = 0;
unsigned long __step_counter4___ = 0;
struct timespec* __GLOBAL_TIMER5_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER5_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER5_DIFF = NULL;
long double __avg_time5_ = 0;
unsigned long __step_counter5___ = 0;
struct timespec* __GLOBAL_TIMER6_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER6_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER6_DIFF = NULL;
long double __avg_time6_ = 0;
unsigned long __step_counter6___ = 0;
struct timespec* __GLOBAL_TIMER7_TICK_A = NULL;
struct timespec* __GLOBAL_TIMER7_TICK_B = NULL;
struct timespec* __GLOBAL_TIMER7_DIFF = NULL;
long double __avg_time7_ = 0;
unsigned long __step_counter7___ = 0;

namespace Lazarus
{

TimeToolbox::TimeToolbox() {
	// TODO Auto-generated constructor stub
}

TimeToolbox::~TimeToolbox() {
	// TODO Auto-generated destructor stub
}

// high precision time measurement functions
void TimeToolbox::getLocalTime(struct date& out)
{
	timeval curTime;
	gettimeofday(&curTime, NULL);

	struct tm time1;
	localtime_r(&curTime.tv_sec,&time1);

	out.m_day = time1.tm_mday;
	out.m_hour = time1.tm_hour;
	out.m_minute = time1.tm_min;
	out.m_month = time1.tm_mon+1;
	out.m_ms = curTime.tv_usec / 1000;
	out.m_second = time1.tm_sec;
	out.m_us = curTime.tv_usec-out.m_ms*1000;
	out.m_year = time1.tm_year + 1900;
	out.m_epoch_s = curTime.tv_sec;
	out.m_epoch_us = curTime.tv_usec;

	out.m_day_in_week = time1.tm_wday;
	out.m_days_in_year = time1.tm_yday;
	out.m_daylight_saving_time = time1.tm_isdst;

	out.m_gmt_offset = time1.tm_gmtoff;
}

unsigned long long int TimeToolbox::getTimerResolution()
{
    struct timespec tp;
    clock_getres(CLOCK_MONOTONIC, &tp);
    return 1000000000 * (unsigned long)tp.tv_sec + (unsigned long) (tp.tv_nsec);
}

long double TimeToolbox::getTime(struct timespec* ts)
{
    return (long double)(ts->tv_sec + 0.000000001*ts->tv_nsec);
}

struct timespec* TimeToolbox::createTick()
{
	return (struct timespec*) malloc(sizeof(struct timespec));
}

void TimeToolbox::freeTicks(struct timespec** a, struct timespec** b)
{
	FREE_NULL_CHECKING(*a);
	FREE_NULL_CHECKING(*b);
}

void TimeToolbox::tickDiff(struct timespec* start, struct timespec* end, struct timespec* diff)
{

	//if an overflow occurred, e.g. 3.5s - 2.9s, subtract 1s from the seconds diff and subtract the negative ns diff from 1.000.000.000 ns
    if((end->tv_nsec-start->tv_nsec)<0)
    {
    	diff->tv_sec = end->tv_sec-start->tv_sec-1;
    	diff->tv_nsec = 1000000000l + end->tv_nsec-start->tv_nsec;
    }
    else
    {
    	diff->tv_sec = end->tv_sec-start->tv_sec;
    	diff->tv_nsec = end->tv_nsec-start->tv_nsec;
    }
}

void TimeToolbox::dateDiff(struct date& start, struct date& end, struct date& diff)
{

	//if an overflow occurred, e.g. 3.5s - 2.9s, subtract 1s from the seconds diff and subtract the negative ns diff from 1.000.000.000 ns
    if((end.m_epoch_us - start.m_epoch_us)<0)
    {
    	diff.m_epoch_s = end.m_epoch_s - start.m_epoch_s -1;
    	diff.m_epoch_us = 1000000l + end.m_epoch_us-start.m_epoch_us;
    }
    else
    {
    	diff.m_epoch_s = end.m_epoch_s - start.m_epoch_s;
    	diff.m_epoch_us = end.m_epoch_us - start.m_epoch_us;
    }
}

void TimeToolbox::convertDateToEpoch(struct date& in,struct date& out)
{
	struct tm time1;

	time1.tm_mday = in.m_day;
	time1.tm_hour = in.m_hour;
	time1.tm_min = in.m_minute;
	time1.tm_mon = in.m_month-1;
	time1.tm_sec = in.m_second;
	time1.tm_year= in.m_year - 1900;

	time1.tm_gmtoff = in.m_gmt_offset;
	time1.tm_wday = in.m_day_in_week;
	time1.tm_yday = in.m_days_in_year;
	time1.tm_isdst = in.m_daylight_saving_time;

	time_t temp = mktime( &time1 );

	out.m_epoch_s = temp;
	out.m_epoch_us = in.m_ms*1000 + in.m_us;//add the last fraction
}

void TimeToolbox::convertEpochToDate(struct date& in,struct date& out)
{
	timeval curTime;
	curTime.tv_sec = in.m_second;
	curTime.tv_usec = in.m_ms*1000 + in.m_us;

	struct tm time1;
	localtime_r(&curTime.tv_sec,&time1);


	out.m_day = time1.tm_mday;
	out.m_hour = time1.tm_hour;
	out.m_minute = time1.tm_min;
	out.m_month = time1.tm_mon+1;
	out.m_second = time1.tm_sec;
	out.m_year = time1.tm_year + 1900;
	out.m_ms = in.m_ms;
	out.m_us = in.m_us;

	out.m_gmt_offset = time1.tm_gmtoff;
	out.m_day_in_week = time1.tm_wday;
	out.m_days_in_year = time1.tm_yday;
	out.m_daylight_saving_time = time1.tm_isdst;

}

void Timestamp::print()
{
	printf("Epoch seconcs: %llu , %u-%u-%u, %u:%u:%u:%u:%u:%u\n",
			m_epoch,
			m_year,
			m_month,
			m_day,
			m_hour,
			m_second,
			m_minute,
			m_ms,
			m_us,
			m_ns);
}

Timestamp TimeToolbox::getTimeStamp(bool gmt_zone)
{
	Timestamp ts;

	//timeval curTime;
	timespec curTime;

	//gettimeofday(&curTime, NULL);
	clock_gettime(CLOCK_REALTIME,&curTime);

	//int milli = curTime.tv_usec / 1000;

	tm* gmtm = NULL;

	if(gmt_zone == false)
	{
		gmtm = localtime(&curTime.tv_sec);
	}
	else
	{
		gmtm = gmtime(&curTime.tv_sec);
	}

	ts.m_epoch=curTime.tv_sec;
	ts.m_month=gmtm->tm_mon+1;
	ts.m_day=gmtm->tm_mday;
	ts.m_year=gmtm->tm_year + 1900;
	ts.m_hour=gmtm->tm_hour;
	ts.m_second=gmtm->tm_sec;
	ts.m_minute=gmtm->tm_min;
	ts.m_ms=curTime.tv_nsec / 1000000;
	ts.m_us=(curTime.tv_nsec % 1000000) / 1000;
	ts.m_ns=curTime.tv_nsec % 1000;

	return ts;
}

int TimeToolbox::updateTimeStamp(Timestamp& ts, bool gmt_zone)
{
	//timeval curTime;
	timespec curTime;

	//gettimeofday(&curTime, NULL);
	int res = clock_gettime(CLOCK_REALTIME,&curTime);
	if(res != 0)
		return -1;

	//int milli = curTime.tv_usec / 1000;

	tm* gmtm = NULL;

	if(gmt_zone == false)
	{
		gmtm = localtime(&curTime.tv_sec);
	}
	else
	{
		gmtm = gmtime(&curTime.tv_sec);
	}

	ts.m_epoch=curTime.tv_sec;
	ts.m_month=gmtm->tm_mon+1;
	ts.m_day=gmtm->tm_mday;
	ts.m_year=gmtm->tm_year + 1900;
	ts.m_hour=gmtm->tm_hour;
	ts.m_second=gmtm->tm_sec;
	ts.m_minute=gmtm->tm_min;
	ts.m_ms=curTime.tv_nsec / 1000000;
	ts.m_us=(curTime.tv_nsec % 1000000) / 1000;
	ts.m_ns=curTime.tv_nsec % 1000;

	return 0;
}


unsigned long long int TimeToolbox::rdtsc(void)
{
	/*unsigned long long int x;
	 __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;*/
	unsigned a, d;
	__asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
	return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}

unsigned long long TimeToolbox::thread_time()
{
   struct rusage ru;
   struct timeval t;
   getrusage(RUSAGE_THREAD,&ru);
   t = ru.ru_utime;
   return (unsigned long long)( t.tv_sec*1000 + t.tv_usec/1000 );
}


}
