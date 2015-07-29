//==============================================================================
// CondVariableXp.hpp - Xenomai POSIX skin condition variable wrapper class.
//
// Author        : Ahmet Soyyigit
// Version       : 1.0 (2015)
// Compatibility : XENOMAI POSIX SKIN, GCC
//==============================================================================

#ifndef _CONDVARIABLEXP_HPP_INCLUDED
#define _CONDVARIABLEXP_HPP_INCLUDED

#include <pthread.h>
#include <time.h>
#include "znmException.hpp"
#include "MutexXp.hpp"

//==============================================================================
// class CondVariableXp
//------------------------------------------------------------------------------
// \brief
// A wrapper for pthread condition variable.
//
// <ul>
// <li>This class will throw an exception of type ZnmException in case of
//     errors.
// </ul>
//
// <b>Example Program:</b>
//
// CondVariableXp.t.cpp
//==============================================================================

class CondVariableXp {
public:
	CondVariableXp (clockid_t cld_id = CLOCK_REALTIME,
					int pshared = PTHREAD_PROCESS_PRIVATE);

	~CondVariableXp ();

	void condWait(MutexXp *mutex);

	int condTimedWait(MutexXp *mutex, const struct timespec *abstime);

	void condSignal();

	void condBroadcast();

private:
	pthread_cond_t condVar;
    // The mutex object

};

// Constructor
CondVariableXp::CondVariableXp (clockid_t clk_id, int pshared ){
	pthread_condattr_t attr;

	ERROR_CHECK_RET (pthread_condattr_init (&attr), "CondVariableXp", "pthread_condattr_init");

	ERROR_CHECK_RET (pthread_condattr_setclock  (&attr, clk_id), "CondVariableXp", "pthread_condattr_setclock");
	ERROR_CHECK_RET (pthread_condattr_setpshared (&attr, pshared), "CondVariableXp", "pthread_condattr_setpshared");

	ERROR_CHECK_RET (pthread_cond_init (&condVar, &attr), "CondVariableXp", "pthread_cond_init");

	ERROR_CHECK_RET (pthread_condattr_destroy (&attr), "CondVariableXp", "pthread_condattr_destroy");
}

// Destructor
CondVariableXp::~CondVariableXp (){
	ERROR_CHECK_RET (pthread_cond_destroy (&condVar), "CondVariableXp", "pthread_cond_destroy");
}

void CondVariableXp::condWait(MutexXp *mutex){
	ERROR_CHECK_RET (pthread_cond_wait (&condVar, &(mutex->d_mutex)), "CondVariableXp", "pthread_cond_wait");
}

int CondVariableXp::condTimedWait(MutexXp *mutex, const struct timespec *abstime){
	int errNumber;

	errNumber = pthread_cond_timedwait (&condVar, &(mutex->d_mutex), abstime);

	if (errNumber == 0)
		return 0;
	else if(errNumber == ETIMEDOUT)
		return -1;
	throw(ZnmException( "CondVariableXp", "pthread_cond_timedwait", errNumber));
}

void CondVariableXp::condSignal() {
	ERROR_CHECK_RET ( pthread_cond_signal (&condVar), "CondVariableXp", " pthread_cond_signal");
}

void CondVariableXp::condBroadcast() {
	ERROR_CHECK_RET ( pthread_cond_broadcast (&condVar), "CondVariableXp", " pthread_cond_broadcast");
}


#endif // _CONDVARIABLEXP_HPP_INCLUDED