#include "thread.h"
#include "pcb.h"
#include "prekidi.h"


void globRun(){
	PCB::running->run();
}

char *strcpy(char *dst, const char *src)
{
 char *ptr;
 ptr = dst;
 while (*src) *dst++ = *src++;
 *dst = 0;
 return(ptr);
}

void Thread::start(){
	lock
	Scheduler::put(myPCB);
	myPCB->status = QUEUED;
	unlock
}

int Thread::waitToComplete(){
	if ((myPCB->status == FINISHED) || (myPCB->status == READY)) return 1;
	lock
	PCB::running->awaitedThread = myPCB;
	PCB::running->status = ASLEEP;
	unlock
	dispatch();
	lock
	int retValue = (PCB::running->status == WOKEN) ? 0 : 1;
	PCB::running->status = RUNNING;
	unlock
	return retValue;
}

int Thread::wakeUp(){
	lock
	if (myPCB->status != ASLEEP) {
		unlock
		return 0;
	}
	else {
		myPCB->status = WOKEN;
		unlock
		return 1;
	}
}

int Thread::sleep(Time timeToSleep){
	lock
	if (timeToSleep == 0) {
		unlock
		return 1;
	}
	PCB::running->status = ASLEEP;
	PCB::running->timeToSleep = timeToSleep;
	unlock
	dispatch();
	lock
	int retValue = (PCB::running->status == WOKEN) ? 0 : 1;
	PCB::running->status = RUNNING;
	unlock
	return retValue;
}

ID Thread::getId(){
	return myPCB->tId;
}

TName Thread::getName(){
	return myPCB->tName;
}

Thread * Thread::getThreadById(ID id){
	for (ThreadElem * tek = PCB::threadList; tek != NULL; tek = tek->sled) {
		if (tek->pcb->tId == id) return tek->thread;
	}
	return NULL;
}

ID Thread::getIdOf(TName name){
	for (ThreadElem * tek = PCB::threadList; tek != NULL; tek = tek->sled) {
		if (tek->pcb->tName == name) return tek->pcb->tId;
	}
	return NULL;
}

Thread::~Thread(){
	while (!this->waitToComplete()) {}
}

Thread::Thread (TName name,StackSize stackSize,Time timeSlice){
	lock

	myPCB = new PCB(this,timeSlice);
	myPCB->tId = ++statID;
	char* newName = new char[12];
	
	if (name) {
		strcpy (newName, name);
	}
	else {
		strcpy (newName, "thread");
		char * toAppend = itoa (myPCB->tId, new char[6], 10);
		strcpy (newName + 6, toAppend);
	}
	myPCB->tName = newName;
	myPCB->status = READY;
	myPCB->createProcess(globRun, stackSize);
	PCB::threadList = new ThreadElem(this, myPCB, PCB::threadList);
	cout<<"napravio "<<myPCB->tName<<" id= "<<myPCB->tId<<endl;
	unlock
}

void dispatch(){ // sinhrona promena konteksta
	lock
	Prekidi::zahtevana_promena_konteksta = 1;
	timer();
	unlock
}