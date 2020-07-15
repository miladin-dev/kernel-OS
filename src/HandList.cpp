#include "System.h"
#include "stdio.h"
#include "HandList.h"

HandlerList::HandlerList(SignalId id){
	first = last = 0;
	handlerNum = 0;
	signalTimedOut = 0;
	mySignalId = id;
	signalBlocked = false;
	savedSignals = 0;
}

HandlerList::~HandlerList(){
	sigElem* temp = first;
	sigElem* old = 0;

	while(temp!=0){
		old = temp;
		temp=temp->next;
		old->next=0;
		delete old;
	}

	first = last = 0;
}

void HandlerList::add(SignalHandler sig){
	last = (first ? last->next : first) = new sigElem(sig);
	++handlerNum;
	//printf("broj elemenata u signalu: %d", handlerNum);
}

void HandlerList::empty(){
	if(first == 0) return;
	sigElem* tmp = first;

	while(first){
		tmp = first;
		first = first->next;
		--handlerNum;
		tmp->next = 0;
		delete tmp;
	}
}

void HandlerList::swap(SignalHandler hand1, SignalHandler hand2){
	if(first == 0) return;
	sigElem* tmp = first;
	sigElem* old = 0;
	sigElem* helpNext = 0;
	SignalHandler helpHandler = 0;

	bool found = false;

	while(tmp != 0){
		if(tmp->mSignalHand == hand1 || tmp->mSignalHand == hand2){
			if(!found){
				old = tmp;
				found = true;
			} else {
				helpHandler = old->mSignalHand;
				//helpNext = old->next;

				old->mSignalHand = tmp->mSignalHand;
				//old->next = tmp->next;

				tmp->mSignalHand = helpHandler;
				//tmp->next = helpNext;
			}
		}
		tmp = tmp->next;
	}

	tmp = 0;
}

void HandlerList::saveSignals(){
	++signalTimedOut;
}

void HandlerList::callHandlers(){
	if(first == 0) return;
	if(isBlocked() || System::globalSignalArray[mySignalId] == 1) {	//Proveravam samo zbog signala(2) jer iz njega direktno pozivam ovaj hendler
			return;
		}
	sigElem* tmp = first;
	while(tmp != 0){
		tmp->mSignalHand();
		tmp = tmp->next;
	}
	tmp = 0;
}

// ovu metodu pozivam iz timera, tj tajmer poziva za novu runningPCB ovu metodu koja za svoj niz signala poziva ovu metodu)
void HandlerList::callTimedOutSignals(){
	while(signalTimedOut){
			--signalTimedOut;
			callHandlers();
		}
}

void HandlerList::getParentHandlers(PCB* child){
	if(first == 0) return;
	sigElem* tmp = first;

	while(tmp != 0){	//while there are handlers for this signal id
		child->registerHandler(mySignalId, tmp->mSignalHand);
		tmp = tmp->next;
	}
	tmp = 0;
}


void HandlerList::blockSignal(){
	//printf("blocked signal pozvan\n");
	signalBlocked = true;
}

void HandlerList::unblockSignal(){
	//printf("unblocked signal pozvan\n");
	signalBlocked = false;
}

bool HandlerList::isBlocked(){
	if(signalBlocked || System::globalSignalArray[mySignalId] == 1) return true;
	else return false;
}

bool HandlerList::isTimedOut(){
	if(signalTimedOut) return true;
	else return false;
}

