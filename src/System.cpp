#include <dos.h>
#include "System.h"
#include "Types.h"		//boolean
#include "SCHEDULE.h"
#include "lock.h"
#include "stdio.h"
#include "KSemap.h"
#include "Semaphor.h"
#include "KernelEv.h"
#include "SemQ.h"
#include "IVTEntry.h"

extern void tick();

//
// Register store
//
static unsigned savedSP;
static unsigned savedSS;
static unsigned savedBP;

const unsigned timerEntry = 0x08;
const unsigned newEntry = 0x60;

InterruptRoutine System::oldTimerRoutine = 0;
volatile PCB* System::runningPCB = 0;
Idle* System::idleThread = 0;
volatile Thread* System::mainThread = 0;

SemQueue* System::semq = new SemQueue();
SemQueue* System::eventq = new SemQueue();
int* System::globalSignalArray = new int[16];

volatile bool dispatchCalled = false;
volatile bool dispatchDelayed = false;

void System::initializeKernel() {
#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif

	initializeThreads();
	initializeSignals();
	initializeInterrupts();

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
}

void System::initializeThreads() {

	//main
	mainThread = new Thread(0, defaultTimeSlice);
	mainThread->myPCB->myStatus = PCB::Running;
	runningPCB = mainThread->myPCB;

	//idle
	idleThread = new Idle();
}

void System::initializeSignals(){
	for(int i = 0; i < 16; i++){
		globalSignalArray[i] = 0;
	}
}

void System::putPCB(PCB* mPCB){
								// If thread status isn't
		if(mPCB->threadState(PCB::Blocked)) return;	// New, Ready or
		if(mPCB->threadState(PCB::Finished)) return;	// Running
								// don't put it in SCHEDULER.

		if(mPCB->getId() == idleThread->getId()) return;

		Scheduler::put(mPCB);
}

void System::getRunningPCB(){

		System::runningPCB = Scheduler::get();

		if(runningPCB->myStatus == PCB::Finished)		// If thread is already in scheduler and is called signal(0);
			System::runningPCB = Scheduler::get();

		if(System::runningPCB == 0) {
				System::runningPCB = System::idleThread->myPCB;
		}

		System::runningPCB->setMyStatus(PCB::Running);
}


void System::initializeInterrupts() {
#ifndef BCC_BLOCK_IGNORE
	oldTimerRoutine = getvect(timerEntry);
	setvect(timerEntry, timer);
	setvect(newEntry, oldTimerRoutine);
#endif
}

int saveLock = 0;
void System::dispatch() {

	if(Preemptive::lockNested > 0){
		saveLock = Preemptive::lockNested;
		--Preemptive::lockNested;
	}
	dispatchCalled = 1;
	timer();
}

int disp = 0;
void interrupt System::timer(...) {

		if(!dispatchCalled){
			asm int 60h;
					if(runningPCB->timeLeft > 0)
					--runningPCB->timeLeft;
					System::semq->timerUpdate();
					tick();
		}

		if((runningPCB->timeLeft == 0) || dispatchCalled == 1) {
			if(Preemptive::lockNested == 0){
					dispatchCalled = 0;
					
				#ifndef BCC_BLOCK_IGNORE
					asm mov savedSP, sp
					asm	mov savedSS, ss
					asm	mov savedBP, bp
				#endif



					runningPCB->stackSegment = savedSS;
					runningPCB->stackPointer = savedSP;
					runningPCB->basePointer = savedBP;

					if(saveLock == 1)
					runningPCB->myLockNested = saveLock;
					else runningPCB->myLockNested = 0;

					saveLock = 0;

					System::putPCB((PCB*)runningPCB);



					System::getRunningPCB();
				
					savedSS = runningPCB->stackSegment;
					savedSP = runningPCB->stackPointer;
					savedBP = runningPCB->basePointer;
					Preemptive::lockNested = runningPCB->myLockNested;
					runningPCB->timeLeft = runningPCB->myTime;

					#ifndef BCC_BLOCK_IGNORE
							asm mov ss, savedSS
							asm mov sp, savedSP
							asm mov bp, savedBP
					#endif


					((PCB*)runningPCB)->callDelayedSignals();

			} else dispatchCalled = 1;
		}

		if(!dispatchCalled){

		}

}



void System::restoreInterrupts(){
#ifndef BCC_BLOCK_IGNORE
	oldTimerRoutine = getvect(newEntry);
	setvect(timerEntry, oldTimerRoutine);
#endif
}

void System::restoreKernel() {
#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif

	restoreInterrupts();

	delete System::semq;
	delete System::eventq;
	delete[] globalSignalArray;
	delete idleThread;
	delete mainThread;
	delete PCB::allThreads;


#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
}





