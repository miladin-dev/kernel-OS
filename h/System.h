#ifndef SYSTEM_H_
#define SYSTEM_H_
#include "Idle.h"
#include "PCB.h"
#include "lock.h"
#include "queues.h"



typedef void interrupt (*InterruptRoutine) (...);


class System {
private:

public:
	//
	// Interrupts
	//
	static void interrupt timer(...);
	static void initializeInterrupts();
	static void restoreInterrupts();
	static InterruptRoutine oldTimerRoutine;


	//
	// Kernel implementation
	//
	static void initializeKernel();
	static void restoreKernel();


	//
	// Thread based implementation
	//
	static void	initializeThreads();
	static void dispatch();
	static void getRunningPCB();
	static void putPCB(PCB*);

	static volatile PCB* runningPCB;
	static volatile PCB* mainPCB;
	static Idle* idleThread;
	static volatile Thread* mainThread;

	static SemQueue* semq;
	static SemQueue* eventq;

	//
	// Signals
	static int* globalSignalArray;
	static void initializeSignals();




};

#endif /* SYSTEM_H_ */
