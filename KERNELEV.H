#ifndef KERNELEV_H
#define KERNELEV_H


#include "event.h"
#include "pcb.h"

class KernelEv {
private:
	ThreadElem * blockedList;
	IVTNo myIVTNo;

	friend class Event;
	
	KernelEv(IVTNo ivtNo);
	~KernelEv();
	int wait();
	void signal();
};


#endif