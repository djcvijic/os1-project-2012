#include "main.h"
#include "kernelev.h"
#include "pcb.h"
#include "thread.h"

KernelEv::KernelEv(IVTNo ivtNo){
	lock
	blockedList = NULL;
	myIVTNo = ivtNo;
	unlock
}

KernelEv::~KernelEv(){
	lock
	signal();
	unlock
}

int KernelEv::wait(){
	PCB::running->status = ASLEEP;
	blockedList = new ThreadElem(PCB::running->myThread, (PCB*) PCB::running, blockedList);
	dispatch();
	if (PCB::running->status == WOKEN) {
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
		return 0;
	}
	return 1;
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