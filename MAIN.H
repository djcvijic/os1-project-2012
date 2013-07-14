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


#endif