#ifndef IDLE_H_
#define IDLE_H_
#include "Thread.h"


class Idle:public Thread {

public:
	Idle();
	~Idle();
	void start();
	void run();
};




#endif /* IDLE_H_ */
