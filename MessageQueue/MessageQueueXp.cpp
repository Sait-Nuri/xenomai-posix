//==============================================================================
// MessageQueueXp.hpp - XENOMAI-POSIX message queue wrapper class.
//
// Author          : Said Nuri UYANIK
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//==============================================================================
#include "MessageQueueXp.hpp"


MessageQueueXp::MessageQueueXp(){
	
	_name = NULL;
	_desc = (mqd_t)-1;
	_maxNumMsgs = 0;
	_maxMsgLength = 0;
	_errno = 0;
}

MessageQueueXp::~MessageQueueXp(){
	
	if(_name)
		delete [] _name;
}

int MessageQueueXp::create(const char *name, int maxNumMsgs, int maxMsgLen){

	if(maxMsgLen > MAXMSGLEN){ // Exception is needed
		return -1;
	}

	
}