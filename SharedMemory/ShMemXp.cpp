//==============================================================================
// ShMemXp.hpp - XENOMAI-POSIX shared memory wrapper class.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 27.10.2015   1.0            Said Nuri UYANIK     Initial creation
//==============================================================================
#include "ShMemXp.hpp"

ShMemXp::ShMemXp(const char* name, int size){
	_shmFd = -1;
	_shmSize = size;
	_isOwner = false;
	_errno = 0;

	 _shmMem = this->create(name, size);
}

ShMemXp::~ShMemXp(){
	if(_isOwner)
		unlink();

	if(_shmName)
		delete [] _shmName;
}

void * ShMemXp::create(const char *name, int size){
	int nlen;

	/* Copy the name */
	nlen = strlen(name);
	_shmName = new char [nlen + 1];
	strncpy(_shmName, name, nlen);
	_shmName[nlen] = '\0';

	// Create shared memory
	_shmFd = shm_open(_shmName,
					 CREATE_AND_OPEN_FLAG,
					 PERMISSION_GROUP_MODE );

	if(_shmFd == -1){
		// If shared memory already exist, and this object
		// has ownership of it, unlink and create new one
		if(errno == EEXIST && _isOwner){
			if(shm_unlink(_shmName) == -1){
				throw ZnmException("Unlink existing shared memory failed ", "create()", _errno);
			}

			_shmFd = shm_open(_shmName,
					 		CREATE_AND_OPEN_FLAG,
					 		PERMISSION_GROUP_MODE );

			if(_shmFd == -1){
				_errno = errno;
				throw ZnmException("Re-creating shared memory failed", "create()", _errno);
			}
		}
		// If shared memory already exist, and this object
		// has NOT ownership of it, try to open
		else if(errno == EEXIST && !_isOwner){
			_shmMem = open(_shmName, _shmSize);
			_errno = 0;
			_isOwner = false;
			return _shmMem;
		}else{
			_errno = errno;
			throw ZnmException("Opening shared memory failed", "open()", _errno);
		}
	}

	//if success

	// Set size of memory map
	if( ftruncate(_shmFd, _shmSize) == -1 ){

		_errno = errno;
		close();
		throw ZnmException("Setting size of memory map failed", "ftruncate()", _errno);
	}

	// Allow shared memory regions to be accessed by the caller 
	_shmMem = mmap(NULL,
					_shmSize,
					PROTECTION,
					MAP_SHARED,
					_shmFd,
					0);

	// handle errors
	if(_shmMem == MAP_FAILED){
		_errno = errno;
		close(); //close desrictors if exist
		throw ZnmException("Mapping failed", "mmap()", _errno);
	}

	// Not need fd anymore
	::close(_shmFd);

	_isOwner = true;
	_errno = 0;

	//return memory region
	return _shmMem;
}

// open + mmap, if success. 
// close + exception , if fails
void *ShMemXp::open(const char *name, int size){

	_shmFd = shm_open(_shmName, 
					OPEN_FLAG, 
					PERMISSION_GROUP_MODE);

	if (_shmFd == -1){
		_errno = errno;
		throw ZnmException("Opening failed", "open()", _errno);
	}

	_shmSize = size;

	// Allow shared memory regions to be accessed by the caller 
	_shmMem = mmap(NULL, 
					_shmSize, 
					PROTECTION, 
					MAP_SHARED,
					_shmFd,
					0);

	// handle errors
	if(_shmMem == MAP_FAILED){
		_errno = errno;
		close(); //close desrictors if exist
		throw ZnmException("Mapping failed", "open()", _errno);
	}

	// if openning is successful
	_errno = 0;
	_isOwner = false;

	return(_shmMem);
}

int ShMemXp::unlink(){
	if(!_isOwner){
		_errno = EACCES;
		throw ZnmException("Unlink failed: No permission to unlink", "unlink()", _errno);
	}

	close();

	if( shm_unlink(_shmName) == -1){
		_errno = errno;
		throw ZnmException("Unlink failed", "unlink", _errno);
	}

	_isOwner = false;
	_errno = 0;

	return 0;
}

int ShMemXp::close(){

	// if already closed, return success
	if(_shmFd == -1){
		return 0;
	}
	/*
	if( ::close(_shmFd) == -1){
		_errno = errno;
		throw ZnmException("Close failed", "close()", _errno);
	}
	*/
	// Unmap
	if(_shmMem != NULL || _shmMem != MAP_FAILED){
		if( munmap(_shmMem, _shmSize) == -1 ){
			_errno = errno;
			throw ZnmException("Unmap failed after close", "close", _errno);
		}
	}
	
	_shmMem = NULL;
	_shmFd = -1;
	_errno = 0;

	return 0;
}

void* ShMemXp::getShmAddr(){
	return _shmMem;
}

int ShMemXp::getShmSize(){
	return _shmSize;
}