#include "main.h"
#include "ivte.h"
#include "event.h"

Event ** IVTEntry::events = new Event* [256];

IVTEntry::IVTEntry (IVTNo ivtNo,pInterrupt routine) {
	lock
	oldRoutine = getvect (myIVTNo = ivtNo);
	setvect (ivtNo, newRoutine = routine);
	unlock
}

IVTEntry::~IVTEntry () {
	lock
	setvect (myIVTNo, oldRoutine);
	unlock
}

pInterrupt IVTEntry::getOldRoutine () {
	return oldRoutine;
}