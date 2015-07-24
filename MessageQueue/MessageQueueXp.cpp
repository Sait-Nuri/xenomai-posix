//==============================================================================
// MessageQueueXp.hpp - XENOMAI-POSIX message queue wrapper class.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 22.10.2015   1.0            Said Nuri UYANIK     Initial creation
// 24.10.2015                                       Thread ile çalışmada sıkıntı var
//==============================================================================

#include "MessageQueueXp.hpp"
#include "znmException.hpp"
#include <iostream>

MessageQueueXp::MessageQueueXp(const char * mq_name){
	
	this->create(mq_name, MAXNUMMSG, MAXMSGLEN);
}

MessageQueueXp::MessageQueueXp(const char* mq_name, int maxNumMsgs, int maxMsgSize){

	this->create(mq_name, maxNumMsgs, maxMsgSize);
}

MessageQueueXp::~MessageQueueXp(){
	
	if(_isOwner){
		unlink();
		//std::cout << "unlinked " << std::endl;
	}
		
		
	if(_name)
		delete [] _name;
}

int MessageQueueXp::getPrior(){

	// Get calling threads policy and parameters
	if( pthread_getschedparam( pthread_self(), &_policy, &_param) == -1){
		_errno = errno;
		//perror("pthread_getschedparam failed in constructor ");
		return -1;
	}

	_sendPrior = _param.sched_priority;

	return 0;
}

int MessageQueueXp::setAttribute(long flag){

	// Unset NONBLOCK attribute for non-blocking send/receive
	_attr.mq_flags = flag;

	if( mq_setattr(_desc, &_attr, &_prevAttr) == -1){
		//perror("setting attribute mqueue failed");
		_errno = errno;
		return -1;
	}

	// get attributes of message queue
	if(getAttribute() == -1){
		return -1;
	}

	if(flag == O_NONBLOCK) 
		_isBlocking = false; 
	else if(flag == 0) 
		_isBlocking = true;
	else 
		throw ZnmException("Unsupported flag", "setAttribute", flag);
}

int MessageQueueXp::getAttribute(){
	
	// read attribute back
	if(mq_getattr( _desc, &_attr) == -1){
		_errno = errno;
		perror("getting attribute mqueue failed");
		return -1;
	}

	_maxNumMsgs = _attr.mq_maxmsg;
	_maxMsgSize = _attr.mq_msgsize;
	(_attr.mq_flags == O_NONBLOCK) ? (_isBlocking = false) :
									 (_isBlocking = true);
	return 0;
}

int MessageQueueXp::create(const char *name, int maxNumMsgs, int maxMsgSize){

	int name_len;		

	/* Check for number of messages*/   
	if(maxMsgSize > MAXMSGLEN){ 
		maxMsgSize = MAXMSGLEN;
	}

	// Get priority and parameters of current thread
	if(getPrior() == -1)
		return -1;

	// Copy the name
	name_len = strlen(name);
	_name = new char [name_len+1];
	strncpy(_name, name, name_len);
	_name[name_len] = '\0';
 	
 	// Set attributes for creating message queue
	_attr.mq_maxmsg = maxNumMsgs;
	_attr.mq_msgsize = maxMsgSize;
	_attr.mq_flags = 0;
	
	// 	Create message queue
	_desc = mq_open(_name, 
					CREATE_AND_OPEN_FLAG, 
					PERMISSION_GROUP_MODE,
					&_attr);
	
	if(_desc != (mqd_t)-1){
		_isOwner = true;
		//std::cout << "owned " << std::endl;
	}else{ // Check for error 
		// if name already exist, unlink and try again
		if (errno == EEXIST){
			// try open
			if(open(name) == -1)
				return -1;

			_isOwner = false;

		}else{
			// Another unknown error
			_errno = errno;
			ZnmException("Creating message queue", "Create()", _errno);
		}
	}

	if(getAttribute() == -1)
		throw ZnmException("Getting attribute failed");
	
	_errno = 0;
    return 0;
}

int MessageQueueXp::open(const char *name){
	
	//open message queue
	_desc = mq_open(_name, OPEN_FLAG);
		
	if(_desc == (mqd_t)-1){
		_errno = errno;
		throw ZnmException("Opening message queue failed", "open()", _errno);
	}

	//std::cout << "opened " << std::endl;

	_errno = 0;
	return 0;
}

int MessageQueueXp::close(){
	
	// Check for mq is already closed
	if(_desc == (mqd_t)-1)
		return 0;

	// Close mq and check for error
	if( mq_close(_desc) == -1 ){
		_errno = errno;
		return -1;
	}

	// if closing success, 
	_desc = (mqd_t)-1;
	_errno = 0;

	return 0;
}

int MessageQueueXp::unlink(){

	// close descriptor before unlink
	if( close() == -1 ){
		throw ZnmException("closing before unlink message queue failed", "unlink()", _errno);
	}
  	
  	// unlink message queue
	if( mq_unlink(_name) == -1){
		_errno = errno;
		throw ZnmException("unlink message queue failed", "mq_unlink", _errno);
	}

	_isOwner = false;
	_errno = 0;
	return 0;
}

int MessageQueueXp::send(const char *msg_buf, int msg_size, const struct timespec * timeout){
	
	// if blocking is not available, make it non-blocking
	if(!_isBlocking){

		// Now blocking send/receive is available
		if(setAttribute(0) == -1)
			return -1;
	}

	if(timeout == NULL){
		// Send message to mqueue 
		if( mq_send( _desc, msg_buf, msg_size, _sendPrior) == -1){
			_errno = errno;
			throw ZnmException("sending message failed", "send()", _errno);
		}
	}else{
		// Send message to mqueue 
		if( mq_timedsend( _desc, msg_buf, msg_size, _sendPrior, timeout) == -1){
			_errno = errno;
			throw ZnmException("timedsend message failed", "send()", _errno);
		}
	}
	

	_errno = 0;
	return 0;
}

int MessageQueueXp::try_send(const char *msg_buf, int msg_size){
	
	// If blocking is available, make it non-blocking
	if(_isBlocking){

		// Non-blocking send/receive is available
		if(setAttribute(O_NONBLOCK) == -1)
			return -1;
	}

	if(mq_send(_desc, msg_buf, msg_size, _sendPrior) == -1){
		_errno = errno;
		throw ZnmException("sending message failed", "send()", _errno);
	}

	_errno = 0;
	return 0;
}

int MessageQueueXp::receive(char *msg_buf, int buf_size, const struct timespec * timeout){
	int ret_val;

	if(!_isBlocking){

		if(setAttribute(0) == -1)
			return -1;
	}

	if(timeout == NULL){
		ret_val = mq_receive(_desc, msg_buf, buf_size, &_receivedPrior);
	}else{
		ret_val = mq_timedreceive(_desc, msg_buf, buf_size, &_receivedPrior, timeout);
	}
	
	if(ret_val == -1){
		// No enough buffer to store received message
		if( errno == EMSGSIZE ){  
			//std::cout << "msg_buf " << msg_buf << " buf_size: " << buf_size << ": " << strlen(msg_buf) << std::endl; 
			//getAttribute();

			//std::cout << "max: " << _maxMsgSize << " :" << _attr.mq_msgsize << std::endl;

			throw ZnmException("No enough buffer for received message", "receive()", EMSGSIZE);
		}

		_errno = errno;
		throw ZnmException("Receiving message failed", "receive()", _errno);
	}
	
	_errno = 0;
	return ret_val;
}

int MessageQueueXp::try_receive(char *msg_buf, int buf_size){
	
	int ret_val;

	if(_isBlocking){

		if(setAttribute(O_NONBLOCK))
			return -1;	
	}

	ret_val = mq_receive(_desc, msg_buf, buf_size, &_receivedPrior);

	if(ret_val == -1){
		// No enough buffer to store received message
		if( errno == EMSGSIZE ){ 
			throw ZnmException("No enough buffer for received message", "try receive()", EMSGSIZE);
		}

		_errno = errno;
		return -1;
	}
	
	_errno = 0;
	return ret_val;
}

int MessageQueueXp::notify(const struct sigevent *notification){
	if( mq_notify(_desc, notification) == -1 ){
		_errno = errno;
		throw ZnmException("Event notification failed", "notify()", _errno);
	}
	
	_errno = 0;
	return 0;
}

int MessageQueueXp::getMsgNum(){
	
	getAttribute();

	return _attr.mq_curmsgs;
}