#ifndef LOCK_H_
#define LOCK_H_


// Macros for locking sections
//
//

#define sysLock() asm{ pushf; cli; }
#define sysUnlock() asm popf;

extern int syncPrintf(const char *format, ...);

class Preemptive{
public:
	static int lockNested;
	static int lockDelayed;
	static void lock();
	static void unlock();
};




#endif /* LOCK_H_ */
