#include "pcb.h"


ThreadElem * PCB::threadList = NULL;
volatile PCB* PCB::running = NULL;

TName PCB::getTName () volatile {
	return tName;
}

PCB::PCB (Thread *thr, Time slice){
	lock
	myThread = thr;
	kvant = slice;
	timeToSleep = 0;
	awaitedThread = NULL;
	unlock
}

PCB::~PCB () {
	ThreadElem * stari = NULL;
	lock
	for (ThreadElem * tek = PCB::threadList; tek != NULL; tek = tek->sled) {
		if (tek->pcb->tId == tId) {
			if (stari) stari->sled = tek->sled;
			else PCB::threadList = tek->sled;
			delete tek;
			break;
		}
		else stari = tek;
	}
	delete [] stack;
	unlock
}

void PCB::createProcess(void (*body)(), StackSize stackSize){
	unsigned* st1 = new unsigned[stackSize];

	st1[stackSize - 1] =0x200;//setovan I fleg u
			  // pocetnom PSW-u za nit
	st1[stackSize - 2] = FP_SEG(body);
	st1[stackSize - 3] = FP_OFF(body);

	sp = FP_OFF(st1 + stackSize - 12); //svi sacuvani registri
	//pri ulasku u interrupt
	//rutinu
	ss = FP_SEG(st1 + stackSize - 12);
	bp = FP_OFF(st1 + stackSize - 12);
	
	stack = st1;
}

void PCB::run() volatile {
	myThread->run();
	lock
	PCB::running->status = FINISHED;
	dispatch();
	unlock
}

/*void exitThread(){
	PCB::running->status = FINISHED;
	dispatch();
}*/