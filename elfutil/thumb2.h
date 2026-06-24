// thumb2.h RP2350 CORTEX-M33 thumb2 code generator 
// (C)2026 Simon Armstrong

#pragma once

#include "rmachine.h"
#include "lmachine.h"

typedef uint16_t u16;
typedef uint32_t u32;

struct thumb2:rmachine{
	enum reg{R0,R1,R2,R3,R4,R5,R6,R7,NONE=255};

	enum reg16{R8=8,R9,R10,R11,R12,R13,R14,R15};
	
    enum reg_sp{SP=13};

	enum reg_pc{PC=15};
	
    enum cc{EQ,NE,CS,CC,MI,PL,VS,VC,HI,LS,GE,LT,GT,LE,AL,NV};

	int		hicode;

	thumb2();
	int *compile();
	rnative *addnative(void *f,const char *n,int s);
	void dump(int *code);
	void addvregs();
	void linkjumps();
	void disasm(u16 code);
	int mapreg(int r);
	int compile(u16 *code);

// narrow opcodes

// t1
	u16 lsl_n(reg d,reg s,int imm5) {return 0x0000|((imm5&0x1f)<<6)|(s<<3)|d;}
	u16 lsr_n(reg d,reg s,int imm5) {return 0x0800|((imm5&0x1f)<<6)|(s<<3)|d;}
	u16 asr_n(reg d,reg s,int imm5) {return 0x1000|((imm5&0x1f)<<6)|(s<<3)|d;}
// t2
	u16 add_n(reg d,reg s0,reg s1) {return 0x1800|(s1<<6)|(s0<<3)|d;}
	u16 sub_n(reg d,reg s0,reg s1) {return 0x1a00|(s1<<6)|(s0<<3)|d;}
	u16 add_n(reg d,reg s,int imm3) {return 0x1c00|(imm3<<6)|(s<<3)|d;}
	u16 sub_n(reg d,reg s,int imm3) {return 0x1e00|(imm3<<6)|(s<<3)|d;}
// t3
	u16 mov_n(reg d,int imm8) {return 0x2000|d<<8|imm8;}
	u16 cmp_n(reg d,int imm8) {return 0x2800|d<<8|imm8;}
	u16 add_n(reg d,int imm8) {return 0x3000|d<<8|imm8;}
	u16 sub_n(reg d,int imm8) {return 0x3800|d<<8|imm8;}
// t4
	u16 and_n(reg d,reg s) {return 0x4000|(s<<3)|d;}
	u16 eor_n(reg d,reg s) {return 0x4040|(s<<3)|d;}
	u16 lsl_n(reg d,reg s) {return 0x4080|(s<<3)|d;}
	u16 lsr_n(reg d,reg s) {return 0x40c0|(s<<3)|d;}
	u16 asr_n(reg d,reg s) {return 0x4100|(s<<3)|d;}
	u16 adc_n(reg d,reg s) {return 0x4140|(s<<3)|d;}
	u16 sbc_n(reg d,reg s) {return 0x4180|(s<<3)|d;}
	u16 ror_n(reg d,reg s) {return 0x41c0|(s<<3)|d;}
	u16 tst_n(reg d,reg s) {return 0x4200|(s<<3)|d;}
	u16 neg_n(reg d,reg s) {return 0x4240|(s<<3)|d;}
	u16 cmp_n(reg d,reg s) {return 0x4280|(s<<3)|d;}
	u16 cmn_n(reg d,reg s) {return 0x42c0|(s<<3)|d;}
	u16 orr_n(reg d,reg s) {return 0x4300|(s<<3)|d;}
	u16 mul_n(reg d,reg s) {return 0x4340|(s<<3)|d;}
	u16 bic_n(reg d,reg s) {return 0x4380|(s<<3)|d;}
	u16 mvn_n(reg d,reg s) {return 0x43c0|(s<<3)|d;}
// t5
	u16 add_n(reg16 d,reg16 s) {return 0x4400|((d>>3)<<7)|(s<<3)|(d&7);}
	u16 cmp_n(reg16 d,reg16 s) {return 0x4500|((d>>3)<<7)|(s<<3)|(d&7);}
	u16 mov_n(reg16 d,reg16 s) {return 0x4600|((d>>3)<<7)|(s<<3)|(d&7);}
	u16 bx_n(reg16 r) {return 0x4700|(r<<3);}
	u16 blx_n(reg16 r){return 0x4780|(r<<3);}
// t6
	u16 ldr_n(reg d,reg_pc,int imm8) {return 0x4800|(d<<8)|imm8;}
// t7
	u16 str_n(reg d,reg b,reg o) {return 0x5000|(o<<6)|(b<<3)|d;}
	u16 strb_n(reg d,reg b,reg o) {return 0x5400|(o<<6)|(b<<3)|d;}
	u16 ldr_n(reg d,reg b,reg o) {return 0x5800|(o<<6)|(b<<3)|d;}
	u16 ldrb_n(reg d,reg b,reg o) {return 0x5c00|(o<<6)|(b<<3)|d;}
// t8
	u16 strh_n(reg d,reg b,reg o) {return 0x5200|(o<<6)|(b<<3)|d;}
	u16 ldsb_n(reg d,reg b,reg o) {return 0x5600|(o<<6)|(b<<3)|d;}
	u16 ldrh_n(reg d,reg b,reg o) {return 0x5a00|(o<<6)|(b<<3)|d;}
	u16 ldsh_n(reg d,reg b,reg o) {return 0x5e00|(o<<6)|(b<<3)|d;}
// t9
	u16 str_n(reg d,reg b,int imm5) {return 0x6000|(imm5<<6)|(b<<3)|d;}
	u16 ldr_n(reg d,reg b,int imm5) {return 0x6800|(imm5<<6)|(b<<3)|d;}
	u16 strb_n(reg d,reg b,int imm5) {return 0x7000|(imm5<<6)|(b<<3)|d;}
	u16 ldrb_n(reg d,reg b,int imm5) {return 0x7800|(imm5<<6)|(b<<3)|d;}
// t10
	u16 strh_n(reg d,reg b,int imm5) {return 0x8000|(imm5<<6)|(b<<3)|d;}
	u16 ldrh_n(reg d,reg b,int imm5) {return 0x8800|(imm5<<6)|(b<<3)|d;}
// t11 
	u16 str_n(reg d,reg_sp,int imm8) {return 0x9000|(d<<8)|imm8;}
	u16 ldr_n(reg d,reg_sp,int imm8) {return 0x9800|(d<<8)|imm8;}
// t12
	u16 add_n(reg d,reg_pc,int imm8) {return 0xa000|(d<<8)|imm8;}
	u16 add_n(reg d,reg_sp,int imm8) {return 0xa800|(d<<8)|imm8;}
// t13
	u16 add_n(reg_sp d,int imm7) {if (imm7>=0) return 0xb000|imm7;return 0xb080|(-imm7);}
// t14
	u16 push_n(int rlist) {return 0xb400|rlist;}
	u16 pop_n(int rlist) {return 0xbc00|rlist;}
// t15
	u16 stmia_n(reg b,int rlist) {return 0xc000|(b<<8)|rlist;}
	u16 ldmia_n(reg b,int rlist) {return 0xc800|(b<<8)|rlist;}
// t16
	u16 bc_n(cc c4,int off9) {return 0xd000|(c4<<8)|((off9>>1)&0xff);}
// t17 
	u16 svc_n(int imm8) {return 0xdf00|(imm8&0xff);}
// t18
	u16 b_n(int off12) {return 0xe000|((off12>>1)&0x7ff);}

// wide opcodes

	u32 b_w(int off25) {
		u32 s=(off25>>24)&1;
		u32 i1=((off25>>23)&1);
		u32 i2=((off25>>22)&1);
		u32 j1=(~(i1^s)&1)<<13;
		u32 j2=(~(i2^s)&1)<<11;
		u32 imm10=(off25>>12)&0x3ff;
		u32 imm11=(off25>>1)&0x7ff;
		return 0xf0009000|(s<<26)|((imm10)<<16)|j1|j2|imm11;
	}

	u32 bc_w(cc c4,int off21) {
		u32 s=(off21>>20)&1;
		u32 i1=(off21>>19)&1;
		u32 i2=(off21>>18)&1;
		u32 j1=(~(i1^s)&1)<<13;
		u32 j2=(~(i2^s)&1)<<11;
		u32 imm6=(off21>>12)&0x3f;
		u32 imm11=(off21>>1)&0x7ff;
		return 0xf0008000|(s<<26)|(c4<<22)|((imm6)<<16)|j1|j2|imm11;
	}

	u32 bl_w(int off25){
		u32 s=(off25>>24)&1;
		u32 i1=((off25>>23)&1);
		u32 i2=((off25>>22)&1);
		u32 j1=(~(i1^s)&1)<<13;
		u32 j2=(~(i2^s)&1)<<11;
		u32 imm10=(off25>>12)&0x3ff;
		u32 imm11=(off25>>1)&0x7ff;
		return 0xf000d000|(s<<26)|((imm10)<<16)|j1|j2|imm11;
	}

	u32 movw_w(reg16 rd,int imm16){
		u32 imm4=(imm16>>12)&0x0f;
		u32 i=(imm16>>11)&0x01;
		u32 imm3=(imm16>>8)&0x07;
		u32 imm8=imm16&0xff;
		return 0xf2400000|(i<<26)|(imm4<<16)|(imm3<<12)|(rd<<8)|imm8;
	}

	u32 movt_w(reg16 rd,int imm16){
		u32 imm4=(imm16>>12)&0x0f;
		u32 i=(imm16>>11)&0x01;
		u32 imm3=(imm16>>8)&0x07;
		u32 imm8=imm16&0xff;
		return 0xf2c00000|(i<<26)|(imm4<<16)|(imm3<<12)|(rd<<8)|imm8;
	}

};
