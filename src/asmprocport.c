#include"userdef.h"

put8 (addr,data)
int addr;
int data;
{
extern int asmaddr;
extern int asmdata;

	asmdata = data;
	asmaddr = addr;
	asmdata = asmdata & MASK8;
	asmput8();
}

put16 (addr,data)
int addr;
int data;
{
extern int asmaddr;
extern int asmdata;
	
	asmdata = data;
	asmaddr = addr;
	asmdata = asmdata & MASK16;
	asmput16();
}

put32 (addr,data)
int addr;
int data;
{
extern int asmaddr;
extern int asmdata;

	asmdata = data;
	asmaddr = addr;
	asmput32();
}

prepreg(reg)
struct regelem reg[];
{
int i,j;
extern int regdata[];

	for (i=0,j=0;reg[i].name != LASTCMD;i++)
		if (reg[i].name[0] != BREAK)
			regdata[j++] = reg[i].value;
}	

storereg(reg)
struct regelem reg[];
{
int i,j;
extern int regdata[];

	for (i=0,j=0;reg[i].name != LASTCMD;i++)
		if (reg[i].name[0] != BREAK)
			reg[i].value = regdata[j++];
}

get8 (addr)   /* returns 8 bits not sign-extended  */
int addr;
{
extern int asmaddr;
extern int asmdata;

	asmaddr = addr;
	asmget8();
	asmdata = asmdata & MASK8;
	return(asmdata);
}

get8s (addr)   /* returns 8 bits sign-extended to 32  */
int addr;
{
extern int asmaddr;
extern int asmdata;

	asmaddr = addr;
	asmget8();
	return(asmdata);
}

get16 (addr)  /* returns 16 bits not sign-extended   */
register int addr;
{
extern int asmaddr;
extern int asmdata;

	asmaddr = addr;
	asmget16();
	asmdata = asmdata & MASK16;
	return(asmdata);
}

get16s (addr)    /* returns 16 bits sign-extended to 32  */
register int addr;
{
extern int asmaddr;
extern int asmdata;

	asmaddr = addr;
	asmget16();
	return(asmdata);
}

get32(addr)
register int addr;
{
extern int asmaddr;
extern int asmdata;

	asmaddr = addr;
	asmget32();
	return(asmdata);
}
