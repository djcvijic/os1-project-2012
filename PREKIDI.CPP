#include "main.h"
#include "pcb.h"
#include "prekidi.h"
#include "user.h"


volatile Time Prekidi::brojac = 20;
volatile unsigned int Prekidi::lockFlag = 1; //fleg za zabranu promene konteksta
volatile int Prekidi::zahtevana_promena_konteksta = 0;

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

static unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

void tick();

void interrupt timer(...){	// prekidna rutina
	if (!Prekidi::zahtevana_promena_konteksta) Prekidi::brojac--;
	
	for (ThreadElem * tek = PCB::threadList; tek != NULL; tek = tek->sled){
		if ((tek->pcb->status == WOKEN) && (tek->pcb->timeToSleep || tek->pcb->awaitedThread)) {
			Scheduler::put(tek->pcb);
			tek->pcb->timeToSleep = 0;
			tek->pcb->awaitedThread = NULL;
		}
		else if (tek->pcb->status == ASLEEP) {
			if ((tek->pcb->awaitedThread) && (tek->pcb->awaitedThread->status == FINISHED)) {
				Scheduler::put(tek->pcb);
				tek->pcb->status = QUEUED;
				tek->pcb->awaitedThread = NULL;
			}
			else if ((tek->pcb->timeToSleep == 1) && (!Prekidi::zahtevana_promena_konteksta)) {
				Scheduler::put(tek->pcb);
				tek->pcb->status = QUEUED;
				tek->pcb->timeToSleep = 0;
			}
			else if ((tek->pcb->timeToSleep > 1) && (!Prekidi::zahtevana_promena_konteksta)) {
				tek->pcb->timeToSleep--;
			}
		}
	}
	
	// poziv stare prekidne rutine koja se
     // nalazila na 08h, a sad je na 60h
     // poziva se samo kada nije zahtevana promena
     // konteksta – tako se da se stara
     // rutina poziva samo kada je stvarno doslo do prekida
	if(!Prekidi::zahtevana_promena_konteksta) {
		tick();
		asm int 60h;
	}
	
	if ((Prekidi::brojac == 0 && PCB::running->kvant > 0) || Prekidi::zahtevana_promena_konteksta) {
	 if (Prekidi::lockFlag){
		Prekidi::zahtevana_promena_konteksta = 0;
		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		//ispis unutar prekidne rutine
		/*lock
		lockCout;
		cout<<"Promena konteksta! Brojac = "<<brojac<<endl;
		Prekidi::lockFlag=1;
		unlock*/
		//kraj ispisa

		if ((PCB::running->status != FINISHED) && (PCB::running->status != ASLEEP)) {
			Scheduler::put((PCB *) PCB::running);
			PCB::running->status = QUEUED;
		}
		else if (PCB::running->status == FINISHED) {
			delete (PCB*) PCB::running;
		}
		
		PCB::running = Scheduler::get();
		if (PCB::running->status != WOKEN) PCB::running->status = RUNNING;

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		Prekidi::brojac = PCB::running->kvant;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}
	  }
	  else Prekidi::zahtevana_promena_konteksta = 1;

	}
} //timer

// postavlja novu prekidnu rutinu
void Prekidi::inic(){

	lock
	asm{
		
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		
	}
	unlock
}

// vraca staru prekidnu rutinu
void Prekidi::restore(){

	lock
	asm {
		
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		
	}
	unlock
}