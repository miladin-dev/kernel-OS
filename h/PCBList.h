#ifndef THRLIST_H_
#define THRLIST_H_
#include "Thread.h"
#include "Types.h"

class PCBList {
private:

	friend class PCB;

	struct lstElem {
		lstElem(PCB* t) {
			nodePCB = t;
			next = nullptr;
		}
		PCB* nodePCB;
		lstElem* next;
	};

	lstElem* first;
	lstElem* last;


public:

	PCBList() : first(nullptr), last(nullptr), lstCounter(0) {}
	~PCBList();
	// List functions
	//
	//
	void push(PCB* t);
	Thread* getThreadById(ID ind);
	void empty();
	void removeNode(PCB*);

	volatile int lstCounter;

	//	Signals
	//
	void blockAll(SignalId);
	void unblockAll(SignalId);
};




#endif /* THRLIST_H_ */
