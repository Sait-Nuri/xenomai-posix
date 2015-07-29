//==============================================================================
// HandlerTask.cpp 
//
// Author        : 
// Version       : 2.0 (July 2015)
// Compatibility : Xenomai POSIX Skin, GCC
//==============================================================================

/*
	TASK MODEL

	|Producer Process|  <--- Message Queue ---> |Handler Task| ---> Shared Memory <--- |Consumer Task|
*/

#include <cstring>
#include "ThreadXp.hpp"
#include "MutexXp.hpp"
#include "CondVariableXp.hpp"
#include "ShMemXp.hpp"
#include "MessageQueueXp.hpp"

#define BUFFER_SIZE 20
#define M_QUEUE "/mal7"

using namespace std;

MutexXp mutex;
CondVariableXp condVar;

class HandlerTask : public ThreadXp
{
public:
	HandlerTask();
  	~HandlerTask();

protected:
	virtual void enterThread(void *arg);
 	virtual int executeInThread(void *arg);
 	virtual void exitThread(void *arg);

private:
	ShMemXp shmNumOfElem;
	ShMemXp shmBuffer;
  	MessageQueueXp _mq1;

	int* _numOfElem;
	int* _buffer;
};

class ConsumerTask : public ThreadXp
{
public:
  	ConsumerTask();
    ~ConsumerTask();
protected:
	virtual void enterThread(void *arg);
	virtual int executeInThread(void *arg);
	virtual void exitThread(void *arg);
private:
	ShMemXp shmNumOfElem;
	ShMemXp shmBuffer;

	int* _numOfElem;
	int* _buffer;
  
};

int main(void) {
	HandlerTask MyHandler;
	ConsumerTask MyConsumer;

	MyHandler.run(&MyHandler);
	MyConsumer.run(&MyConsumer);

	MyHandler.join();
	MyConsumer.join();

	return 0;
}

HandlerTask::HandlerTask() : 
			shmNumOfElem("/NumOfElemShm", sizeof(int)), 
			shmBuffer("/BufferShm", sizeof(int) * BUFFER_SIZE),
			_mq1(M_QUEUE){

	_numOfElem = (int*) shmNumOfElem.getShmAddr();

	_buffer = (int*) shmBuffer.getShmAddr();



}

HandlerTask::~HandlerTask(){ }


void HandlerTask::enterThread(void *arg){
	cerr << "enter h" << endl << flush;
}

int HandlerTask::executeInThread(void *arg){
	
	cerr << "execute h" << endl << flush;

	int msg = 5;
	struct timespec delay;

 	delay.tv_sec = 0;
 	delay.tv_nsec = (long int)2e8;

 	char msg_buffer[512];

	while(1){
		
		_mq1.receive(msg_buffer, 512);

		msg = (int)(msg_buffer[0] - '0');

		cerr << "aldım ulan: " << msg << endl;

		mutex.lock();
		while(*_numOfElem == BUFFER_SIZE)
			condVar.condWait(&mutex);
		_buffer[(*_numOfElem)++] = msg;
		condVar.condSignal();
		mutex.unlock();
		nanosleep(&delay, NULL);
	}

	return 0;
}

void HandlerTask::exitThread(void *arg){ 
	shmNumOfElem.unlink();
	shmBuffer.unlink();

	cerr << "exit h" << endl << flush;
}
ConsumerTask::ConsumerTask() : 
			shmNumOfElem("/NumOfElemShm", sizeof(int)), 
			shmBuffer("/BufferShm", sizeof(int) * BUFFER_SIZE){

	_numOfElem = (int*) shmNumOfElem.getShmAddr();

	_buffer = (int*) shmBuffer.getShmAddr();
}

ConsumerTask::~ConsumerTask(){ }


void ConsumerTask::enterThread(void *arg){ 
	cerr << "enter c" << endl << flush;
}

int ConsumerTask::executeInThread(void *arg){
	int msg;
	struct timespec delay;

 	delay.tv_sec = 0;
 	delay.tv_nsec = (long int)3e8;

 	cerr << "execute c" << endl << flush;

	while(1){
		mutex.lock();
		while(*_numOfElem == 0)
			condVar.condWait(&mutex);
		msg = _buffer[--(*_numOfElem)];
		cerr << "message: " << msg << endl;
		condVar.condSignal();
		mutex.unlock();
		nanosleep(&delay, NULL);
	}

	return 0;
}

void ConsumerTask::exitThread(void *arg){ 
	shmNumOfElem.unlink();
	shmBuffer.unlink();

	cerr << "exit c" << endl << flush;
}
