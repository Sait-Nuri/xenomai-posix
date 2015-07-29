//==============================================================================
// ShMemXp.hpp - XENOMAI-POSIX shared memory wrapper class.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 27.10.2015   1.0            Said Nuri UYANIK     Initial creation

#ifndef _SHMEM_HPP_INCLUDED
#define _SHMEM_HPP_INCLUDED

#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <fcntl.h>
#include <stdlib.h>
#include <inttypes.h>
#include "znmException.hpp"

#define PERMISSION_GROUP_MODE S_IRWXU | S_IRWXG | S_IRWXO //read/write/execute for enyone
#define CREATE_AND_OPEN_FLAG O_CREAT | O_EXCL | O_RDWR // Create
#define OPEN_FLAG O_RDWR
#define TRUNCATE_MEMORY O_TRUNC
#define DIRECT_MEMORY_ACCESS O_DIRECT
#define PROTECTION PROT_READ | PROT_WRITE

#include <iostream>

using namespace std;

class ShMemXp
{

	public:
		ShMemXp(const char* name, int size);

		~ShMemXp();

		void* getShmAddr();

		int getShmSize();

		int unlink();

		inline int getErrnoError() const;
	
	private:
		
		void *create(const char *name, int size); 

		void *open(const char *name, int size);

		int close();


		/** 
		 * File descriptor of open shared memory.
		 =================================================*/
		int _shmFd;

		char* _shmName;

		int _shmSize;

		void* _shmMem;

		bool _isOwner;

		int _errno;

};

int ShMemXp::getErrnoError() const
{
	return _errno;
}

#endif