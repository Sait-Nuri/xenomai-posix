#include "MessageQueueXp.hpp"

int main(int argc, char const *argv[])
{
	MessageQueueXp mq1("mq1");
	mq1.unlink();

	return 0;
}