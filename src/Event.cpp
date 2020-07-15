#include "Event.h"
#include "KernelEv.h"
#include "lock.h"

Event::Event(IVTNo ivtNo) {
	Preemptive::lock();
	myImpl = new KernelEv(ivtNo);
	Preemptive::unlock();
}

Event::~Event(){
	Preemptive::lock();
	delete myImpl;
	Preemptive::unlock();
}

void Event::wait() {
	Preemptive::lock();
	myImpl->wait();
	Preemptive::unlock();
}

void Event::signal() {
	Preemptive::lock();
	myImpl->signal();
	Preemptive::unlock();
}
