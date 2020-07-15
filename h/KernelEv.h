#ifndef KERNELEV_H_
#define KERNELEV_H_
#include "Event.h"
#include "PCB.h"
#include "queues.h"

class KernelEv{
public:
	friend class SemQueue;

	KernelEv(IVTNo);
	~KernelEv(); 				// to be defined

	void wait();
	void signal();

	int value;
	PCB* mOwner;
private:
	int blocked;
	int signaled;

};



#endif /* KERNELEV_H_ */
