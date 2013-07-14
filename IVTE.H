#ifndef IVTE_H
#define IVTE_H


#define PREPAREENTRY(brUl,pozStaru) IVTEntry * ulaz##brUl; \
void interrupt prekid##brUl (...) { \
if ( pozStaru ) (*(ulaz##brUl##->getOldRoutine()))(); \
KernelEv::getEventByNo( brUl )->signal(); \
} \
ulaz##brUl = new IVTEntry ( brUl , prekid##brUl );


#include "event.h"

typedef void interrupt (*pInterrupt)(...);

class IVTEntry{
private:
	pInterrupt oldRoutine;
	pInterrupt newRoutine;
	IVTNo myIVTNo;

	IVTEntry(IVTNo ivtNo,pInterrupt routine);
	~IVTEntry();
	pInterrupt getOldRoutine();
	
	friend class KernelEv;
};


#endif