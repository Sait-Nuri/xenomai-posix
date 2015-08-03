//==============================================================================
// AlarmXn.cpp - XENOMAI-POSIX Alarm (Xenomai watchdog timer) wrapper class.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 03.08.2015   1.0            Said Nuri UYANIK     Initial creation
// 
//==============================================================================

#include "AlarmXn.hpp"
#include "znmException.hpp"
#include <iostream>

using namespace std;

AlarmXn::AlarmXn(const char* name, RTIME value, RTIME interval){
	_alarm = new RT_ALARM;
	_initShot = value;
	_interval = interval;

	this->create(name);
}

AlarmXn::~AlarmXn(){

	destroy();
}

int AlarmXn::create(const char * name){
	int name_len;

	// Alarm ismini kopyala
	name_len = strlen(name);
	_alarmName = new char [name_len+1];
	strncpy(_alarmName, name, name_len);
	_alarmName[name_len] = '\0';

	mlockall(MCL_CURRENT|MCL_FUTURE);

	if(rt_alarm_create(_alarm, _alarmName) != 0){
		throw ZnmException("Alarm create failed", "create()", errno);
	}

	return 0;
}

int AlarmXn::destroy(){

	if(_alarmName)
		delete [] _alarmName;

	if(_alarm)
		delete _alarm;

	if(_alarmInfo)
		delete _alarmInfo;

	if(rt_alarm_delete(_alarm) != 0)
		throw ZnmException("Alarm delete failed", "destroy()", errno);

	return 0;
}

RT_ALARM_INFO* AlarmXn::inquire(RT_ALARM_INFO * info){
	
	int ret;

	if(info != NULL){
		ret = rt_alarm_inquire(_alarm, info);

		if(ret != 0)
			throw ZnmException("alarm inquire failed", "inquire()", errno);
		
		return info;
	}else{
		if(_alarmInfo == NULL){
			_alarmInfo = new RT_ALARM_INFO;
		}
		
		ret = rt_alarm_inquire(_alarm, _alarmInfo);

		if(ret != 0)
			throw ZnmException("alarm inquire failed", "inquire()", errno);
		
		return _alarmInfo;
	}
}

int AlarmXn::start (){
	int ret;

	ret = rt_alarm_start(_alarm, _initShot, _interval);

	if (ret != 0){
		//perror(":");
		throw ZnmException("alarm start failed", "start()", errno);
	}
		

	return 0;
}

int AlarmXn::stop (){
	int ret;

	ret = rt_alarm_stop(_alarm);

	if(ret != 0){
		throw ZnmException("alarm stop failed", "stop()", errno);
	}

	return 0;
}

int AlarmXn::wait (){
	int ret;

	ret = rt_alarm_wait(_alarm);

	if(ret != 0){
		cout << errno << ret << endl;
		throw ZnmException("alarm wait failed", "wait()", errno);
	}

	return 0;
}
