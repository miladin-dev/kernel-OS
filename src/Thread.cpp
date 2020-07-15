#include "Thread.h"
#include "SCHEDULE.H"
#include "System.h"
#include "PCB.h"
#include "lock.h"
#include "stdio.h"

Thread::Thread(StackSize stackSize,	Time timeSlice)
{
#ifndef BCC_BLOCK_IGNORE
	//Preemptive::lock();
	sysLock();
#endif

	myPCB = new PCB(this, stackSize, timeSlice);

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
	//Preemptive::unlock();
#endif
}

void Thread::start() {
#ifndef BCC_BLOCK_IGNORE
	//Preemptive::lock();
	sysLock();
#endif
	myPCB->start();
#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
	//Preemptive::unlock();
#endif
}

void Thread::waitToComplete(){
#ifndef BCC_BLOCK_IGNORE
	//Preemptive::lock();
	sysLock();
#endif

	myPCB->waitToComplete();

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
	//Preemptive::unlock();

#endif
}

ID Thread::getId() {
	return myPCB->getId();
}

Thread::~Thread(){
#ifndef BCC_BLOCK_IGNORE
	//Preemptive::lock();
	sysLock();
#endif
	//printf("Thread Destruktor niti: %d\n", getId());
	waitToComplete();
	delete myPCB;

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
	//Preemptive::unlock();
#endif
}

ID Thread::getRunningId() {
	return System::runningPCB->getId();
}

Thread* Thread::getThreadById(ID id) {
	return PCB::getThreadById(id);
}

void dispatch(){
#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif
	System::dispatch();
#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
}

// Signals

void Thread::signal(SignalId signal) {
	if(signal < 0 || signal > 15) return;
	if(signal == 2 || signal == 1) return;												//Ako je pozvan iz korisnickog koda

	Preemptive::lock();
	myPCB->signal(signal);
	Preemptive::unlock();
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	if(signal < 0 || signal > 15) return;
	if(signal == 0) return;

	Preemptive::lock();
	myPCB->registerHandler(signal, handler);
	Preemptive::unlock();
}

void Thread::unregisterAllHandlers(SignalId id) {
	if(id < 0 || id > 15) return;
	if(id == 0) return;

	Preemptive::lock();
	myPCB->unregisterAllHandlers(id);
	Preemptive::unlock();
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	if(id < 0 || id > 15) return;
	if(id == 0) return;

	Preemptive::lock();
	myPCB->swap(id, hand1, hand2);
	Preemptive::unlock();
}

void Thread::blockSignal(SignalId signal) {
	if(signal < 0 || signal > 15) return;

	Preemptive::lock();
	myPCB->blockSignal(signal);
	Preemptive::unlock();
}

void Thread::unblockSignal(SignalId signal) {
	if(signal < 0 || signal > 15) return;

	Preemptive::lock();
	myPCB->unblockSignal(signal);
	Preemptive::unlock();
}

void Thread::blockSignalGlobally(SignalId signal) {
	if(signal < 0 || signal > 15) return;

	Preemptive::lock();
	PCB::blockSignalGlobally(signal);
	Preemptive::unlock();
}

void Thread::unblockSignalGlobally(SignalId signal) {
	if(signal < 0 || signal > 15) return;

	Preemptive::lock();
	PCB::unblockSignalGlobally(signal);
	Preemptive::unlock();
}
