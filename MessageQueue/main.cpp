#include "MessageQueueXp.hpp"
#include "MqThread.hpp"
#include <stdlib.h>

void create_unlink_test();
void communication_test();

int main(int argc, char const *argv[])
{
	
	//create_unlink_test();

	communication_test();

	//sleep(5);
	return 0;
}

void create_unlink_test(){

	MessageQueueXp mq1("mq2");
	MessageQueueXp mq2("mq2");
}

void communication_test(){

	char rbuf[1024];
	char sbuf = '1';

	MessageQueueXp mq1("mq3");
	mq1.send(&sbuf, 1);

	MessageQueueXp mq2("mq3");
	mq2.receive(rbuf, 1024);

	cout << "received: " << rbuf << endl;

}