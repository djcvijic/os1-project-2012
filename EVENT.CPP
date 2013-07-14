#include "main.h"
#include "event.h"
#include "kernelev.h"
#include "ivte.h"

Event::Event (IVTNo ivtNo) {
	lock
	myImpl = new KernelEv(ivtNo);
	IVTEntry::events[ivtNo] = this;
	unlock
}

Event::~Event () {
	lock
	delete myImpl;
	unlock
}

int Event::wait() {
	return myImpl->wait();
}

void Event::signal() {
	lock
	myImpl->signal();
	unlock
}