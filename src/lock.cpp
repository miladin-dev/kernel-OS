#include "lock.h"
#include "Thread.h"

extern int dispatchCalled;
int Preemptive::lockNested = 0;
int Preemptive::lockDelayed = 0;
//extern volatile int dispatchCalled;
//extern volatile int lockDelayed;

void Preemptive::lock() {
	lockNested=1;
}

void Preemptive::unlock(){
	lockNested = 0;
	if(dispatchCalled) dispatch();
}



