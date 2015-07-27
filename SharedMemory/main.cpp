//==============================================================================
// ShMemXp.hpp - XENOMAI-POSIX shared memory main file.
// Xenomai-version : 2.6.4
// Compatibility   : XENOMAI, g++
//
// Modification History:
// Date         Version        Modified By			Description
// 27.10.2015   1.0            Said Nuri UYANIK     Initial creation
//==============================================================================
#include "ShMemXp.hpp"

int main(int argc, char const *argv[])
{
	ShMemXp shm1("/shm0", sizeof(double));

	sleep(5);

	return 0;
}
