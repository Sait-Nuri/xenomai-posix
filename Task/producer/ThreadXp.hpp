//==============================================================================
// ThreadXp.hpp - Thread creation and handling.
//
// Author        : Ahmet Soyyigit
// Version       : 2.0 (July 2015)
// Compatibility : Xenomai POSIX Skin, GCC
//==============================================================================

#ifndef _THREADXP_HPP_INCLUDED
#define _THREADXP_HPP_INCLUDED

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>


//==============================================================================
// class Thread
//------------------------------------------------------------------------------
// \brief
// A mechanism to execute code in a separate thread.
//
// This is a pure virtual base class for threads. Users must reimplement 
// atleast the enterThread(), executeInThread() and exitThread() functions in 
// a derived class to use this wrapper over POSIX threads. 
// 
// This class is based partly on the idea presented by Ryan Teixeira in
// http://www.geocities.com/SiliconValley/Heights/6038/dthreads.html
//
// <b>Example Program:</b>
// \include Thread.t.cpp
//==============================================================================

class ThreadXp
{
 public:
 
  ThreadXp(int detachstate = PTHREAD_CREATE_JOINABLE, 
        size_t stacksize = PTHREAD_STACK_MIN,
        int inheritsched = PTHREAD_EXPLICIT_SCHED,
        int policy = SCHED_OTHER,
        int sched_priority = 0,               // NOT SUPPORTED
        char *name = NULL,                    // NOT SUPPORTED
        int floatingPoint = 1);               // NOT SUPPORTED
       // xnarch_cpumask_t *cpuMask = NULL ); // NOT SUPPORTED
   // The constructor. Does some initializations.
  	
  virtual ~ThreadXp();
    // This destructor does nothing. NOTE: If the derived 
    // class does not have a shutdown routine that calls 
    // cancel() and join() to wait for the thread to 
    // exit cleanly after executing the clean up routine 
    // (see exitThread()), the destructor of the derived 
    // class should call cancel() and join(). 
 
  int run(void *arg = NULL);
   // Start the thread with arguments 'arg'.
   // <br><hr>
   // <ul>
   // <li> This function blocks until enterThread() finishes
   //      executing in the thread. 
   // </ul>
   // <hr><br>
   //  arg     A pointer to arguments passed to the new
   //          thread.
   //  return  0 on success, and errno code on error
   //          (EPERM if thread is already running). 
  
  bool isThreadRunning();
   //  return  true if thread is running, else false.
   
  int cancel();
   // Request cancellation of execution of the thread.
   // Cancel requests are held pending until a cancellation
   // point in the thread is reached ( see man pages for
   // pthread_setcanceltype() and pthread_testcancel() ).
   //  return  0 on success, and errno code on error
   //          (ESRCH if thread is already cancelled).
  
  int join();
   // Wait until thread finishes execution. 
   //  return  return value from the thread, or -1 if 
   //          thread already exited (most probably  
   //          due to a call to cancel).
  
  pthread_t getThreadId();
   //  return  Thread ID if the thread is already 
   //           running, else 0.
    
 protected:
  virtual void enterThread(void *arg) = 0;
   // As soon as the run() function is called and the thread is
   // instantiated, this function gets called in the separate thread.
   // Override this function in the derived class to do thread
   // setup operations such as setting thread priority, scheduling 
   // policy and so on.
   //  arg     Arguments passsed by the call to run().
   
  virtual int executeInThread(void *arg) = 0;
   // This is the main function that gets executed in a 
   // separate thread. Override this function in your derived class.
   //  arg     Arguments passsed by the call to run().
   //  return  Your choice of return value.

  virtual void exitThread(void *arg) = 0;
   // This is a routine that gets executed in the thread just 
   // before it terminates (due to a call to cancel(), pthread_exit()
   // and so on). Put your cleanup code here by overriding this 
   // function in your derived class.
   //  arg     Arguments passsed by the call to run().

  //======== END OF INTERFACE ========

 private:
  static void *threadEntry(void *classPtr);
   // An entry point for the new thread.
  
  static void threadExit(void *classPtr);
   // Calls cleanup routine for the thread.

  pthread_t d_threadId;
   // Thread ID
  
  void *d_arg;
   // Thread arguments
  
  pthread_attr_t d_attr;
   // A thread attributes
  
  bool d_threadRunning;
   // Thread run status
  
  sem_t d_sema;
   // Semaphore to wait on thread exit
  
  pthread_mutex_t d_lock;
   // lock for exclusive access to functions
  
};
#endif // _THREADXP_HPP_INCLUDED