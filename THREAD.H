#ifndef THREAD_H
#define THREAD_H


#include "main.h"

void interrupt timer(...);
void dispatch();

class PCB; // Kernel's implementation of a user's thread
class Thread {
public:
void start();
int waitToComplete(); // returns: 1 – run completed, 0 – woken up
int wakeUp();  // returns: 1 – deblocked, 0 – no effect
static int sleep(Time timeToSleep); // returns: 1 – sleep completed, 0 – woken up
ID getId();
TName getName();      
static Thread * getThreadById(ID id);
static ID getIdOf(TName name);
virtual ~Thread();
protected:
friend class PCB;
Thread (TName name,StackSize stackSize=4096,Time timeSlice=2);
virtual void run() {}
private:
PCB* myPCB;
};

static ID statID = 0;


#endif