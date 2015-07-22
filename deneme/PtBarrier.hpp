//==============================================================================
// PtBarrier.hpp - Pthread barrier synchronization object wrapper class.
//
// Author        : Vilas Kumar Chitrakaran
// Version       : 2.0 (Apr 2005)
// Compatibility : POSIX, GCC
//==============================================================================

#ifndef _PTBARRIER_HPP_INCLUDED
#define _PTBARRIER_HPP_INCLUDED

#include <pthread.h>
#include "ErrnoException.hpp"

//==============================================================================
// class PtBarrier
//------------------------------------------------------------------------------
// \brief
// The pthread barrier synchronization object.
//
// <ul>
// <li>A barrier can be created and used to synchronize a bunch of threads. A 
//     barrier object common to multiple threads blocks each of the threads 
//     until all of them have reached a certain point in their code, at which 
//     point they are all released.
// <li>This class will throw an exception of type ErrnoException in case of
//     errors.
// </ul>
// 
// <b>Example Program:</b>
// \include PtBarrier.t.cpp
//==============================================================================

class PtBarrier
{
 public:
  inline PtBarrier(int n);
   // Initialize a barrier object.
   //  n  The number of threads that must call wait() before
   //     any of them successfully returns from the call. This
   //     value must be greater than 0.
   
  inline ~PtBarrier();
   // Destroy the barrier object
   
  inline void wait();
   // Synchronize participating threads at the barrier. 
   // NOTE: For 'n' cooperating threads as specified in the constructor:
   // <ul>
   // <li> This function blocks until 'n-1' other participating threads 
   //      have called wait() on the same barrier.
   // <li> You can't unblock this function by calling wait() 'n' times 
   //      from the same thread.
   // </ul>

  //======== END OF INTERFACE ========

 private:
  inline void errorCheck(int code); // This will throw an exception on any error.
  pthread_barrier_t d_barrier;      // Barrier object
};


//==============================================================================
// PtBarrier::PtBarrier
//==============================================================================
PtBarrier::PtBarrier(int n)
{
 if (n < 1)
  n = 1;
 pthread_barrierattr_t attr;
 errorCheck( pthread_barrierattr_init(&attr) );
 pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
 errorCheck( pthread_barrier_init(&d_barrier, &attr, n) );
 pthread_barrierattr_destroy(&attr);
}


//==============================================================================
// PtBarrier::~PtBarrier
//==============================================================================
PtBarrier::~PtBarrier()
{
 pthread_barrier_destroy(&d_barrier);
}


//==============================================================================
// PtBarrier::wait
//==============================================================================
void PtBarrier::wait()
{
 pthread_barrier_wait(&d_barrier);
}


//==============================================================================
// PtBarrier::errorCheck
//==============================================================================
void PtBarrier::errorCheck(int code)
{
 if(code == 0 || code == PTHREAD_BARRIER_SERIAL_THREAD ) return;
 throw(ErrnoException(code, "[PtBarrier]"));
}

#endif // _PTBARRIER_HPP_INCLUDED
