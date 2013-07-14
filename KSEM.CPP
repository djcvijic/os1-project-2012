#include "main.h"
#include "pcb.h"
#include "ksem.h"
#include "thread.h"

int KernelSem::wait(){
	lock
	int retVal = 1;
	if (--sVal < 0) {
		PCB::running->status = ASLEEP;
		blockedList = new ThreadElem(PCB::running->myThread, (PCB*) PCB::running, blockedList);
		dispatch();
	}
	if (PCB::running->status == WOKEN) {
		retVal = 0;
		PCB::running->status = RUNNING;
	}
	if (semPreempt) dispatch();
	unlock
	return retVal;
}

void KernelSem::signal(){
	if (sVal++ < 0) {
		ThreadElem *preth = NULL;
		ThreadElem *tek = blockedList;
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