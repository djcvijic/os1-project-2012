#include "main.h"
#include "ivte.h"
#include "event.h"


IVTEntry::IVTEntry (IVTNo ivtNo,pInterrupt routine) {
	lock
	oldRoutine = getVect (myIVTNo = ivtNo);
	setVect (ivtNo, newRoutine = routine);
	unlock
}

IVTEntry::~IVTEntry () {
	lock
	setVect (ivtNo, oldRoutine);
	unlock
}

pInterrupt IVTEntry::getOldRoutine () {
	return oldRoutine;
}