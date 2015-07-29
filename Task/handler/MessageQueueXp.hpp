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

#include <mqueue.h>
#include <string>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#define MAXNUMMSG 128  //DefaULT maximum number of message in queue
#define MAXMSGLEN 128 //Default maximum message length
#define PERMISSION_GROUP_MODE S_IRWXU | S_IRWXG | S_IRWXO //read/write/execute for enyone
#define CREATE_AND_OPEN_FLAG O_CREAT | O_EXCL | O_RDWR  // Create
#define OPEN_FLAG O_RDWR


class MessageQueueXp
{
public:

	/** 
	 * mq_name: name of message queue will be created.
	 =================================================*/

	MessageQueueXp(const char* mq_name);
	
	/** 
	 * mq_name: name of message queue will be created.
	 * maxNumMsgs: maximum nuber of messages
	 * maximum size of a message
	 =================================================*/
    
	MessageQueueXp(const char* mq_name, int maxNumMsgs, int maxMsgSize);
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

	int send(const char *msg_buf, int msg_size, const struct timespec * timeout = NULL);

	int try_send(const char *msg_buf, int msg_size);

	int receive(char *msg_buf, int buf_size, const struct timespec * timeout = NULL);

	int try_receive(char *msg_buf, int buf_size);

	int notify(const struct sigevent *notification);

	int getMsgNum();

	inline int getMaxNumMsgs() const { return _maxNumMsgs; };

	inline int getMaxMsgLength() const { return _maxMsgSize; };

	inline int getErrno() const { return _errno; };

	inline char* getMqName() const { return _name; };

private:

	char* _name;               // Name of the message queue
	mqd_t _desc;               // Descriptor for the queue
	int _maxNumMsgs;           // max. number of messages in queue
  	int _maxMsgSize;           // maximum size of a message in the queue
	int _errno;                // Latest error message
	struct mq_attr _attr;      // attribute of message queue 
	struct mq_attr _prevAttr;  // Keeps previous attribute of mq when set new attributes
	int _policy;               // scheduling policy of calling thread
	struct sched_param _param; // parameters of calling thread
	bool _isBlocking;          // Blocking caller thread is available or not for mq_send(), mq_receive() ...
	bool _isOwner;             // Owner of mqeueu or not
	unsigned _sendPrior;       // Priority of message to send
	unsigned _receivedPrior;   // Priority of received message

	int create(const char *name, int maxNumMsgs, int maxMsgSize = MAXMSGLEN);

	int open(const char *name);

	int unlink();

	int close();

	int getPrior();

	int setAttribute(long flag);

	int getAttribute();
};

#endif 