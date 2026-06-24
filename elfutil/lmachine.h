// lmachine.h

#pragma once

#include <cstdint>

// note: ops<MOV can not be shuffled by register reordering
// INV BRA RET GET all require delay slots in mips 
// typedef unsigned short ushort;
// typedef unsigned char uchar;

enum lop5{
	PAD,LNK,INV,RTS,GET,XCPT,XCHK,UNK0,
	MOV,iADD,iSUB,iAND,iOR,iXOR,iNEG,iSHL,
	iSHR,iUSHR,iDIV,iMUL,iREM,SWCH,iSGN,iSLT,
	bBRA,bBEQ,bBNE,bBAL,bBLT,bBGE,bBGT,bBLE
};

#define MAXOP 31

#define REG  0x10000000			//was 0x00000000
#define REGX 0x20000000
#define IND  0x40000000
#define INDB 0x50000000
#define INDS 0x60000000
#define INDC 0x70000000
#define IMM  0x80000000
#define IMU  0xc0000000

#define BSTART 0x01000000
#define BEND   0x02000000
#define FILLER 0x04000000

// INV stores parameter count in llogic.dest>>8
// sp set for boundary llogic only
struct llogic	{
	llogic	*next;
	union{
		llogic	*prev;
		int		pos;
	};
    int	address;
    int opcode;
    int dest;
    int src0;
    int src1;

    //flags   opcode=b0..b7=operand b8..b15=sp b24..b31=flags 

	inline void set(int a,int sp,int o,int d=0,int s0=0,int s1=0){
		address=a;opcode=(sp<<8)|(o&255);dest=d;src0=s0;src1=s1;//flags=0;
	}

    inline int isbranch() {
        return ((opcode&0x30)==0x30);
    }

    void dumpreg(int r);
	void dump();
};

struct lreg{
	lreg	*next,*link;	//next links regs in block, link joins regs across blocks (circular) 
	int		start,end;
	uint8_t	rsrc,rdest;
	short	tribs;
	lreg	*orig;			//0=this 1=parent >1=this

	void init(int s,int d,int b,int e){
		next=0;link=this;
		rsrc=s;rdest=d;start=b;end=e;
		tribs=0;orig=0;
	}
};

struct lblock{
	int		type;			//llogic operator ending block ie INV
	int		spin,spout;		//sp adjustment required due to non sequential program flow
	int		start,end;		//llogic address range
	lreg	*ulist;			//uninitialised
	lreg	*ilist;			//initialised
	int		nlink;			//successive block count
	lblock	**link;			//ptr to list
	int		nfeed;			//number of feeds
	lblock	**feed;			//feed list
	
    //visitor=0;lptr=0;
	void init() {
        type=start=end=0;ulist=ilist=0;nlink=0;nfeed=0;
    }	

    //changed so end=a+1
	void regwrite(lreg **rend,int r,int a) {
		lreg *j;
		j=*rend;(*rend)++;j->init(r,0,a,a+1);	//,0
		j->orig=j;j->tribs=-1;
		j->next=ilist;ilist=j;j++;
	}
	
    //register,address
	lreg *regread(lreg **rend,int r,int a) {
		lreg *j,*f;
		int l;
		f=0;l=-1;
		j=ilist;while (j) {if (j->rsrc==r && j->start>l) {l=j->start;f=j;} j=j->next;}
		j=ulist;while (j) {if (j->rsrc==r && j->start>l) {l=j->start;f=j;} j=j->next;}
		if (f && (a>f->start)) {if (a>f->end) f->end=a;return f;}
		j=*rend;(*rend)++;j->init(r,0,start-1,a);	//,1
		j->next=ulist;ulist=j;return j;
	}

    // register r,address a
	lreg *findsrcreg(int r,int a) {
		lreg *j;
		if (r==0) return 0;
		for (j=ilist;j;j=j->next) {if ((r==j->rsrc)&&(a>j->start)&&(a<=j->end)) return j;}
		for (j=ulist;j;j=j->next) {if ((r==j->rsrc)&&(a>j->start)&&(a<=j->end)) return j;}
		return 0;
	}

    //register r,address a
	lreg *finddestreg(int r,int a) {
		lreg *j;
		if (r==0) return 0;
		for (j=ilist;j;j=j->next) {if ((r==j->rsrc)&&(a==j->start)) return j;}
		return 0;
	}
};

struct lmachine{
    //llogic cache
	llogic *lcache,*lend;				
    //register cache	
	lreg *rcache,*rend;					
    //block cache
	lblock *bcache,*bend;				
    //linkage list	
	lblock **blink,**blonk;				
    //assigned registers
	lreg *rlinks[256];					
	int RA,SP,EX,LO,HI,TA,TB,P;

	lmachine(int ra,int sp,int ex,int lo,int hi,int ta,int tb,int pr);
	void linklogic();
	void scheduleregs();
	void dump();

	void connectreg(lblock *b,lreg *j);
	void assignreg(lreg *j);
	void assignreg(lreg *j,int n);
	int weldable(lreg *aptr,lreg *bptr,int w);
	void weldreg(lreg *aptr,lreg *bptr,int w);
};
