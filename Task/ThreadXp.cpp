//==============================================================================
// ThreadXp.cpp - Thread creation and handling.
//
// Author        : Ahmet Soyyigit
// Version       : 2.0 (July 2015)
// Compatibility : Xenomai POSIX Skin, GCC
//==============================================================================

#include "ThreadXp.hpp"
#include "znmException.hpp"
#include <iostream>

#define ERROR_CHECK_RET_THREAD(RET, FNAME) ERROR_CHECK_RET(RET, "ThreadXp", FNAME);

//==============================================================================
// ThreadXp::ThreadXp
//==============================================================================
ThreadXp::ThreadXp(int detachstate /*= PTHREAD_CREATE_JOINABLE*/, 
        size_t stacksize /*= PTHREAD_STACK_MIN*/,
        int inheritsched /*= PTHREAD_EXPLICIT_SCHED*/,
        int policy /*= SCHED_OTHER*/,
        int sched_priority /*= 0*/,             // NOT SUPPORTED
        char *name /*= NULL*/,                  // NOT SUPPORTED
        int floatingPoint /*= 1*/               // NOT SUPPORTED
        //xnarch_cpumask_t *cpuMask /*= NULL */ // NOT SUPPORTED
) 
{
 struct sched_param_ex schedParam;

 schedParam.sched_priority = sched_priority;
 d_threadId = 0;
 d_arg = NULL;
 d_threadRunning = false;
 
 ERROR_CHECK_RET_THREAD( sem_init (&d_sema ,0, 0) , 
  "sem_init");
 ERROR_CHECK_RET_THREAD( pthread_attr_init(&d_attr) ,
  "pthread_attr_init");
 ERROR_CHECK_RET_THREAD( pthread_attr_setdetachstate(&d_attr, detachstate),
  "pthread_attr_setdetachstate");
 ERROR_CHECK_RET_THREAD( pthread_attr_setstacksize(&d_attr, stacksize),
  "pthread_attr_setstacksize");
 ERROR_CHECK_RET_THREAD( pthread_attr_setinheritsched(&d_attr, inheritsched), 
  "pthread_attr_setinheritsched");
 ERROR_CHECK_RET_THREAD( pthread_attr_setschedpolicy(&d_attr, policy),
  "pthread_attr_setschedpolicy");
 //pthread_attr_setschedparam_ex (&d_attr, &schedParam);
 //pthread_attr_setname_np(&d_attr, name );
 //pthread_attr_setfp_np(&d_attr, floatingPoint);
 //if(cpuMask != NULL)
 //	pthread_attr_setaffinity_np(&d_attr, *cpuMask);

 pthread_mutexattr_t mAttr;
 ERROR_CHECK_RET_THREAD( pthread_mutexattr_init(&mAttr) , 
  "pthread_mutexattr_init"); 
 ERROR_CHECK_RET_THREAD( pthread_mutexattr_setpshared(&mAttr, PTHREAD_PROCESS_PRIVATE) , 
  "pthread_mutexattr_setpshared");
 ERROR_CHECK_RET_THREAD( pthread_mutex_init(&d_lock, &mAttr) , 
 "pthread_mutex_init"); // init mutex
 ERROR_CHECK_RET_THREAD( pthread_mutexattr_destroy(&mAttr), 
  "pthread_mutexattr_destroy");
}

//==============================================================================
// ThreadXp::~ThreadXp
//==============================================================================
ThreadXp::~ThreadXp()
{
 pthread_attr_destroy(&d_attr);
 pthread_mutex_destroy(&d_lock);
 sem_destroy (&d_sema);
}


//==============================================================================
// ThreadXp::run
//==============================================================================
int ThreadXp::run(void *arg)
{
 int code;

 ERROR_CHECK_RET_THREAD( pthread_mutex_lock(&d_lock)  ,
  "pthread_mutex_lock");
 if(d_threadRunning)
 {
  ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock)  , 
    "pthread_mutex_unlock");
  return EPERM;
 }
 d_arg = arg;
 ERROR_CHECK_RET_THREAD( code = pthread_create(&d_threadId, &d_attr, ThreadXp::threadEntry, this), 
  "pthread_create");
 if(code == 0)
  d_threadRunning = true;
 ERROR_CHECK_RET_THREAD( sem_wait (&d_sema) , 
  "sem_wait");
 ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock)  ,
  "pthread_mutex_unlock");
 return code;
}


//==============================================================================
// ThreadXp::isThreadRunning
//==============================================================================
bool ThreadXp::isThreadRunning()
{
 bool out;
 ERROR_CHECK_RET_THREAD( pthread_mutex_lock(&d_lock) ,
  "pthread_mutex_lock");
 out = d_threadRunning;
 ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock) ,
  "pthread_mutex_unlock");
 return out;
}


//==============================================================================
// ThreadXp::cancel
//==============================================================================
int ThreadXp::cancel()
{
 ERROR_CHECK_RET_THREAD( pthread_mutex_lock(&d_lock) ,
  "pthread_mutex_lock");
 if(!d_threadRunning)
 {
  ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock) ,
  "pthread_mutex_unlock");
  return ESRCH;
 }
 int code;
 d_threadRunning = false;
 code = pthread_cancel(d_threadId);
 sem_wait (&d_sema);
 ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock) ,
  "pthread_mutex_unlock");
 return code;
}

//==============================================================================
// ThreadXp::join
//==============================================================================
int ThreadXp::join()
{
 int retval = -1;
 ERROR_CHECK_RET_THREAD( pthread_mutex_lock(&d_lock) ,
  "pthread_mutex_lock");
 if(!d_threadRunning)
 {
  ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock) ,
  "pthread_mutex_unlock");
  return retval;
 }
 sem_wait (&d_sema);
 d_threadRunning = false;
 pthread_join(d_threadId, (void **)(&retval));
 ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock) ,
  "pthread_mutex_unlock");
 return retval;
}


//==============================================================================
// ThreadXp::getThreadId
//==============================================================================
pthread_t ThreadXp::getThreadId()
{
 pthread_t id;
 ERROR_CHECK_RET_THREAD( pthread_mutex_lock(&d_lock) ,
  "pthread_mutex_lock");
 id = d_threadId;
 ERROR_CHECK_RET_THREAD( pthread_mutex_unlock(&d_lock) ,
  "pthread_mutex_unlock");
 return id;
}


//==============================================================================
// ThreadXp::threadEntry
//==============================================================================
void *ThreadXp::threadEntry(void *classPtr)
{
 int code;
 // mark thread cancellable
 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
 
 // Run the thread function
 ThreadXp *tPtr = (ThreadXp *)classPtr;
 pthread_cleanup_push(&(ThreadXp::threadExit), classPtr);
 tPtr->enterThread(tPtr->d_arg);
 sem_post (&(tPtr->d_sema)); 
 code = tPtr->executeInThread(tPtr->d_arg);
 pthread_testcancel();
 pthread_cleanup_pop(1);
  
 // Exit thread
 return((void *)code);
}


//==============================================================================
// ThreadXp::threadExit
//==============================================================================
void ThreadXp::threadExit(void *classPtr)
{
 ThreadXp *tPtr = (ThreadXp *)classPtr;
 tPtr->exitThread(tPtr->d_arg);
 sem_post (&(tPtr->d_sema)); 
}
