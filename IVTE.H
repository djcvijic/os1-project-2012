#ifndef IVTE_H
#define IVTE_H


#define PREPAREENTRY(brUl,pozStaru) void interrupt prekid##brUl (...); \
IVTEntry * ulaz##brUl = new IVTEntry ( brUl , prekid##brUl ); \
void interrupt prekid##brUl (...) { \
if ( pozStaru ) (*(ulaz##brUl##->getOldRoutine()))(); \
IVTEntry::events[ brUl ]->signal(); \
}

#include "event.h"

typedef void interrupt (*pInterrupt)(...);

class IVTEntry{
private:
	pInterrupt oldRoutine;
	pInterrupt newRoutine;
	IVTNo myIVTNo;

	~IVTEntry();
	
	friend class KernelEv;
	
public:	
	static Event ** events;

	pInterrupt getOldRoutine();
	IVTEntry(IVTNo ivtNo,pInterrupt routine);
};


#endif