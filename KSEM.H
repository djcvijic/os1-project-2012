#ifndef KSEM_H
#define KSEM_H


#include "pcb.h"

extern int semPreempt;

class KernelSem {

private:
	int sVal;
	ThreadElem * blockedList;
	
	friend class Semaphore;
	
	int wait ();
	void signal();
	int val () const;
	
public:
	KernelSem(int init);
	~KernelSem();
	
};


#endif