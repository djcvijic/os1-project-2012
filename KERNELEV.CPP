#include "main.h"
#include "event.h"
#include "kernelev.h"
#include "ivte.h"

EventElem * KernelEv::eventList = NULL;

KernelEv::KernelEv(IVTNo ivtNo, Event event){
	lock
	blockedList = NULL;
	myEvent = event;
	myIVTNo = ivtNo;
	eventList = new EventElem (this, eventList);
	unlock
}

KernelEv::~KernelEv(){
	lock
	EventElem * preth = NULL;
	for (EventElem * tek = eventList; tek; tek = tek->sled) {
		if (myIVTNo == tek->kernelEv->myIVTNo) {
			if (preth) preth->sled = tek->sled;
			else eventList = tek->sled;
			delete tek;
		}
		else preth = tek;
	}
	delete ivte;
	signal();
	unlock
}

int KernelEv::wait(){
	lock
	int retVal = 1;
	PCB::running->status = ASLEEP;
	blockedList = new ThreadElem(PCB::running->myThread, (PCB*) PCB::running, blockedList);
	dispatch();
	if (PCB::running->status == WOKEN) {
		retVal = 0;
		PCB::running->status = RUNNING;
	}
	unlock
	return retVal;
}

void KernelEv::signal(){
	for (ThreadElem* stari = blockedList; stari; stari = blockedList) {
		Scheduler::put(stari->pcb);
		stari->pcb->status = QUEUED;
		blockedList = blockedList->sled;
		delete stari;
	}
	dispatch();
}

Event * KernelEv::getEventByNo (IVTNo ivtNo) {
	lock
	Event * retVal = NULL;
	for (EventElem * tek = eventList; tek; tek = tek->sled) {
		if (tek->kernelEv->myIVTNo == ivtNo) {
			retVal = tek->kernelEv->myEvent;
			break;
		}
	}
	unlock
	return retVal;
}