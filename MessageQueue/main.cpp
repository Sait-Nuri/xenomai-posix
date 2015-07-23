#include "MessageQueueXp.hpp"

void create_unlink_test();
void communication_test();

int main(int argc, char const *argv[])
{
	
	create_unlink_test();

	communication_test();

	return 0;
}

void create_unlink_test(){

	MessageQueueXp mq1("mq1");
	MessageQueueXp mq2("mq1");
	mq1.unlink();
}

void communication_test(){

}