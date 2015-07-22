//==============================================================================
// RWLock.hpp - Reader/Writer lock wrapper class.
//
// Author        : Vilas Kumar Chitrakaran
// Version       : 2.0 (Apr 2005)
// Compatibility : POSIX, GCC
//==============================================================================

#ifndef _RWLOCK_HPP_INCLUDED
#define _RWLOCK_HPP_INCLUDED

#include <pthread.h>
#include "ErrnoException.hpp"

//==============================================================================
// class RWLock
//------------------------------------------------------------------------------
// \brief
// The pthread reader-writer lock.
//
// <ul>
// <li>A Reader-Writer lock allows concurrent access to multiple processes for 
//     reading shared data, but restricts writing to shared data only when 
//     no readers are present.
// <li>Conversely, when a writer has access to shared data, 
//     all other writers and readers are blocked until the writer is done.
// <li>This class will throw an exception of type ErrnoException in case of
//     errors.
// </ul>
//
// <b>Example Program:</b>
// \include RWLock.t.cpp
//==============================================================================

class RWLock
{
 public:
  inline RWLock();
   // Constructor initializes the lock.
   
  inline ~RWLock();
   // Destroys the lock.
   
  inline void readLock();
   // Acquire the shared lock for read access. 
   // If the lock is not available, block until it is.
   
  inline int tryReadLock();
   // Try to acquire the shared lock for read access. 
   // If the lock is not available, return immediately.
   //  return  0 on successful acquisition of lock, else -1

  inline void writeLock();
   // Acquire the shared lock for exclusive write access. 
   // If the lock is not available, block until it is.

  inline int tryWriteLock();
   // Try to acquire the shared lock for exclusive write access. 
   // If the lock is not available, return immediately.
   //  return  0 on successful acquisition of lock, else -1

  inline void unlock();
   // Unlock the shared lock. If the calling thread doesn't own
   // the lock, the behavior of this function is undefined.
   
  //======== END OF INTERFACE ========
 private:
  inline void errorCheck(int code);
   // This will throw an exception on any error.

  pthread_rwlock_t d_rwl;
   // The pthread lock
  
};


//==============================================================================
// RWLock::RWLock
//==============================================================================
RWLock::RWLock()
{
 pthread_rwlockattr_t attr;
 errorCheck( pthread_rwlockattr_init(&attr) );
 errorCheck( pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE) );
 errorCheck( pthread_rwlock_init(&d_rwl, &attr) );
 pthread_rwlockattr_destroy(&attr);
}


//==============================================================================
// RWLock::~RWLock
//==============================================================================
RWLock::~RWLock()
{
 pthread_rwlock_destroy(&d_rwl);
}


//==============================================================================
// RWLock::readLock
//==============================================================================
void RWLock::readLock()
{
 errorCheck( pthread_rwlock_rdlock(&d_rwl) );
}


//==============================================================================
// RWLock::tryReadLock
//==============================================================================
int RWLock::tryReadLock()
{
 int retVal;

 retVal = pthread_rwlock_tryrdlock(&d_rwl);

 if( retVal == 0 )
  return 0;
  
 if( retVal == EAGAIN || retVal == EBUSY ) 
  return -1;
 
 errorCheck(retVal);
 return -1;
}


//==============================================================================
// RWLock::writeLock
//==============================================================================
void RWLock::writeLock()
{
 errorCheck( pthread_rwlock_wrlock(&d_rwl) );
}


//==============================================================================
// RWLock::tryWriteLock
//==============================================================================
int RWLock::tryWriteLock()
{
 int retVal;
 retVal = pthread_rwlock_trywrlock(&d_rwl);
 
 if( retVal == 0 )
  return 0;
 
 if( retVal == EAGAIN || retVal == EBUSY ) 
  return -1;
 
 errorCheck(retVal);
 return -1;
}


//==============================================================================
// RWLock::unlock
//==============================================================================
void RWLock::unlock()
{
 errorCheck( pthread_rwlock_unlock(&d_rwl) );
}


//==============================================================================
// RWLock::errorCheck
//==============================================================================
void RWLock::errorCheck(int code)
{
 if(code == 0) return;
 throw(ErrnoException(code), "[RWLock]");
}

#endif // _RWLOCK_HPP_INCLUDED
