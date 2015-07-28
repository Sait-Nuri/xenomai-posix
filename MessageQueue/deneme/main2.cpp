#include "../MessageQueueXp.hpp"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using namespace std;

void receive();


int main(int argc, char const *argv[])
{
	
	//create_unlink_test();

	receive();

	//sleep(5);
	return 0;
}

void receive(){

	char rbuf[1024];
	char sbuf = '1';

	//MessageQueueXp mq1("mq3");
	//mq1.send(&sbuf, 1);

	MessageQueueXp mq2("mq10");
	mq2.receive(rbuf, 1024);

	cout << "received: " << rbuf << endl;

}