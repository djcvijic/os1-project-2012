#include "main.h"
#include "thread.h"
#include "pcb.h"
#include "prekidi.h"
#include "semaphor.h"
#include "user.h"
#include "bounded.h"
#include "intLock.h"

/*
void a(){
	for (int i = 0; i < 20; ++i) {
		lock
		lockCout
		cout<<"u BRZOJ niti"<<PCB::running->getTName()<<", i = "<<i<<endl;
		unlockCout
		unlock
		for (int k = 0; k < 20000; ++k)
			for (int j = 0; j < 20000; ++j);
	}
}

void b(){

	for (int i =0; i < 5; ++i) {
			lock

			//begin lock
			lockCout;
			//end lock
			cout<<"u niti "<<PCB::running->getTName()<<", i = "<<i<<endl;
			//begin unlock
			unlockCout;
			//end unlock

			unlock
			for (int k = 0; k<20000; ++k)
				for (int j = 0; j <20000; ++j);
	}
	
	lockCout
	cout<<"uspavljuje se "<<PCB::running->getTName()<<" na 5 sekundi."<<endl;
	unlockCout
	
	Thread::sleep(90);
	
	lockCout
	cout<<"budi se "<<PCB::running->getTName()<<endl;
	unlockCout

	for (int ii =5; ii < 10; ++ii) {
			lock

			//begin lock
			lockCout;
			//end lock
			cout<<"u niti "<<PCB::running->getTName()<<", i = "<<ii<<endl;
			//begin unlock
			unlockCout;
			//end unlock

			unlock
			for (int k = 0; k<20000; ++k)
				for (int j = 0; j <20000; ++j);
	}
}

class ThreadA: public Thread{
	virtual void run(){
		a();
	}
	public:
	ThreadA (TName name,StackSize stackSize,Time timeSlice):
		Thread(name,stackSize,timeSlice){}
	virtual ~ThreadA () {
		while (!this->waitToComplete()) {
		}
	} 
};

class ThreadB: public Thread{
	virtual void run(){
		b();
	}
	public:
	ThreadB (TName name,StackSize stackSize,Time timeSlice):
		Thread(name,stackSize,timeSlice){}
	virtual ~ThreadB () {
		while (!this->waitToComplete()) {
		}
	} 
};

int userMain(int argc, char* argv[]){
	intLock

	const int numThreads = atoi(argv[2]);
	ThreadA **thra = new ThreadA * [numThreads];
	ThreadB **thrb = new ThreadB * [numThreads];
	
	for (int n = 0; n < numThreads; n++){
		thra[n] = new ThreadA(NULL, defaultStackSize, atoi(argv[3]));
		thra[n]->start();
		thrb[n] = new ThreadB(NULL, defaultStackSize, atoi(argv[3]));
		thrb[n]->start();
	}

	intUnlock

	//for (int i = 0; i < 30; ++i) {
		//lockCout
		//cout<<"userMain "<<i<<endl;
		//unlockCout

		//for (int j = 0; j< 30000; ++j)
			//for (int k = 0; k < 30000; ++k);
	//}

	//thr[0]->waitToComplete();
	
	intLock
	for (int i=0; i<numThreads; i++){
		delete thra[i];
		delete thrb[i];
	}  
	delete [] thra;
	delete [] thrb;
	
	lockCout
	cout<<"Happy End"<<endl;
	unlockCout
	
	intUnlock
	return 19 + argc - argc;
}
*/

int userMain(int argc, char* argv[]);

class UserThread: public Thread{
	int argc;
	char** argv;
	int myRetVal;
	virtual void run(){
		myRetVal = userMain(argc, argv);
	}
public:
	UserThread (TName name,StackSize stackSize,Time timeSlice, int x, char** y): Thread(name,stackSize,timeSlice)
	{
		argc = x;
		argv = y;
	}
	virtual ~UserThread ();
	int getRetVal () { return myRetVal; }
};

UserThread::~UserThread() {
	while (!this->waitToComplete()) {}
}


int main (int argc, char* argv[]){
	Prekidi::inic();
	lock
	PCB::running = new PCB(NULL, 0);
	PCB::running->tName = "MAIN";
	PCB::running->status = RUNNING;
	PCB::running->tId = 0;
	UserThread *userThread = new UserThread ("USERMAIN", 4096, 2, argc, argv);
	userThread->start();
	unlock
	PCB * userPCB;
	for (ThreadElem * tek = PCB::threadList; tek; tek = tek->sled)
		if (tek->thread == userThread) {
			userPCB = tek->pcb;
			break;
		}
	while (userPCB->status != FINISHED) {
		//lock
		//cout<<".";
		//unlock
		dispatch();
	}
	int mainRetVal = userThread->getRetVal();
	delete userThread;
	Prekidi::restore();
	return mainRetVal;
}