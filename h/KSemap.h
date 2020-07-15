#ifndef KSEMAP_H_
#define KSEMAP_H_
#include "PCB.h"	// PCB is pushB parameter.
#include "queues.h"
#include "Semaphor.h"

class SemQueue;
class SleepQueue;

class KernelSem{
public:
	KernelSem(int init);
	~KernelSem();

	int wait(Time maxTimeToWait);
	int signal(int n = 0);

	int val() const;

	void updateValue(int);

	volatile int waitRes;
	SleepQueue* sleepq;


	void ispis();

	friend class SemQueue;
	friend class SleepQueue;
private:
	int value;
};


#endif /* KSEMAP_H_ */
