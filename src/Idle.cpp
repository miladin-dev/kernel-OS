#include "Idle.h"
#include "System.h"
#include "stdio.h"
#ifndef IDLETHR_CPP_
#define IDLETHR_CPP_

Idle::Idle(): Thread(defaultStackSize, 1) {}
Idle::~Idle(){ }


void Idle::run() {
	//syncPrintf("\n //IDLE NIT \n");
	/*int counter = 0;
	while(counter == 0) {
		counter = counter + 1;
		counter = counter - 1;

	}*/

	while(true) {}

}

void Idle::start() {


	myPCB->setMyStatus(PCB::Ready);


}



#endif /* IDLETHR_CPP_ */
