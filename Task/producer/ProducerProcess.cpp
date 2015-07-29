//==============================================================================
// ProducerProcess.cpp 
//
// Author        :
// Version       : 2.0 (July 2015)
// Compatibility : Xenomai POSIX Skin, GCC
//==============================================================================

/*
	TASK MODEL

	|Producer Process|  <--- Message Queue ---> |Handler Task| ---> Shared Memory <--- |Consumer Task|
*/


#include "ThreadXp.hpp"
#include "MessageQueueXp.hpp"
#include <stdlib.h> 
#include <iostream>
#include <unistd.h>

#define MESSAGE_QUEUE "/mal7"

using namespace std;

class ProducerProcess : public ThreadXp
{
public:
  ProducerProcess();
  ~ProducerProcess();

  	int sendMessage(int number);
protected:
  virtual void enterThread(void *arg);
  virtual int executeInThread(void *arg);
  virtual void exitThread(void *arg);
private:
  	MessageQueueXp _mq1;
	int _number; 
	char _ch;
};

int main(void) {

	ProducerProcess myTask;

	myTask.run();

	myTask.join();

	return 0;
}

ProducerProcess::ProducerProcess() : _mq1(MESSAGE_QUEUE){
	_number = 0;
	_ch = '0';
}

ProducerProcess::~ProducerProcess(){

}

int ProducerProcess::sendMessage(int number){

	_ch = (char)(_number + 48);
	return _mq1.send(&_ch, sizeof(_ch));
}

void ProducerProcess::enterThread(void *arg){
	
	cerr << "enter p" << endl << flush;

	/* initialize random seed: */
	srand (time(NULL));
}

int ProducerProcess::executeInThread(void *arg){
	
	cerr << "execute p" << endl << flush;

	int counter = 0;

	while( 1  ){
		
		_number = rand() % 10;

		if(sendMessage(_number) == 0)
			cerr << "gönderdim: " << _number << endl << flush;
		else 
			perror("burada bir hata var ");

		sleep(1);
	}
}

void ProducerProcess::exitThread(void *arg){
	cerr << "exit p" << endl << flush;
}