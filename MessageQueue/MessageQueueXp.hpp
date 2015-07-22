//==============================================================================
// MessageQueueXp.hpp - XENOMAI-POSIX message queue wrapper class.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 22.10.2015   1.0            Said Nuri UYANIK     Initial creation
// 
//==============================================================================

#ifndef _MESSAGEQUEUE_HPP_INCLUDED
#define _MESSAGEQUEUE_HPP_INCLUDED

#define DEFAULT_MAXMSGLEN 1024 //Default maximum message length

#include <mqueue.h>
#include <string>
#include <errno.h>
#include <pthread.h>

class MessageQueueXp
{
public:
	
	/** 
	 *
	 * Default constructor.
	 */ 
	MessageQueueXp();
	
	/** 
	 *
	 * Default destructor deletes the message queue if it was created.
	 */ 
	~MessageQueueXp();

	/** 
	 *
	 * Creates a message queue object with read/write access. 
	 *  
	 */
	
	int create(const char *name, int maxNumMsgs, int maxMsgLen = DEFAULT_MAXMSGLEN);

	
};

#endif 