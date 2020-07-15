#include "lock.h"
#include "KSemap.h"
#include "Semaphor.h"

Semaphore::Semaphore(int init) {
#ifndef BCC_BLOCK_IGNORE		// only if sysLock();
	sysLock();
#endif

	myImpl = new KernelSem(init);

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
}

Semaphore::~Semaphore(){
#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif

	delete myImpl;

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
}

int Semaphore::wait(Time maxTimeToWait){
#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif
	int ret = myImpl->wait(maxTimeToWait);
#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif
	return ret;
}

int Semaphore::signal(int n){
#ifndef BCC_BLOCK_IGNORE
	sysLock();
#endif

	int ret = myImpl->signal(n);

#ifndef BCC_BLOCK_IGNORE
	sysUnlock();
#endif

	return ret;
}

int Semaphore::val() const {
	return myImpl->val();
}

void Semaphore::ispis(){
	myImpl->ispis();
}

