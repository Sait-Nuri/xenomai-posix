//==============================================================================
// Thread.cpp - Thread creation and handling.
//
// Author        : Vilas Kumar Chitrakaran
// Version       : 2.0 (Apr 2005)
// Compatibility : POSIX, GCC
//==============================================================================

#include "Thread.hpp"
#include <iostream>

//==============================================================================
// Thread::Thread
//==============================================================================
Thread::Thread()
{
 d_threadId = 0;
 d_arg = NULL;
 d_threadRunning = false;
 d_barrier = new PtBarrier(2);
 errorCheck( pthread_attr_init(&d_attr) );
 errorCheck( pthread_attr_setinheritsched(&d_attr, PTHREAD_INHERIT_SCHED) );
 errorCheck( pthread_attr_setdetachstate(&d_attr, PTHREAD_CREATE_JOINABLE) );
 
 pthread_mutexattr_t mAttr;
 errorCheck( pthread_mutexattr_init(&mAttr) ); // init to default attributes
 errorCheck( pthread_mutexattr_setpshared(&mAttr, PTHREAD_PROCESS_PRIVATE) );
 errorCheck( pthread_mutex_init(&d_lock, &mAttr) ); // init mutex
 pthread_mutexattr_destroy(&mAttr);
}

//==============================================================================
// Thread::~Thread
//==============================================================================
Thread::~Thread()
{
 pthread_attr_destroy(&d_attr);
 pthread_mutex_destroy(&d_lock);
 delete d_barrier;
}


//==============================================================================
// Thread::run
//==============================================================================
int Thread::run(void *arg)
{
 int code;

 errorCheck( pthread_mutex_lock(&d_lock) );
 if(d_threadRunning)
 {
  errorCheck( pthread_mutex_unlock(&d_lock) );
  return EPERM;
 }
 d_arg = arg;
 code = pthread_create(&d_threadId, &d_attr, Thread::threadEntry, this);
 if(code == 0)
  d_threadRunning = true;
 d_barrier->wait();
 errorCheck( pthread_mutex_unlock(&d_lock) );
 return code;
}


//==============================================================================
// Thread::isThreadRunning
//==============================================================================
bool Thread::isThreadRunning()
{
 bool out;
 errorCheck( pthread_mutex_lock(&d_lock) );
 out = d_threadRunning;
 errorCheck( pthread_mutex_unlock(&d_lock) );
 return out;
}


//==============================================================================
// Thread::cancel
//==============================================================================
int Thread::cancel()
{
 errorCheck( pthread_mutex_lock(&d_lock) );
 if(!d_threadRunning)
 {
  errorCheck( pthread_mutex_unlock(&d_lock) );
  return ESRCH;
 }
 int code;
 d_threadRunning = false;
 code = pthread_cancel(d_threadId);
 d_barrier->wait();
 errorCheck( pthread_mutex_unlock(&d_lock) );
 return code;
}

//==============================================================================
// Thread::join
//==============================================================================
int Thread::join()
{
 int retval = -1;
 errorCheck( pthread_mutex_lock(&d_lock) );
 if(!d_threadRunning)
 {
  errorCheck( pthread_mutex_unlock(&d_lock) );
  return retval;
 }
 d_barrier->wait();
 d_threadRunning = false;
 pthread_join(d_threadId, (void **)(&retval));
 errorCheck( pthread_mutex_unlock(&d_lock) );
 return retval;
}


//==============================================================================
// Thread::getThreadId
//==============================================================================
pthread_t Thread::getThreadId()
{
 pthread_t id;
 errorCheck( pthread_mutex_lock(&d_lock) );
 id = d_threadId;
 errorCheck( pthread_mutex_unlock(&d_lock) );
 return id;
}


//==============================================================================
// Thread::threadEntry
//==============================================================================
void *Thread::threadEntry(void *classPtr)
{
 int code;
 // mark thread cancellable
 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
 
 // Run the thread function
 Thread *tPtr = (Thread *)classPtr;
 pthread_cleanup_push(&(Thread::threadExit), classPtr);
 tPtr->enterThread(tPtr->d_arg);
 tPtr->d_barrier->wait();
 code = tPtr->executeInThread(tPtr->d_arg);
 pthread_testcancel();
 pthread_cleanup_pop(1);
  
 // Exit thread
 return((void *)code);
}


//==============================================================================
// Thread::threadExit
//==============================================================================
void Thread::threadExit(void *classPtr)
{
 Thread *tPtr = (Thread *)classPtr;
 tPtr->exitThread(tPtr->d_arg);
 tPtr->d_barrier->wait();
}


//==============================================================================
// Thread::errorCheck()
//==============================================================================
void Thread::errorCheck(int code)
{
 if(code == 0) return;
 throw(ErrnoException(code),"[Thread]");
}
