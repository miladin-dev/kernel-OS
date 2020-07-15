#include "KSemap.h"
#include "System.h"
#include "queues.h"
#include "stdio.h"
#include "SemQ.h"

//SemQueue* KernelSem::semq = new SemQueue();


KernelSem::KernelSem(int init){
	value = init;
	sleepq = new SleepQueue();		// Every KernelSem has it's own sleepq
	sleepq->setKernelSem(this);
	System::semq->pushB(this);				// Add to all semaphore list.
	waitRes = 1;
}

KernelSem::~KernelSem(){
	System::semq->deleteNode(this);
	delete sleepq;
}

int KernelSem::wait(Time maxTimeToWait){

	int result;

	if(maxTimeToWait == 0) {
		--value;
		if(value < 0){
			sleepq->pushB((PCB*)System::runningPCB, -1);		// -1 means that it's unlimitedSleeping
			System::runningPCB->myStatus = PCB::Blocked;
			dispatch();
		}
	}
	else
	if(maxTimeToWait > 0) {
		--value;
		if(value < 0){
			sleepq->pushB((PCB*)System::runningPCB, maxTimeToWait);
			System::runningPCB->myStatus = PCB::Blocked;
			dispatch();
		}
	}

	//syncPrintf("waitRes: %d", waitRes);

	return waitRes;
}

int KernelSem::signal(int numToUnblock){
	//ako je n = 0 ponasa se normalno znaci uvecacava value i vraca return 0;
	// ako je vece od 0 numToUnblock povecavam value za tu vrednost. Povratna vrednost oznacava broj odblokiranih niti


	//syncPrintf("signal pozvan\n");

	if(numToUnblock < 0) return numToUnblock;

	if(numToUnblock > 0){
		waitRes = 1;
		value += numToUnblock;
		int res = sleepq->reschedule(numToUnblock);
		return res;										 // Number of unblocked threads
	}

	if(numToUnblock == 0){
		//syncPrintf("s.signal(0)\n");
		++value;
		if(value <= 0){
			waitRes = 1;
			sleepq->reschedule(1);
		}
		return 0;
	}

	return 123; // nece stici dovde
}


int KernelSem::val() const{
	return value;
}

void KernelSem::updateValue(int val){
	value += val;
}


void KernelSem::ispis(){
	sleepq->ispis();
}




