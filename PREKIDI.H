#ifndef PREKIDI_H
#define PREKIDI_H


#include "main.h"

void tick();

static void interrupt timer(...);

class Prekidi{
	static volatile Time brojac;
	
	friend void interrupt timer(...);
	
public:
	static volatile unsigned int lockFlag; //fleg za zabranu promene konteksta
	static volatile int zahtevana_promena_konteksta;
	
	// postavlja novu prekidnu rutinu
	static void inic();

	// vraca staru prekidnu rutinu
	static void restore();
};


#endif