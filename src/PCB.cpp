#include <dos.h>
#include "System.h"
#include "PCB.h"
#include "Types.h"
#include "SCHEDULE.h"
#include "Idle.h"
#include "lock.h"
#include "stdio.h"
#include "HandList.h"
#include "KSemap.h"
#include "KernelEv.h"
#include "PCBList.h"

#define SIG_SIZE 16
int PCB::sID = 0;
const unsigned setPSW = 0x200;
const StackSize maxStackSize = 0x1000; // Hex number of 2^16 (65 536 B)
PCBList* PCB::allThreads = new PCBList();


PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice)
{
	initializePCB(thread, stackSize, timeSlice);
}


void PCB::initializePCB(Thread* thread, StackSize stackSize, Time timeSlice){

	myThread = thread;
	myTime = timeSlice;
	timeLeft = myTime;
	myStatus = New;
	myID = ++sID;
	myLockNested = 0;
	waitingThreads = new PCBList();



	//if(!myThread) {					// If it's main thread.
	if(myID == 1){
			stack = 0;
			stackSegment = stackPointer = basePointer = 0;
			mParent = 0;
	}

	else {
			mParent = (PCB*)System::runningPCB;
			if(myID == System::idleThread->getId()) mParent = 0;			// If it's idle thread.
			if(stackSize > maxStackSize) stackSize = maxStackSize;

			initializeStack(stackSize);
	}

	allThreads->push(this);

	//
	// Signals
	//
	initializeSignals();

}


PCB::~PCB(){

	//If it's main thread, it doesn't have threadWrapper.
	if(myID == 1) 	signals[2]->callHandlers();

	waitingThreads->empty();
	allThreads->removeNode(this);

	if(stack) delete[] stack;
	delete waitingThreads;

	waitingThreads = 0;
	mParent = 0;

	//Signals
	for(int i = 0; i < SIG_SIZE; i++){
			delete signals[i];
	}
}

void PCB::initializeStack(StackSize stackSize) {
		stackSize/=sizeof(unsigned);  					// Divide BYTES by 2. Get's the number of locations for an array. Takes 2 bytes to store unsigned (16bits)
		stack = new unsigned[stackSize];
		stack[stackSize - 1] = setPSW;
	#ifndef BCC_BLOCK_IGNORE
		stack[stackSize - 2] = FP_SEG(threadWrapper);
		stack[stackSize - 3] = FP_OFF(threadWrapper);

		stackSegment = FP_SEG(stack + stackSize - 12);
		stackPointer = FP_OFF(stack + stackSize - 12);
		basePointer = stackPointer;
	#endif
}

void PCB::threadWrapper() {

	System::runningPCB->myThread->run();

	if(System::runningPCB->mParent != 0) System::runningPCB->mParent->signals[1]->saveSignals();	//ne preko signal da bih oznacio da je kernel kod
	System::runningPCB->signals[2]->callHandlers();						// Call immediately signal 2 for this thread.

	//System::runningPCB->mParent->myThread->signal(1);
	//((PCB*)System::runningPCB)->myThread->signal(2);


#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif
	//syncPrintf("threadWrapper niti broj %d\n", System::runningPCB->getId());
	System::runningPCB->myStatus = PCB::Finished;
	System::runningPCB->waitingThreads->empty();

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
	dispatch();
}


void PCB::start() {
	if(!threadState(PCB::New)) return;
		setMyStatus(PCB::Ready);
		Scheduler::put(this);
}

void PCB::waitToComplete() {

		if(myStatus != Finished && System::runningPCB->myID != myID
												&& this != System::mainThread->myPCB
												&& this != System::idleThread->myPCB) {  		// Can't call for main and idle thread, and itself.
			System::runningPCB->setMyStatus(PCB::Blocked);
				waitingThreads->push((PCB*)System::runningPCB);
					dispatch();
		}
}


void PCB::setMyStatus(threadStatus state) volatile{
	myStatus = state;
}

Thread* PCB::getMyThread() volatile{
	return myThread;
}

bool PCB::threadState(threadStatus state) volatile{

	switch(state) {
	case New:
			return (myStatus == New ? true : false);

	case Ready:
			return (myStatus == Ready ? true : false);

	case Blocked:
			return (myStatus == Blocked ? true : false);

	case Finished:
			return (myStatus == Finished ? true : false);

	}
	return false;
}

Thread* PCB::getThreadById(int id){
	return allThreads->getThreadById(id);
}


ID PCB::getId() volatile{
	return myID;
}


//	-------------------  ---------------------------- -------------------  ---------------------------- -------------------  ----------------------------
//	-------------------  ---------------------------- -------------------  ---------------------------- -------------------  ----------------------------
//	-------------------  ----------------------------                Signals implementation 			-------------------  ----------------------------
//	-------------------  ---------------------------- -------------------  ---------------------------- -------------------  ----------------------------
//	-------------------  ---------------------------- -------------------  ---------------------------- -------------------  ----------------------------


// metodu pozivam u pcb konstruktoru
void PCB::initializeSignals(){
	for(int i = 0 ; i < SIG_SIZE; i++){
		signals[i] = new HandlerList(i);
	}

	if(mParent != 0){
		for(int i=1; i < SIG_SIZE; i++){
			mParent->signals[i]->getParentHandlers(this);
		}
	}
}

void PCB::signalZeroHandler(){

	//printf("usao u zeroHendler niti %d\n", getId());
	myStatus = Finished;


	if(mParent != 0) mParent->signals[1]->saveSignals();	//ne preko signal da bih oznacio da je kernel kod
	this->signals[2]->callHandlers();						//pozvace ga odmah pre unistenja

	waitingThreads->empty();
	allThreads->removeNode(this);
	System::semq->removeFromSemaphor(this);
	System::eventq->removeFromEvent(this);

	dispatch();

}

void PCB::signal(SignalId signal) {
	if(System::runningPCB == this) signals[signal]->callHandlers();		// Obradi odmah ako sam u kontekstu date niti
	else signals[signal]->saveSignals();								// ako nisam samo sacuvaj signal
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	signals[signal]->add(handler);
}

void PCB::unregisterAllHandlers(SignalId id) {
	signals[id]->empty();
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	signals[id]->swap(hand1, hand2);
}

void PCB::blockSignal(SignalId signal) {
	signals[signal]->blockSignal();
}

void PCB::unblockSignal(SignalId signal) {
	signals[signal]->unblockSignal();
}

void PCB::blockSignalGlobally(SignalId signal) {
	System::globalSignalArray[signal] = 1;
	allThreads->blockAll(signal);
}

void PCB::unblockSignalGlobally(SignalId signal) {
	System::globalSignalArray[signal] = 0;
	allThreads->unblockAll(signal);
}

void PCB::callDelayedSignals(){
	bool zeroFlag = false;
	for(int i = 0 ; i < SIG_SIZE; i++){
		if(i==2) continue;
		if(signals[i]->isBlocked() || System::globalSignalArray[i] == 1) continue; // moram da pitam da li je blokiran

		if(signals[i]->isTimedOut()){	// i da li je timedOut > 0 odnosno da li je pozvan izvan njegovog konteksta signal
			if(i==0) {
				zeroFlag = true;
				continue;
			}

			signals[i]->callTimedOutSignals();		//pozovi 3-15 signale, 2 nece biti delayed jer se poziva samo kad se nit unistava(ili delet-uje)
		}
	}
	if(zeroFlag) signalZeroHandler();	// ako 0 nije blokiran tek kad odradim sve ostale zakasnele signale unistim nit.
}


