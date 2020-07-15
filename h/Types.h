#ifndef TYPES_H_
#define TYPES_H_

//
// Define boolean
//
typedef int bool;
#define true 1
#define false 0

//	Define nullptr
//
//
#define nullptr 0


// staviti ovaj extern u System.h
extern volatile bool dispatchCalled;

#define intInterrupt(number) asm int number








#endif /* TYPES_H_ */
