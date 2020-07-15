#include "KernelEv.h"
#include "System.h"
#include "IVTEntry.h"
#include "PCB.h"



KernelEv::KernelEv(IVTNo ivtn) {
	mOwner = (PCB*)System::runningPCB;
	value = 0;
	IVTEntry::ivtTable[ivtn]->initializeKernelEv(this);
	blocked = 0;
	signaled = 0;
	System::eventq->addKEvent(this);
}

void KernelEv::wait(){
	if(mOwner != System::runningPCB) return;

	if(signaled) {
		signaled = 0;
		return;
	}
	mOwner->myStatus = PCB::Blocked;
	blocked = 1;
	dispatch();
}

void KernelEv::signal(){
	if(value) return; //for all events list (for signal(0)) - only once called since if destroy our thread, its owner, can't be called signal once again

	if(blocked){
		mOwner->myStatus = PCB::Ready;
		blocked = 0;
		System::putPCB(mOwner);
		dispatch();
	}
	else {
		signaled = 1;
	}
}

KernelEv::~KernelEv(){
	mOwner = 0;
}




