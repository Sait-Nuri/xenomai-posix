//==============================================================================
// AlarmXn.hpp - XENOMAI-POSIX Alarm (Xenomai watchdog timer) wrapper class.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 03.08.2015   1.0            Said Nuri UYANIK     Initial creation
// 
//==============================================================================

#ifndef _ALARM_HPP_INCLUDED
#define _ALARM_HPP_INCLUDED

#define ALARM_START_OFFSET 300000000000 /* First shot at now + 3 sec */
#define ALARM_INTERVAL 100000000000 /* Period is 1 sec */

#include <string>
#include <errno.h>
#include <native/alarm.h>
#include <sys/mman.h>

class AlarmXn
{

public:
	
	AlarmXn(const char* name, RTIME value = ALARM_START_OFFSET, RTIME interval = ALARM_INTERVAL);

	~AlarmXn();

	int destroy();

	RT_ALARM_INFO* inquire(RT_ALARM_INFO * info = NULL);
	
	int start ();

	int stop ();

	int wait ();

private:

	RT_ALARM* _alarm;

	char* _alarmName;

	RT_ALARM_INFO* _alarmInfo;

	RTIME _initShot;

	RTIME _interval;

	int create ( const char * name);
};

#endif