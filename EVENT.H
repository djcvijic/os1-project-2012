#ifndef EVENT_H
#define EVENT_H


typedef unsigned char IVTNo;

class KernelEv;

class Event {
public:
	Event (IVTNo ivtNo);
	~Event ();
	int wait ();
	void signal();

private:
	KernelEv* myImpl;
};


#endif