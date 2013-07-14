#include "main.h"
#include "pcb.h"
#include "ksem.h"
#include "thread.h"

KernelSem::KernelSem (int init) {
	lock
	sVal = init;
	blockedList = NULL;
	unlock
}

KernelSem::~KernelSem () {
	lock
	ThreadElem * stari;
	while (blockedList) {
		stari = blockedList;
		stari->thread->wakeUp();
		blockedList = blockedList->sled;
		delete stari;
	}
	unlock
}