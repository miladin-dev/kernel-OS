#ifndef IVTENTRY_H_
#define IVTENTRY_H_
//#include "Event.h"
#include "System.h"		//typedef InterruptRoutine


#define PREPAREENTRY(entryNum, callOld)\
	void interrupt intr##entryNum(...);\
	IVTEntry ivte##entryNum(entryNum, intr##entryNum);\
	void interrupt intr##entryNum(...){\
		ivte##entryNum.signal();\
		if(callOld == 1)\
			ivte##entryNum.callOldRoutine();\
	}\

typedef unsigned char IVTNo;

class KernelEv;

class IVTEntry{
public:

	//friend class KernelEv;

	IVTEntry(IVTNo, InterruptRoutine);
	~IVTEntry();

	void signal();
	void callOldRoutine();
	void initializeKernelEv(KernelEv*);
	static IVTEntry* ivtTable[256];
private:
	IVTNo ivtNum;
	InterruptRoutine oldRoutine;
	KernelEv* myKernelEv;

};



#endif /* IVTENTRY_H_ */
