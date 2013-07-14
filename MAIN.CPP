#include "main.h"
#include "thread.h"
#include "pcb.h"
#include "prekidi.h"
#include "user.h"

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
	lock
	Prekidi::inic();
	PCB::running = new PCB(NULL, 0);
	PCB::running->tName = "MAIN";
	PCB::running->status = RUNNING;
	UserThread *userThread = new UserThread ("USERMAIN", defaultStackSize, defaultTimeSlice, argc, argv);
	userThread->start();
	PCB * userPCB;
	for (ThreadElem * tek = PCB::threadList; tek; tek = tek->sled)
		if (tek->thread == userThread) {
			userPCB = tek->pcb;
			break;
		}
	unlock
	while (userPCB->status != FINISHED) dispatch();
	lock
	int mainRetVal = userThread->getRetVal();
	delete userThread;
	Prekidi::restore();
	unlock
	return mainRetVal;
}