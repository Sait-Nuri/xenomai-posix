//==============================================================================
// ErrnoException.hpp - Generic Exception handling.
//
// Author        : Vilas Kumar Chitrakaran
// Version       : 2.0 (Apr 2005)
// Compatibility : POSIX, GCC
//==============================================================================

#ifndef _ERRNOEXCEPTION_HPP_INCLUDED
#define _ERRNOEXCEPTION_HPP_INCLUDED

#include <errno.h>
#include <unistd.h>
#include <string.h>

//==============================================================================
// class ErrnoException
//------------------------------------------------------------------------------
// \brief
// A mechanism for basic run-time exception handling.
//
// <b>Example Program:</b>
// \include ErrnoException.t.cpp
//==============================================================================

class ErrnoException
{
 public:
  inline ErrnoException();
   // Standard constructor sets error to 0 (no error)
  
  inline ErrnoException(int error, const char *desc=NULL);
   // This constructor allows initialization
   //  error  set integer error code. (0 reserved for no error)
   //  desc   set a short description [less than
   //         40 chars], possibly just the object that
   //         set the error.
   
  inline ErrnoException(const ErrnoException &e);
   // Copy constructor
   
  inline ErrnoException &operator=(const ErrnoException &e);
   // Assignment operation 
  
  ~ErrnoException(){};
   // Destructor does nothing
   
  inline void setError(int error, const char *desc=NULL);
   // Set an error
   //  error  set integer error code. (0 reserved for no error)
   //  desc   set a short description [less than
   //         40 chars], possibly just the object that
   //         set the error.
   
  inline int getErrorCode() const;
   //  return  latest error code. (0 means no error).
   
  inline const char *getErrorDesc() const;
   //  return  any descriptive message that was set with the 
   //          error.  

  //======== END OF INTERFACE ========

 private:
  int d_errno;     // error number
  char d_desc[40]; // description
};


//==============================================================================
// ErrnoException::ErrnoException
//==============================================================================
ErrnoException::ErrnoException()
{
 d_errno = 0;
 d_desc[0]='\0';
}


ErrnoException::ErrnoException(int error, const char *desc)
{
 setError(error, desc);
}


ErrnoException::ErrnoException(const ErrnoException &e)
{
 setError(e.getErrorCode(), e.getErrorDesc());
}


//==============================================================================
// ErrnoException::operator=
//==============================================================================
ErrnoException &ErrnoException::operator=(const ErrnoException &e) 
{
 setError(e.getErrorCode(), e.getErrorDesc());
 return *this;
}


//==============================================================================
// ErrnoException::setError
//==============================================================================
void ErrnoException::setError(int error, const char *desc)
{
 d_errno = error;
 if(desc)
  strncpy(d_desc, desc, 39);
 d_desc[39] = '\0';
}


//==============================================================================
// ErrnoException::getErrorCode
//==============================================================================
int ErrnoException::getErrorCode() const
{
 return d_errno;
}


//==============================================================================
// ErrnoException::getErrorDesc
//==============================================================================
const char *ErrnoException::getErrorDesc() const
{
 return d_desc;
}

#endif //_ERRNOEXCEPTION_HPP_INCLUDED
