#ifndef MAIN_H
#define MAIN_H


//pretpostavljeni memorijski model: huge

// Zabranjuje prekide
#define lock { asm pushf; asm cli; }

// Dozvoljava prekide
#define unlock asm popf;

// Zabrana ispisa
#define lockCout Prekidi::lockFlag=0;

// Dozvola ispisa
#define unlockCout Prekidi::lockFlag=1;\
		if (Prekidi::zahtevana_promena_konteksta) dispatch();

		
#include <iostream.h>
#include <stdlib.h>
#include <dos.h>
#include <schedule.h>

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;
typedef char* TName;
enum PCBStatus { READY, QUEUED, RUNNING, ASLEEP, WOKEN, FINISHED };


#endif