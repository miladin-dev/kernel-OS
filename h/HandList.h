#ifndef H_HANDLIST_H_
#define H_HANDLIST_H_
#include "Thread.h"		// zbog signalhandlera da ga prepozna
#include "Types.h"

class HandlerList{
public:
	HandlerList(SignalId);
	~HandlerList();

	struct sigElem{
		sigElem(SignalHandler sigh){
			mSignalHand = sigh;
			next = 0;
		}
		SignalHandler mSignalHand;
		sigElem* next;
	};

	bool signalBlocked;
	int signalTimedOut;
	int savedSignals;
	void add(SignalHandler);
	void empty();
	void swap(SignalHandler, SignalHandler);
	void callHandlers();
	void blockSignal();
	void unblockSignal();
	void callTimedOutSignals();
	void callZeroHandler(Thread*);
	void saveSignals();
	void getParentHandlers(PCB*);

	bool isBlocked();
	bool isTimedOut();
private:
	sigElem* first;
	sigElem* last;
	int handlerNum;
	SignalId mySignalId;
};



#endif /* H_HANDLIST_H_ */
