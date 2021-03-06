#include "pcb.h"


ThreadElem * PCB::threadList = NULL;
volatile PCB* PCB::running = NULL;
ID PCB::statID = 0;

void PCB::run() volatile {
	myThread->run();
	lock
	PCB::running->status = FINISHED;
	dispatch();
	unlock
}

TName PCB::getTName () volatile {
	return tName;
}

PCB::PCB (Thread *thr, Time slice){
	lock
	tId = statID++;
	myThread = thr;
	kvant = slice;
	status = READY;
	timeToSleep = 0;
	awaitedThread = NULL;
	unlock
}

PCB::~PCB () {
	lock
	ThreadElem * stari = NULL;
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

void PCB::start () {
	Scheduler::put(this);
	status = QUEUED;
}

int PCB::waitToComplete() {
	if ((status == FINISHED) || (status == READY)) return 1;
	running->awaitedThread = this;
	running->status = ASLEEP;
	dispatch();
	int retValue = (running->status == WOKEN) ? 0 : 1;
	running->status = RUNNING;
	return retValue;
}

int PCB::wakeUp() {
	if (status != ASLEEP) {
		return 0;
	}
	else {
		status = WOKEN;
		return 1;
	}
}

int PCB::sleep(Time tts) {
	if (tts == 0) {
		dispatch();
		return 1;
	}
	running->status = ASLEEP;
	running->timeToSleep = tts;
	dispatch();
	int retValue = (running->status == WOKEN) ? 0 : 1;
	running->status = RUNNING;
	return retValue;
}