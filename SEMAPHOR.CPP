#include "semaphor.h"
#include "ksem.h"
#include "pcb.h"
#include "thread.h"

Semaphore::Semaphore (int init) {
	lock
	myImpl = new KernelSem(init);
	unlock
}

Semaphore::~Semaphore () {
	lock
	delete myImpl;
	unlock
}

int Semaphore::wait() {
	lock
	int retVal = 1;
	if (--myImpl->sVal < 0) {
		PCB::running->status = ASLEEP;
		myImpl->blockedList = new ThreadElem(PCB::running->myThread, (PCB*) PCB::running, myImpl->blockedList);
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

void Semaphore::signal() {
	lock
	if (myImpl->sVal++ < 0) {
		ThreadElem *preth = NULL;
		ThreadElem *tek = myImpl->blockedList;
		for (; tek->sled; tek = tek->sled) {
			preth = tek;
		}
		if (preth) preth->sled = NULL;
		else myImpl->blockedList = NULL;
		Scheduler::put(tek->pcb);
		tek->pcb->status = QUEUED;
		delete tek;
	}
	if (semPreempt) dispatch();
	unlock
}

int Semaphore::val() const {
	return myImpl->sVal;
}