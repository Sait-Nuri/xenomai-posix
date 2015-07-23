/*
 * ZNMException.h
 *
 *  Created on: Dec 22, 2010
 *      Author: root
 */

#ifndef ZNMEXCEPTION_H_
#define ZNMEXCEPTION_H_

#include <exception>
#include <errno.h>
#include <string>
#include <iostream>

class ZnmException : public std::exception
{
public:
    ZnmException(std::string pName, std::string pFunction, int pErrorNo)
	{
		mError = pName + " " + pFunction + ": " + strerror(pErrorNo);
		mErrorNo = pErrorNo;
	}

    ZnmException(int pErrorNo)
	{
		mError = strerror(pErrorNo);
		mErrorNo = pErrorNo;
	}

    ZnmException(std::string pError)
	{
		mError = pError;
	}

    virtual ~ZnmException() throw()
	{

	}

	virtual const char* what() const throw()
	{
		return mError.c_str();
	}

	int errorNo() { return mErrorNo; }

private:
	std::string mError;
	int mErrorNo;
};


#endif /* ZEXCEPTION_H_ */
