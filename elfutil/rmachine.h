// rmachine.h

#ifndef rmachine_h
#define rmachine_h

#include "lmachine.h"

struct rnative{
	rnative *next;
	void *func;
	const char *name;
	int	 stub[8];
};

#define STUB 1
#define XSTUB 2
#define STUB5 3
#define STUB6 4

struct rmachine{
	lmachine *lmac;
	llogic	*lcache,*lstart;
	int		maxr,maxs,maxv;
	void	**jcache,**jptr;
	int		totcode;
	
	rmachine();

	virtual rnative *addnative(void *f,const char *n,int s)=0;
	virtual int *compile()=0;
//	virtual void dumpop(int op,int loc)=0;
	virtual void dump(int *code)=0;
	virtual void linkjumps()=0;
	virtual void addvregs()=0;
	virtual int branch(llogic *l);	//extend base implementation to correct for chip specific stuff

//	void linkstack(int RA,int SP);
	void filldelays();
	void fillxchks();
	void fixloads();
	void checkreg(int n);
	void dumplogic();
	void addjump(void *j);
	void cludgegetputs();

	int calcaddress(int d);
};

#endif
