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

#define MAXMSGLEN 1024 //Default maximum message length
#define MAXNUMMSG 128  //DefaULT maximum number of message in queue

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
	
	int create(const char *name, int maxNumMsgs, int maxMsgLen = MAXMSGLEN);

	int open(const char *name);

	int close();

	int unlink();

	int send(const char *msg_buf, int msg_size);

	int try_send(const char *msg_buf, int msg_size);

	int receive(char *msg_buf, int buf_size);

	int try_receive(char *msg_buf, int buf_size);

	int notify(const struct sigevent *notification);

	inline int getMaxNumMsgs() const { return _maxNumMsgs; };

	inline int getMaxMsgLength() const { return _maxMsgLength; };

	inline int getErrno() const { return _errno; };

	inline char* getMqName() const { return _name; };

private:

	char* _name;          // Name of the message queue
	mqd_t _desc;          // Descriptor for the queue
	int _maxNumMsgs;      // max. number of messages in queue
  	int _maxMsgLength;    // maximum length of a message in the queue
	int _errno;           // Latest error message
	struct mq_attr _attr; // attribute of message queue 

};

#endif 