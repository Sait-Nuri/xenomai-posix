//==============================================================================
// MutexXp.hpp - POSIX mutex wrapper class.
//
// Author        : Ahmet Soyyigit
// Version       : 1.0 (2015)
// Compatibility : XENOMAI POSIX SKIN, GCC
//==============================================================================

#ifndef _MUTEXXP_HPP_INCLUDED
#define _MUTEXXP_HPP_INCLUDED

#include <pthread.h>
#include <time.h>
#include "znmException.hpp"

//==============================================================================
// class MutexXp
//------------------------------------------------------------------------------
// \brief
// A wrapper for pthread mutex, with the added functionality that it is
// recursive.
//
// <ul>
// <li>This class is useful if the thread is already in a mutex protected
//     section of the code and needs to call another routine that locks
//     the same mutex again.
// <li>This class will throw an exception of type ZnmException in case of
//     errors.
// </ul>
//
// <b>Example Program:</b>
//
// MutexXp.t.cpp
//==============================================================================

class MutexXp
{
 friend class CondVariableXp;
 public:
  inline MutexXp(int type =  PTHREAD_MUTEX_DEFAULT,
                int proto = PTHREAD_PRIO_NONE,
                int pshared = PTHREAD_PROCESS_PRIVATE);
   // Constructs a MutexXp
 
  inline ~MutexXp();
   // Deletes a MutexXp

  inline void lock();
   // Locks a mutex. If the mutex is locked by another thread, 
   // this thread is blocked until the mutex gets unlocked.

  inline void unlock();
   // Unlocks a mutex.

  inline int tryLock();
   // Returns 0 and locks the mutex if it is not already locked by another 
   // thread, else returns -1.

  inline int timedLock(const struct timespec *to);

  //======== END OF INTERFACE ========

 private:
  
  pthread_mutex_t d_mutex;
   // The mutex object
};


//==============================================================================
// MutexXp::MutexXp()
//==============================================================================
MutexXp::MutexXp(int type /*=  PTHREAD_MUTEX_DEFAULT*/,
                int proto /*= PTHREAD_PRIO_NONE*/,
                int pshared /*= PTHREAD_PROCESS_PRIVATE*/
                )
{
 pthread_mutexattr_t mAttr;
  
 ERROR_CHECK_RET( pthread_mutexattr_init(&mAttr) , "MutexXp", "pthread_mutexattr_init"); 
 
 // The following are actually default.
 ERROR_CHECK_RET( pthread_mutexattr_settype (&mAttr, type) , "MutexXp", "pthread_mutexattr_settype");
 ERROR_CHECK_RET( pthread_mutexattr_setprotocol(&mAttr, proto) , "MutexXp", "pthread_mutexattr_setprotocol");
 ERROR_CHECK_RET( pthread_mutexattr_setpshared(&mAttr, pshared) , "MutexXp", "pthread_mutexattr_setpshared");

 ERROR_CHECK_RET( pthread_mutex_init(&d_mutex, &mAttr) , "MutexXp", "pthread_mutex_init"); // init mutex

 ERROR_CHECK_RET( pthread_mutexattr_destroy(&mAttr) , "MutexXp", "pthread_mutexattr_destroy");
}


//==============================================================================
// MutexXp::~MutexXp()
//==============================================================================
MutexXp::~MutexXp()
{
 ERROR_CHECK_RET( pthread_mutex_destroy(&d_mutex), "MutexXp", "pthread_mutex_destroy");
}


//==============================================================================
// MutexXp::lock()
//==============================================================================
void MutexXp::lock()
{
 ERROR_CHECK_RET( pthread_mutex_lock(&d_mutex) , "MutexXp", "pthread_mutex_lock");
}


//==============================================================================
// MutexXp::unlock()
//==============================================================================
void MutexXp::unlock()
{
 ERROR_CHECK_RET( pthread_mutex_unlock(&d_mutex) , "MutexXp", "pthread_mutex_unlock");
}


//==============================================================================
// MutexXp::tryLock()
//==============================================================================
int MutexXp::tryLock()
{
 int errNumber;

 errNumber = pthread_mutex_trylock(&d_mutex);
 if (errNumber == 0)
  return 0;
 else if(errNumber == EBUSY)
  return -1;
 throw(ZnmException("MutexXp", "pthread_mutex_trylock", errNumber));
 
}


//==============================================================================
// MutexXp::timedLock(const struct timespec *to)
//==============================================================================
int MutexXp::timedLock(const struct timespec *to)
{
 int errNumber;

 errNumber = pthread_mutex_timedlock (&d_mutex, to);
 
 if (errNumber == 0)
  return 0;
 else if(errNumber == ETIMEDOUT)
  return -1;
 throw(ZnmException( "MutexXp", "pthread_mutex_timedlock", errNumber));
}



#endif // MUTEXXP_HPP_INCLUDED
