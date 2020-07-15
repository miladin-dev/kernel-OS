#include "queues.h"
#include "System.h" //Zbog System::putPCB;
#include "stdio.h"
#include "KernelEv.h"
#include "SCHEDULE.h"

//
//	Sleep queue implementation
//
SleepQueue::SleepQueue(){
	front = back = 0;
	elemNum = 0;
	myKernelSem = 0;
}

SleepQueue::~SleepQueue(){
	qElem* temp = front;
	qElem* old = 0;

			while(temp!=0){
				old = temp;
				temp = temp->next;
				old->next = 0;
				delete old;
				--elemNum;
			}

			front = back = 0;
}

void SleepQueue::setKernelSem(KernelSem* ksem){
	myKernelSem = ksem;
}

void SleepQueue::pushB(PCB* pcb, Time time){
	back = (front ? back->next : front) = new qElem(pcb, time);
	++elemNum;
	//syncPrintf("dodata u listu, backPCB: %d \n", back->pcb->myID);
}

int SleepQueue::isEmpty(){
	if(front == 0) return 1;
	else return 0;
}

int SleepQueue::reschedule(int numToUnblock){		// pozivam IZ KernelSem::SIGNALA SAMO

	int counter = numToUnblock;
	if(numToUnblock > elemNum)
		counter = elemNum;

	if(front == 0) return 0;
	qElem* tmp = front;

	for(int i=0; i<counter; i++){				// Za zadati broj odblokiraj niti na semaforu.s
		tmp->pcb->myStatus = PCB::Ready;
		System::putPCB(tmp->pcb);
		tmp = tmp->next;
		popF();
	}

	return counter;

}

void SleepQueue::popF(){
	if(front == 0) return;



	qElem* old;
	old = front;
	front = front->next;

	if(front == 0) back = 0;		// Ako je bio samo jedan el u listi

	old->next = 0;
	delete old;

	--elemNum;
}


int SleepQueue::timerSleep(){

		if(front == 0) return 0;

		int unblockedCounter = 0;
		qElem* tmp = front;
		qElem* prev = 0;

		int trBrojElemenata = elemNum;

		for(int j = 0; j < trBrojElemenata; j++){

			if(tmp->timeToSleep > 0){			// Moze biti samo ili > 0 ili -1.
				--tmp->timeToSleep;

				if(tmp->timeToSleep == 0){
					// Ako je prvi el u listi = 0

					if(prev == 0) {
						tmp->pcb->myStatus = PCB::Ready;
						Scheduler::put(tmp->pcb);
						//System::putPCB(tmp->pcb);
						myKernelSem->waitRes = 0;
						++unblockedCounter;
						popF();
						tmp = front;
					}
					else{
						prev->next = tmp->next;
						tmp->pcb->myStatus = PCB::Ready;
						Scheduler::put(tmp->pcb);
						//System::putPCB(tmp->pcb);
						myKernelSem->waitRes = 0;
						--elemNum;
						++unblockedCounter;

						if(tmp->next == 0) back = prev;
						tmp->next = 0;
						delete tmp;
						tmp = prev->next;
					}
				} else{ //ako nije time = 0;
					prev = tmp;
					tmp = tmp->next;
				}
			}
			else{	//ako je time = -1;
				prev = tmp;
				tmp = tmp->next;
			}
		}

		tmp = 0;
		prev = 0;

		return unblockedCounter;
}
/*
//printf("U TIMER SLEEPU\n");
		if(front == 0) return 0;

		int unblockedCounter = 0;
		qElem* tmp = front;
		qElem* prev = 0;



		for(int j = 0; j < elemNum; j++){

			if(tmp->timeToSleep > 0){			// Moze biti samo ili > 0 ili -1.
				--tmp->timeToSleep;



				tmp = tmp->next;
			}
		}


		tmp = front;


			while(tmp!=0 && tmp->timeToSleep == 0){
				tmp->pcb->myStatus = PCB::Ready;
				System::putPCB(tmp->pcb);
				myKernelSem->waitRes = 0;
				++unblockedCounter;
				popF();
				tmp = front;
				/*if(back != 0)
				//printf("front====back->pcb->id: %d\n", back->pcb->getId());
			}

			while(tmp!=0){
				while(tmp!=0 && tmp->timeToSleep !=0){
					prev = tmp;
					tmp = tmp->next;
				}

				if(tmp == 0) return unblockedCounter;

				prev->next = tmp->next;
				tmp->next = 0;

				tmp->pcb->myStatus = PCB::Ready;
				System::putPCB(tmp->pcb);
				myKernelSem->waitRes = 0;
				delete tmp;
				--elemNum;
				++unblockedCounter;

				if(prev->next == 0 && prev !=0) back = prev;
				tmp = prev->next;
			}

			return unblockedCounter;
}

*/

void SleepQueue::ispis(){
	//if(front == 0) printf("lista prazna\n");
	qElem* tmp = front;

	while(tmp!=0){
		//printf("Sleeping lista, nit: %d \n", tmp->pcb->getId());
		tmp = tmp->next;
	}
	//printf("BROJ EL U LISTI: %d\n", elemNum);

}


// Called from PCB::zeroHandler{  semq->removeFromSemaphor;} { .. sleepq-> removePCB } - kada stigne signal(0)
int SleepQueue::removePCB(PCB* mPCB){
	if(front == 0) return 0;

	qElem* tmp = front;
	qElem* prev = 0;

	while(tmp != 0 && tmp->pcb->myID != mPCB->myID){
		prev = tmp;
		tmp = tmp->next;
	}

	if(tmp != 0){		//samo ako je nadjen
		if(prev == 0) {
			myKernelSem->waitRes = 1;
			popF();
			return 1;
		}
		else{
				if(tmp->next == 0) back = prev;
				prev->next = tmp->next;
				myKernelSem->waitRes = 1;
			}

			tmp->next = 0;
			--elemNum;
			delete tmp;
			return 1;
	}

	return 0;
}





