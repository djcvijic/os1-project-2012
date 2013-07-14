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
	return myImpl->wait();
}

void Semaphore::signal() {
	lock
	myImpl->signal();
	unlock
}

int Semaphore::val() const {
	return myImpl->val();
}