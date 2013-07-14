#include "semaphor.h"
#include "ksem.h"

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
	lock
	int retVal = myImpl->wait();
	unlock
	return retVal;
}

void Semaphore::signal() {
	lock
	myImpl->signal();
	unlock
}

int Semaphore::val() const {
	lock
	int retVal = myImpl->val();
	unlock
	return retVal;
}