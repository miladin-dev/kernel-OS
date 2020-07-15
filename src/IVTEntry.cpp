#include <dos.h>
#include "IVTEntry.h"
#include "KernelEv.h"
#include "stdio.h"

IVTEntry* IVTEntry::ivtTable[256] = {0};

IVTEntry::IVTEntry(IVTNo ivtn, InterruptRoutine newRoutine){

#ifndef BCC_BLOCK_IGNORE
	sysLock();
	oldRoutine = getvect(ivtn);
	setvect(ivtn, newRoutine);
#endif
	ivtNum = ivtn;
	myKernelEv = 0;
	ivtTable[ivtNum] = this;
#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
}


IVTEntry::~IVTEntry(){
	(*oldRoutine)();
#ifndef BCC_BLOCK_IGNORE
	setvect(ivtNum, oldRoutine);
#endif
	myKernelEv = 0;
	ivtTable[ivtNum] = 0;
}

void IVTEntry::signal(){
	//Preemptive::lock();
	myKernelEv->signal();
	//Preemptive::unlock();
}

void IVTEntry::callOldRoutine(){
	(*oldRoutine)();
}

void IVTEntry::initializeKernelEv(KernelEv* kEv) {
	myKernelEv = kEv;
}
