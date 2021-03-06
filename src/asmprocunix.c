#include"userdef.h"

put8 (addr,data)
int addr;
int data;
{
extern int m8[];

	m8[addr & MASK8] = data & MASK8;
/*
	asmdata = data;
	asmaddr = addr;
	asmput8();
*/
}

put16 (addr,data)
int addr;
int data;
{
extern int m8[];

	m8[addr++ & MASK8] = data >> 8;
	m8[addr & MASK8] = data & MASK8;
/*
	asmdata = data;
	asmaddr = addr;
	asmput16();
*/
}

put32 (addr,data)
int addr;
int data;
{
extern int m8[];

	m8[addr++ & MASK8] = data >> 24;
	m8[addr++ & MASK8] = (data >> 16) & MASK8;
	m8[addr++ & MASK8] = (data >> 8) & MASK8;
	m8[addr & MASK8] = data & MASK8;
/*
	asmdata = data;
	asmaddr = addr;
	asmput32();
*/
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

getmpu()
{

}

get8s (addr)
int addr;
{
extern int m8[];

	return(m8[addr & MASK8]);
/*
	asmaddr = addr;
	asmget8();
	return(asmdata);
*/
}

get8 (addr)
int addr;
{
extern int m8[];

	return(m8[addr & MASK8]);
/*
	asmaddr = addr;
	asmget8();
	return(asmdata);
*/
}

get16 (addr)
register int addr;
{
extern int m8[];
extern int asmdata;

	asmdata = m8[addr++ & MASK8];
	asmdata = (asmdata << 8) | m8[addr & MASK8];
	return(asmdata);
/*
	asmaddr = addr;
	asmget16();
	return(asmdata);
*/
}

get16s (addr)
register int addr;
{
extern int m8[];
extern int asmdata;

	asmdata = m8[addr++ & MASK8];
	asmdata = (asmdata << 8) | m8[addr & MASK8];
	return(asmdata);
/*
	asmaddr = addr;
	asmget16();
	return(asmdata);
*/
}


get32(addr)
register int addr;
{
extern int m8[];
extern int asmdata;

	asmdata = m8[addr++ & MASK8];
	asmdata = (asmdata << 8) | m8[addr++ & MASK8];
	asmdata = (asmdata << 8) | m8[addr++ & MASK8];
	asmdata = (asmdata << 8) | m8[addr++ & MASK8];
	return(asmdata);
/*
	asmaddr = addr;
	asmget32();
	return(asmdata);
*/
}
