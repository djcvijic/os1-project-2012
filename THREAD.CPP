#include "main.h"
#include "thread.h"
#include "pcb.h"
#include "prekidi.h"


void globRun(){
	PCB::running->run();
}

char *strcpy(char *dst, const char *src)
{
	lock
	char *ptr;
	ptr = dst;
	while (*src) *dst++ = *src++;
	*dst = 0;
	unlock
	return(ptr);
}

int strcmp (const char * src, const char * dst) {
	lock
	int ret = 0 ;
	while( ! (ret = *(unsigned char *)src - *(unsigned char *)dst) && *dst)
			++src, ++dst;
	if ( ret < 0 )
			ret = -1 ;
	else if ( ret > 0 )
			ret = 1 ;
	unlock
	return( ret );
}

void dispatch(){ // sinhrona promena konteksta
	lock
	Prekidi::zahtevana_promena_konteksta = 1;
	timer();
	unlock
}

void Thread::start(){
	lock
	myPCB->start();
	unlock
}

int Thread::waitToComplete(){
	lock
	int retVal = myPCB->waitToComplete();
	unlock
	return retVal;
}

int Thread::wakeUp(){
	lock
	int retVal = myPCB->wakeUp();
	unlock
	return retVal;
}

int Thread::sleep(Time timeToSleep){
	lock
	int retVal = PCB::sleep(timeToSleep);
	unlock
	return retVal;
}

ID Thread::getId(){
	return myPCB->tId;
}

TName Thread::getName(){
	return myPCB->tName;
}

Thread * Thread::getThreadById(ID id){
	lock
	for (ThreadElem * tek = PCB::threadList; tek != NULL; tek = tek->sled) {
		if (tek->pcb->tId == id) {
			unlock
			return tek->thread;
		}
	}
	unlock
	return NULL;
}

ID Thread::getIdOf(TName name){
	lock
	for (ThreadElem * tek = PCB::threadList; tek != NULL; tek = tek->sled) {
		if (0 == strcmp(tek->pcb->tName, name)) {
			unlock
			return tek->pcb->tId;
		}
	}
	unlock
	return NULL;
}

Thread::~Thread(){
	while (!this->waitToComplete()) {}
}

Thread::Thread (TName name,StackSize stackSize,Time timeSlice){
	lock
	myPCB = new PCB(this,timeSlice);
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
	myPCB->createProcess(globRun, stackSize);
	PCB::threadList = new ThreadElem(this, myPCB, PCB::threadList);
	/*
	lockCout
	cout<<"napravio "<<myPCB->tName<<" id = "<<myPCB->tId<<endl;
	unlockCout
	*/
	unlock
}