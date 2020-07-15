#include "System.h" 
#include "stdio.h"
#include "KernelEv.h"
#include "SCHEDULE.h"
#include "SemQ.h"

SemQueue::SemQueue(){
	front = back = 0;
	elemNum = 0;
}

SemQueue::~SemQueue(){
	if(front == 0) return;
	semElem* temp = front;
	semElem* old = 0;

		while(temp!=0){
			old = temp;
			temp = temp->next;
			old->next = 0;
			delete old;
			--elemNum;
		}

		front = back = 0;
}

void SemQueue::pushB(KernelSem* ksem){
	back = (front ? back->next : front) = new semElem(ksem);
	++elemNum;
}


void SemQueue::timerUpdate(){
	if(front == 0) return;	

	semElem* tmp = front;

	for(int i=0; i < elemNum; i++){
		int val = tmp->ksem->sleepq->timerSleep();
		tmp->ksem->updateValue(val);

		tmp = tmp->next;
	}

	tmp=0;
}




void SemQueue::deleteNode(KernelSem* ksem){
	if(front == 0) return;
	semElem* tmp = front;
	semElem* prev = 0;
	
	if(tmp == 0) return;

	while(tmp != 0 && tmp->ksem != ksem){
		prev = tmp;
		tmp = tmp->next;
	}

	if(prev == 0) {
		front = front->next;
	}
	else{
		if(tmp->next == 0) back = prev;
		prev->next = tmp->next;
	}

	tmp->next = 0;
	--elemNum;
	delete tmp;

}

void SemQueue::removeFromSemaphor(PCB* mPCB){
	if(front == 0) return;			

		semElem* tmp = front;

		for(int i=0; i < elemNum; i++){
			int val = tmp->ksem->sleepq->removePCB(mPCB);
			tmp->ksem->updateValue(val);	
			tmp = tmp->next;
		}

		tmp=0;
}

//
//	USED FOR ALL KERNEL EVENTS LIST (to delete it with signal(0))
//

void SemQueue::addKEvent(KernelEv* kev){
	back = (front ? back->next : front) = new semElem(kev);
	++elemNum;
}

void SemQueue::removeFromEvent(PCB* mPCB){
	if(front == 0) return;

	semElem* tmp = front;
	while(tmp != 0){
		if(tmp->kEv->mOwner == mPCB){			
			tmp->kEv->value = 1;	
			return;
		}
		tmp = tmp->next;
	}

	tmp = 0;
}


