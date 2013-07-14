#include "main.h"
#include "pcb.h"
#include "ksem.h"
#include "thread.h"

int KernelSem::wait(){
	int retVal = 1;
	if (--sVal < 0) {
		PCB::running->status = ASLEEP;
		blockedList = new ThreadElem(PCB::running->myThread, (PCB*) PCB::running, blockedList);
		dispatch();
	}
	if (PCB::running->status == WOKEN) {
		sVal++;
		ThreadElem* preth = NULL;
		for (ThreadElem* tek = blockedList; tek; tek = tek->sled) {
			if (tek->thread->getId() == PCB::running->tId) {
				if (preth) preth->sled = tek->sled;
				else blockedList = tek->sled;
				delete tek;
				break;
			}
			else preth = tek;
		}
		PCB::running->status = RUNNING;
		retVal = 0;
	}
	if (semPreempt) dispatch();
	return retVal;
}

void KernelSem::signal(){
	if (sVal++ < 0) {
		ThreadElem *preth = NULL;
		ThreadElem *tek = blockedList; // posto je sVal bio < 0, ovo nikad nije NULL
		for (; tek->sled; tek = tek->sled) {
			preth = tek;
		}
		if (preth) preth->sled = NULL;
		else blockedList = NULL;
		Scheduler::put(tek->pcb);
		tek->pcb->status = QUEUED;
		delete tek;
	}
	if (semPreempt) dispatch();
}

int KernelSem::val () const {
	return sVal;
}

KernelSem::KernelSem (int init) {
	lock
	sVal = init;
	blockedList = NULL;
	unlock
}

KernelSem::~KernelSem () {
	lock
	ThreadElem * stari;
	while (blockedList) {
		stari = blockedList;
		stari->thread->wakeUp();
		blockedList = blockedList->sled;
		delete stari;
	}
	unlock
}