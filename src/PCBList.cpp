#include "Thread.h"
#include "PCB.h"
#include "System.h" //zbog syncprintf
#include "SCHEDULE.h"
#include "stdio.h"
#include "PCBList.h"


void PCBList::push(PCB* t) {
	last = (first ? last->next : first) = new lstElem(t);
	lstCounter++;
}

int iii = 0;
Thread* PCBList::getThreadById(ID ind) {
	if(first == 0)
		return 0;

	lstElem* temp = first;
	bool found = false;

	while(!found && temp != 0) {
		if(temp->nodePCB->getId() == ind)
			found = true;
		else temp=temp->next;
	}


	if(temp != 0 && found){
		return temp->nodePCB->myThread;
	}
	else return 0;

}

void PCBList::empty(){
	if (first == 0) return;
	lstElem* temp = first;
	lstElem* old = 0;

	while(temp != 0) {

		temp->nodePCB->myStatus = PCB::Ready;

		//printf("U scheduler ubacujem nit %d sa statusom %d\n", temp->nodePCB->getId(), temp->nodePCB->myStatus);

		System::putPCB(temp->nodePCB);

		old = temp;
		temp = temp->next;
		delete old;
		--lstCounter;
	}

	first = last = 0;

}

void PCBList::removeNode(PCB* mPCB){
	if(first == 0) return;
	lstElem* temp = first;
	lstElem* prev = 0;

		if(temp == 0) return;			// If list is empty!
		if(mPCB == 0) return;

		int deleteId = mPCB->myID;

		while(temp->nodePCB->myID != deleteId && temp != 0) {
			prev = temp;
			temp=temp->next;
		}

		if(temp == 0) return;

		if(prev == 0) {
			first = first->next;
		}
		else {
			if(temp->next == 0) last = prev;
			prev->next = temp->next;
		}

		if(temp->next != 0)
		temp->next = 0;

		--lstCounter;
		delete temp;
}


PCBList::~PCBList(){
	if(first == 0) return;
	lstElem* temp = first;
	lstElem* old = 0;

	while(temp != 0){
		old = temp;
		temp = temp->next;
		old->next = 0;
		delete old;
	}

	first = last = 0;
	//printf("Zavrsio threadlist destruktor");
}

/*
 *
 * 	Signals methods
 *
 */

void PCBList::blockAll(SignalId id){
	if(first == 0) return;
	lstElem* tmp = first;

	for(int i=0; i<lstCounter; i++){
		tmp->nodePCB->blockSignal(id);
		tmp = tmp->next;
	}

	tmp = 0;
}

void PCBList::unblockAll(SignalId id){
	if(first == 0) return;
	lstElem* tmp = first;

	for(int i=0; i<lstCounter; i++){
		tmp->nodePCB->unblockSignal(id);
		tmp = tmp->next;
	}

		tmp = 0;
}

