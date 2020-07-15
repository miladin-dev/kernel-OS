#include "KSemap.h"
//#include "KernelEv.h"


#ifndef SEMQ_H_
#define SEMQ_H_

class KernelEv;

class SemQueue{
public:
	friend class KernelSem;
	friend class KernelEv;

	SemQueue();
	~SemQueue();

	struct semElem{
		friend class KernelEv;

		semElem(KernelSem* k){
			ksem = k;
			next = 0;
			kEv = 0;
		}
		semElem(KernelEv* kev){
			kEv = kev;
			next = 0;
			ksem = 0;
		}
		KernelEv* kEv;
		KernelSem* ksem;
		semElem* next;
	};

	void popF();
	void pushB(KernelSem* );

	void timerUpdate();

	void deleteNode(KernelSem* );
	void removeFromSemaphor(PCB*);

	//
	//	Event
	//
	void addKEvent(KernelEv*);
	void removeFromEvent(PCB*);
private:
	semElem* front;
	semElem* back;
	volatile int elemNum;
};





#endif /* H_SEMQ_H_ */
