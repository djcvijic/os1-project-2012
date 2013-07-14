#ifndef PCB_H
#define PCB_H

#include "main.h"
#include "thread.h"

enum PCBStatus { READY, QUEUED, RUNNING, ASLEEP, WOKEN, FINISHED };

struct ThreadElem{
	Thread * thread;
	PCB * pcb;
	ThreadElem * sled;
	ThreadElem(Thread * x, PCB * y, ThreadElem * z){
		thread = x; pcb = y; sled = z;
	}
};

class PCB{
 private:
	unsigned sp;
	unsigned ss;
	unsigned bp;
	Time timeToSleep;
	Time kvant;
	Thread * myThread;
	PCB * awaitedThread;
	TName tName;
	ID tId;
	volatile PCBStatus status;
	unsigned * stack;
	
	friend class Thread;
	friend class KernelSem;
	friend class KernelEv;
	friend void interrupt timer(...);
	friend void exitThread();
	friend int main (int argc, char* argv[]);
	
	void start();
	int waitToComplete();
	int wakeUp();
	static int sleep(Time tts);
	
public:
	static ThreadElem * threadList;
	static volatile PCB* running;
	static ID statID;
	TName getTName () volatile;
	PCB (Thread * thr = NULL, Time slice = 2);
	~PCB ();
	void createProcess(void (*body)(), StackSize stackSize);
	void run() volatile;
};


#endif