#ifndef QUEUE_H_
#define QUEUE_H_
#include "KSemap.h"
#include "SemQ.h"
//#include "KernelEv.h"

class SleepQueue {
public:
	friend class KernelSem;

	SleepQueue();
	~SleepQueue();

	struct qElem{
		qElem(PCB* p, Time time) {
			pcb = p;
			next = 0;
			timeToSleep = time;
		}
		PCB* pcb;
		qElem* next;
		volatile Time timeToSleep;
		volatile int waitResult;
	};

	void popF();
	void pushB(PCB* mPCB, Time);


	int reschedule(int);

	int timerSleep();

	int isEmpty();

	void setKernelSem(KernelSem*);
	int removePCB(PCB* );

	void ispis();
private:
	KernelSem* myKernelSem;
	qElem* front;
	qElem* back;
	volatile int elemNum;
};



#endif /* QUEUE_H_ */
