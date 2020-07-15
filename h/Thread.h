/*
 * Thread.h
 *
 *  Created on: March 22, 2020
 *      Author: OS1
 */


#ifndef THREAD_H_
#define THREAD_H_

// Signal typedefs
typedef void (*SignalHandler)();
typedef unsigned SignalId;

typedef unsigned long StackSize;
typedef unsigned int Time; // time, x 55ms
typedef int ID;

const StackSize defaultStackSize = 900; // hoce sa 600 - 200 200 4
const Time defaultTimeSlice = 2; // default = 2*55m s


class PCB; // Kernel's implementation of a user's thread

class Thread {
public:

	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);


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

protected:

	friend class PCB;
	friend class Idle;
	friend class System;
	friend class ThreadList;

	Thread(StackSize stackSize = defaultStackSize,
						Time timeSlice = defaultTimeSlice);

	virtual void run() {}

private:
	PCB* myPCB;
};

void dispatch();

#endif /* THREAD_H_ */

