//==============================================================================
// Thread.t.cpp - Example program for Thread class.
//
// Author        : Vilas Kumar Chitrakaran
//==============================================================================

#include "Thread.hpp"
#include "RWLock.hpp"
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

//==============================================================================
// MyThread class
//==============================================================================
class MyThread : public Thread
{
 public:
  MyThread();
  ~MyThread();
  void setVal(int p);
  int getVal();
 protected:
  virtual void enterThread(void *arg);
  virtual int executeInThread(void *arg);
  virtual void exitThread(void *arg);
 private:
  int d_val;
  RWLock d_lock;
};

//-----------------------------------------------------------------------------
// MyThread::MyThread
//-----------------------------------------------------------------------------
MyThread::MyThread() 
{
 d_val = 0; 
}

//-----------------------------------------------------------------------------
// MyThread::~MyThread
//-----------------------------------------------------------------------------
MyThread::~MyThread()
{
 cancel();
}

//-----------------------------------------------------------------------------
// MyThread::enterThread
//-----------------------------------------------------------------------------
void MyThread::enterThread(void *arg)
{
 arg=arg;
 cout << "thread: in entry routine" << endl;
}

//-----------------------------------------------------------------------------
// MyThread::executeInThread
//-----------------------------------------------------------------------------
int MyThread::executeInThread(void *arg)
{
 int i = 0;
 struct timespec napTime;
 napTime.tv_sec = 0;
 napTime.tv_nsec = (long int)5e8;
 
 while(1)//i < 2)
 {
  i = ((MyThread *)arg)->getVal();
  cout << "thread: reading value = " << i << endl;
  nanosleep(&napTime, NULL);
  pthread_testcancel();
 }
 
 return i;
}

//-----------------------------------------------------------------------------
// MyThread::setVal
//-----------------------------------------------------------------------------
void MyThread::setVal(int p) 
{
 d_lock.writeLock();
 d_val = p; 
 d_lock.unlock();
}

//-----------------------------------------------------------------------------
// MyThread::getVal
//-----------------------------------------------------------------------------
int MyThread::getVal() 
{
 int i;
 d_lock.readLock();
 i = d_val;
 d_lock.unlock();
 return i; 
};

//-----------------------------------------------------------------------------
// MyThread::cleanupInThread
//-----------------------------------------------------------------------------
void MyThread::exitThread(void *arg)
{
 arg=arg;
 cout << "thread: in cleanup routine" << endl;
}


//==============================================================================
// main function
//==============================================================================
int main()
{
 MyThread thread;
 struct timespec napTime;
 napTime.tv_sec = 1;
 napTime.tv_nsec = 0;
 int err;
 int i = 0;
 
 // Start the thread
 err = thread.run(&thread);
 if( err != 0)
 {
  cout << "parent: Thread creation failed: " << strerror(err) << endl;
  return -1;
 }
 
 // Yield to allow the thread to run
 sched_yield();

 // Change data in parent thread
 while(i < 3)
 {
  cout << "parent: setting value = " << i << endl;
  thread.setVal(i);
  nanosleep(&napTime,NULL);
  i++;
 }
 
 // wait for thread to finish
 //err = thread.join();
 if( err != 0)
 {
  cout << "parent: thread retuned: " << err << endl;
 }
 cout << "parent: exiting" << endl;
 return 0;
}
