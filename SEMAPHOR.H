#ifndef SEMAPHOR_H
#define SEMAPHOR_H


class KernelSem;

class Semaphore {
public:
	Semaphore (int init=1);
	~Semaphore ();

	virtual int wait ();
	virtual void signal();

	int val () const; // Returns the current value of the semaphore

private:
	KernelSem* myImpl;
};


#endif