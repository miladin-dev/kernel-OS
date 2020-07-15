#ifndef PCB_H_
#define PCB_H_
#include "HandList.h"	//
#include "PCBList.h"	// Lists
#include "Thread.h"
#include "Types.h"

class PCB {
public:
	friend class System;
	friend class PCBList;

	PCB(Thread*, StackSize, Time);
	~PCB();

	// PCB Initialization
	//
	//

	enum threadStatus { New=0, Ready, Blocked, Running, Finished};


	// Thread functions
	//
	//
	ID getId() volatile;   // ID - typedef int
	Thread* getMyThread() volatile;
	void setMyStatus(threadStatus) volatile;
	bool threadState(threadStatus) volatile;
	void waitToComplete();
	void start();
	static Thread* getThreadById(int);



	// List of threads
	//
	//
	static PCBList* allThreads;
	PCBList* waitingThreads;
	volatile threadStatus myStatus;
	int myLockNested;
	int myLockDelayed;
	ID myID;


	//
	//	Signals
	//
	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

	void blockSignal(SignalId signal);
	void unblockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);

	void initializeSignals();
	void callDelayedSignals();
	void signalZeroHandler();

	HandlerList* signals[16];
	PCB* mParent;
	Thread* myThread;

private:

	// PCB Initialization
	//
	//
	unsigned* stack;
	unsigned stackSegment;
	unsigned stackPointer;
	unsigned basePointer;
	void initializePCB(Thread*, StackSize, Time);
	void initializeStack(StackSize);
	static void threadWrapper();


	// Thread parts for initialization
	//
	//

	static int sID;
	Time myTime;

	Time timeLeft;
};



#endif /* PCB_H_ */
