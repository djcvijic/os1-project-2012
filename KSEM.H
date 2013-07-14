#ifndef KSEM_H
#define KSEM_H


#include "pcb.h"

class KernelSem {

private:
	int sVal;
	ThreadElem * blockedList;
	friend class Semaphore;
	
public:
	KernelSem(int init);
	~KernelSem();
	
};


#endif