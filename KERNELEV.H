#ifndef KERNELEV_H
#define KERNELEV_H


#include "event.h"
#include "pcb.h"

struct EventElem{
	KernelEv * kernelEv;
	EventElem * sled;
	EventElem(KernelEv * x, EventElem * y){
		kernelEv = x; sled = y;
	}
};

class KernelEv {
private:
	ThreadElem * blockedList;
	//Event * myEvent;
	IVTNo myIVTNo;
	
	static EventElem * eventList;

	friend class Event;
	
	KernelEv(IVTNo ivtNo);
	~KernelEv();
	int wait();
	void signal();
	
//public:
	//static Event * getEventByNo (IVTNo ivtNo);
};


#endif