#include "userdef.h"
#define PRINTF FALSE

/* ************************************************************ */
/* ROUTINE: disassembly						*/
/* ************************************************************ */

int disasm(start)
int start;
{
extern OPSTRUCTURE syntax1[];
extern COPROCSTRUCTURE syntax2[];
extern error;

int opword;
int h,j;
int size;


size=0;
opword=get16(start);

for(j=1;j<=MAXINST;j++)
{
	error=FALSE;
	tablematch(opword,j);
	if(!error)
		{
		size = disassembleattempt(start,j);
		if(size != 0)
			break;
		}
}

#if(DEVICE==68040 || COPROCESSOR==TRUE)
for(h=3;h<=MAXCOPROCINST;h++)
{
	error=FALSE;
	j=fpmatch(start);
	if(!error)
		{
		size = disassembleattempt(start,j);
		if(size != 0)
			break;
		}
}
#endif
if(size == 0)
	{
	print("\r%c%8x",HEXDEL,start);
	print(" %4X\t\t  DC.W",get16(start));
	print(" \t\t?  ");
	return(2);
	}
else
	return(size);
}

/* ************************************************************ */
/* ROUTINE: disassembleattempt						*/
/* ************************************************************ */

disassembleattempt(start,j)
int start;
int j;
{
extern OPSTRUCTURE syntax1[];
extern MONADICDYADIC MONAorDYA[];
extern CONDITIONALS typecondition[];
extern TYPES typebitfield[];
extern SIZETYPES sizefield[];
extern CONTROLREG CRreg[];
extern char sizechar[];
extern error;

static char RegType[2]={'D','A'};

int size;
int opword;
int extensionword;
int legalsource,legaldest;
int EAentrysource,EAentrydest;
int EAstartpointsource,EAstartpointdest;
int l,m;
int condition;

#if (DEVICE==68030)
int fc;
#endif

#if (DEVICE==68040 || COPROCESSOR==TRUE)

extern COPROCSTRUCTURE syntax2[];
extern COPROCCONDITIONALS typepredicate[];

int opclass;
int rx;
int ry;
int extension;
int commandword;

commandword=get16(start+2);
#endif

opword=get16(start);

size=2;
switch(j)
{
	case 4:/* DYNAMIC BIT */
	case 5:
	case 6:
	case 7:
		if(bitnum(5,3,opword)==0)
			sizechar[0]='l';
		else
			sizechar[0]='b';
		EAstartpointdest=start+size;
		extensionword=get16(EAstartpointdest);
		switch(j)
		{
			case 4:	
				EAentrydest=illegalEA(2,opword,extensionword);
				break;
			default:	
				EAentrydest=illegalEA(0,opword,extensionword);
				break;
		}
		if(EAentrydest==255)
			return(0);
		/* START PRINTING DISASSEMMBLY */
		size=size+sizeEA(EAentrydest,EAstartpointdest,sizechar[0]);
		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax1[j].name);
		print("%c\t",sizechar[0]);
		print("D%d,",bitnum(11,9,opword));
		printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
		print("   ?");
		return(size);
	case 15:/* STATIC BIT */
	case 16:
	case 17:
	case 18:
		sizechar[0]='b';
		size=size+2;
		EAstartpointdest=start+size;
		extensionword=get16(EAstartpointdest);
		switch(j)
		{
			case 15:	
				EAentrydest=illegalEA(1,opword,extensionword);
				break;
			default:	
				EAentrydest=illegalEA(0,opword,extensionword);
				break;
		}
		if(EAentrydest==255)
			return(0);
		/* START PRINTING DISASSEMMBLY */
		if(bitnum(5,3,opword)==0)
			sizechar[0]='l';
		else
			sizechar[0]='b';
		size=size+sizeEA(EAentrydest,EAstartpointdest,sizechar[0]);
		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax1[j].name);
		print("%c\t",sizechar[0]);
		print("#$%x,",(get16(start+2) & 0xff));
		printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
		print("   ?");
		return(size);
#if(DEVICE == 68040 || COPROCESSOR==TRUE)
	case 200: /* General Instructions */
		size = size + 2;
		opclass = bitnum(15,13,commandword);
		rx = bitnum(12,10,commandword);
		ry = bitnum(9,7,commandword);
		extension = bitnum(6,0,commandword);
		switch(opclass)
		{
			case 0:/* FPm,FPn */
			case 2:/* <ea>,FPn  or move constant to FPn */
				if( opclass == 0x2 && rx == 0x7)/* FMOVECR */
					{
					print("%c%8x ",HEXDEL,start);
					printasm(size,start);
					print("fmovecr.x	");
					switch(bitnum(6,0,commandword))
					{
						case 0x0:
							print("pi,");
							break;
						case 0xb:
							print("log10(2),");
							break;
						case 0xc:
							print("e,");
							break;
						case 0xd:
							print("log2(e),");
							break;
						case 0xe:
							print("log10(e),");
							break;
						case 0xf:
							print("0.0,");
							break;
						case 0x30:
							print("1n(2),");
							break;
						case 0x31:
							print("1n(10),");
							break;
						case 0x32:
							print("10^0,");
							break;
						case 0x33:
							print("10^1,");
							break;
						case 0x34:
							print("10^2,");
							break;
						case 0x35:
							print("10^4,");
							break;
						case 0x36:
							print("10^8,");
							break;
						case 0x37:
							print("10^16,");
							break;
						case 0x38:
							print("10^32,");
							break;
						case 0x39:
							print("10^64,");
							break;
						case 0x3a:
							print("10^128,");
							break;
						case 0x3b:
							print("10^256,");
							break;
						case 0x3c:
							print("10^512,");
							break;
						case 0x3d:
							print("10^1024,");
							break;
						case 0x3e:
							print("10^2048,");
							break;
						case 0x3f:
							print("10^4096,");
							break;
						default:
							print("#$%x,",extension);
							break;
					}
					print("FP%d",ry);
					print("   ?");
					return(size);
					}

/* CHECK FOR LEGAL extension */

				if(extension== 0x5 
   				     || extension == 0x7
				     || extension == 0xb
				     || extension == 0x13
				     || extension == 0x17
				     || extension == 0x1b
				     || (extension >= 0x29 && extension<=0x2f)
				     || extension == 0x39
				     || (extension >= 0x3b && extension<=0x3f))
						return(0);

/* SETUP SIZECHAR & SIZE */
				if(opclass == 0)
					sizechar[0]='x';
				else 
					fpsize(rx);

/* CHECK FOR LEGAL <fmt> EA COMBINATION IF opclass == 2 */

				if(opclass == 2)/* account for size of <ea> */
					{
					EAentrysource=illegalEA(2,opword,get16(start+4));
					if(EAentrysource==255 || 
					     (EAentrysource==0 && 
					     rx != 0x0 && 
					     rx != 0x1 && 
					     rx != 0x4 && 
					     rx != 0x6))
						return(0);
					size=size+sizeEA(EAentrysource,start+4,sizechar[0]);
					}
/* PRINT START and HEXCODE */
				print("%c%8x ",HEXDEL,start);
				printasm(size,start);
/* PRINT INSTRUCTION NAME */
				if(extension == 0x0) 
					print("fmove");
				else if(extension >= 0x30 && extension <= 0x37) 
					print("fsincos");
				else if(extension == 0x3a ) 
					print("ftst");
				else
					for(l=0;l<=MONADICSRCH;l++)
						if(MONAorDYA[l].extension == extension)
							print(MONAorDYA[l].instruction);
/* PRINT SIZECHAR */
				print(".%c	",sizechar[0]);
/* PRINT SOURCE */
				/* ********************* */
				/* Special case for ftst */
				if( extension == 0x3a && opclass == 0)
					print("FP%d",rx);
				else if( extension == 0x3a && opclass == 2)
					printEA(EAentrysource,start+4,sizechar[0],opword);
				/* ********************* */
				else if(extension >= 0x30 && extension <= 0x37 && opclass == 0) 
					print("FP%d,FP%d:FP%d",rx,bitnum(2,0,extension),ry);
				else if(extension >= 0x30 && extension <= 0x37 && opclass == 2) 
					{
					printEA(EAentrysource,start+4,sizechar[0],opword);
					print(",FP%d:FP%d",bitnum(2,0,extension),ry);
					}

				else if(opclass == 0)
					if(rx == ry &&
					    ((extension < 0x20 ||
					    extension > 0x28) &&
					    extension != 0x38 &&
					    extension != 0x0))
						print("FP%d",rx);
					else 
						print("FP%d,FP%d",rx,ry);

				else if(opclass == 2)
					{
					printEA(EAentrysource,start+4,sizechar[0],opword);
					print(",FP%d",ry);
					}
				print("   ?");
				return(size);
			case 1:
				return(0);
			case 3:/* MOVE FPm <ea> */
/* Check for destination Data format */
/* If Packed destination then extension == K-factor */
/* Else this field should == zero */
				if(rx != 3 && rx != 7 && extension != 0)	
					return(0);

				/* IS SIZECHAR ALLOWABLE */
				/* If Dn mode and incorrect size */
				if(bitnum(5,3,opword)==0 && 
					rx!=6 && rx!=4 && rx!=1 && rx!=0)
					return(0);
				EAentrysource=illegalEA(0,opword,get16(start+4));
				if(EAentrysource==255) 
					return(0);
				fpsize(rx);/* sets up the global sizechar[0] */
				size=size+sizeEA(EAentrysource,start+4,sizechar[0]);
				print("%c%8x ",HEXDEL,start);
				printasm(size,start);
				print("fmove.%c	FP%d,",sizechar[0],ry);
				printEA(EAentrysource,start+4,sizechar[0],opword);
				if(sizechar[0]=='p' && ((extension & 0x70) == extension))
					print("{D%d}",bitnum(6,4,extension));
				else if(sizechar[0]=='p')
					print("{#%x}",bitnum(6,0,extension));
				return(size);
				break;
			case 4:
				EAentrysource=illegalEA(10,opword,get16(start+4));
				if(EAentrysource==255 || 
				     rx == 0x0 || 
				     ry != 0x0 || 
				     extension != 0x0) 
						return(0);
				/* If An mode and incorrect source register */
				if(bitnum(5,3,opword)==1 && rx!=1 )
					return(0);
				/* If Dn mode and multiple source register */
				if(bitnum(5,3,opword)==0 && 
				     rx!=1 &&
				     rx!=2 &&
				     rx!=4)
					return(0);
				sizechar[0] = 'l';
				size=size+sizeEA(EAentrysource,start+4,sizechar[0]);
				print("%c%8x ",HEXDEL,start);
				printasm(size,start);
				if(rx == 1 || rx == 2 || rx == 4)
					print("fmove.l	");
				else
					print("fmovem.l      ");
				printEA(EAentrysource,start+4,sizechar[0],opword);
				print(",");
				printFPRegList(commandword,start);	
				return(size);
				break;
			case 5:
				EAentrysource=illegalEA(9,opword,get16(start+4));
				if(EAentrysource==255 || 
				     rx == 0x0 || 
				     ry != 0x0 || 
				     extension != 0x0) 
						return(0);
				/* If An mode and incorrect source register */
				if(bitnum(5,3,opword)==1 && rx!=1 )
					return(0);
				/* If Dn mode and multiple source register */
				if(bitnum(5,3,opword)==0 && 
				     rx!=1 &&
				     rx!=2 &&
				     rx!=4)
					return(0);
				sizechar[0] = 'l';
				size=size+sizeEA(EAentrysource,start+4,sizechar[0]);
				print("%c%8x ",HEXDEL,start);
				printasm(size,start);
				if(rx == 1 || rx == 2 || rx == 4)
					print("fmove.l	");
				else
					print("fmovem.l      ");
				printFPRegList(commandword,start);	
				print(",");
				printEA(EAentrysource,start+4,sizechar[0],opword);
				return(size);
				break;
			case 6:
				EAentrysource=illegalEA(7,opword,get16(start+4));
				if(EAentrysource==255 || 
				     bitnum(10,8,commandword) != 0x0 || 
				     (bitnum(11,11,commandword) == 0x1 && 
				     (bitnum(7,7,commandword) != 0x0 || 
				      bitnum(3,0,commandword) != 0x0)))
					return(0);
				sizechar[0] = 'x';
				size=size+sizeEA(EAentrysource,start+4,sizechar[0]);
				print("%c%8x ",HEXDEL,start);
				printasm(size,start);
				print("fmovem.x      ");
				printEA(EAentrysource,start+4,sizechar[0],opword);
				print(",");
				printFPRegList(commandword,start);	
				return(size);
				break;
			case 7:
				EAentrysource=illegalEA(6,opword,get16(start+4));
				if(EAentrysource==255 || 
				     bitnum(10,8,commandword) != 0x0 || 
				     (bitnum(11,11,commandword) == 0x1 && 
				     (bitnum(7,7,commandword) != 0x0 || 
				      bitnum(3,0,commandword) != 0x0)))
						return(0);
				sizechar[0] = 'x';
				size=size+sizeEA(EAentrysource,start+4,sizechar[0]);
				print("%c%8x ",HEXDEL,start);
				printasm(size,start);
				print("fmovem.%c      ",sizechar[0]);
				printFPRegList(commandword,start);	
				print(",");
				printEA(EAentrysource,start+4,sizechar[0],opword);
				return(size);
				break;
		}
	case 203: /* FBcc  */
		if(bitnum(6,6,opword)==0)
			size = size + 2;
		else
			size = size + 4;
		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax2[j-200].name);
		print("\b\b\b");
		print(typepredicate[bitnum(5,0,get16(start))]);
		print(".%c  \t",sizechar[0]);
		if(bitnum(6,6,opword)==0)
			print("#%x",bitnum(15,0,get16(start+2))+start+2);
		else
			print("#%x",get32(start+2)+start+2);
		print("   ?");
		return(size);
	case 204: /* FDBcc */
	case 212: /* Scc.   */
	case 213: /* FTRAPcc  */
	case 214: /* FTRAPcc. */
		error = FALSE;
		switch(j)
		{
			case 204:
				size = size + 4;
				break;
			case 212:
				EAentrysource=illegalEA(0,opword,get16(start+size+2));
				if(EAentrysource==255)
					{
					error = TRUE;
					EAentrysource = 0;
					}
				size=size+sizeEA(EAentrysource,start+4,sizechar[0])+2;
				break;
			case 213:
			case 214:
				switch(bitnum(2,0,opword))
				{
					case 2:
						size = size + 4;
						break;
					case 3:
						size = size + 6;
						break;
					case 4:
						break;
				}
				break;
		}
		if(error)
			return(0);
		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax2[j-200].name);
		if(j==212 || j==214)
			print("\b\b\b");
		else
			print("\b\b");
		print(typepredicate[bitnum(5,0,get16(start+2))]);
		if(j==212 || j==214)
			print(".%c  \t",sizechar[0]);
		else
			print(" \t");
		switch(j)
		{
			case 204: /* FDBcc */
				print("D%d,$%x",bitnum(2,0,opword),bitnum(15,0,get16(start+4)+start+2));
				break;
			case 212: /* Scc.   */
				printEA(EAentrysource,start+4,sizechar[0],opword);
				break;
			case 213: /* FTRAPcc  */
				break;
			case 214: /* FTRAPcc. */
				if(bitnum(2,0,opword)==2)
					print("#%x",bitnum(15,0,get16(start+4)));
				else
					print("#%x",get32(start+4));
				break;
		}
		print("   ?");
		return(size);
	case 209: /* FNOP  */
		size = size + 2;
		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax2[j-200].name);
		print("      \t");
		print("?");
		return(size);
		break;
	case 210: /* FRESTORE */
	case 211: /* FSAVE */
		if(j==210)
			EAentrysource=illegalEA(7,opword,get16(start+size));
		else
			EAentrysource=illegalEA(6,opword,get16(start+size));
		if(EAentrysource==255)
			return(0);
		size=size+sizeEA(EAentrysource,start+2,sizechar[0]);
		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax2[j-200].name);
		print("   \t");
		printEA(EAentrysource,start+2,sizechar[0],opword);
		print("   ?");
		return(size);
		break;
#endif
	default:
		/* IS SIZE ALLOWABLE */
		sizechar[0]=sizefield[syntax1[j].sizestr].size[bitnum(syntax1[j].sizestartbit,syntax1[j].sizeendbit,opword)];	
		if(sizechar[0]=='~')
			return(0);
		/* OPERATION SIZE ACCOUNTING */
		switch(j)
		{
			case 1:
			case 12:
			case 13:
			case 14:
			case 21:
			case 22:
				switch(bitnum(7,6,get16(start)))
				{
				case 0:
				case 1:
					size=size+2;
					break;
				case 2:
					size=size+4;
					break;
				default:
					return(0);
				}
				break;
			case 2:
			case 3:
			case 8:
			case 9:
			case 10:
			case 11:
			case 19:
			case 20:
			case 40:
			case 44:
#if (DEVICE >= 68010)
			case 125:
			case 126:
			case 128:
			case 129:
#endif
#if (DEVICE >= 68020)
			case 131:
			case 132:
			case 134:
			case 135:
			case 140:
			case 141:
			case 142:
			case 143:
			case 144:
			case 145:
			case 146:
			case 147:	
			case 148:
			case 149:
			case 152:
			case 153:
			case 154:
			case 155:
			case 156:
			case 157:
			case 158:
			case 159:
#endif
#if (DEVICE >= 68030)
			case 160:
			case 161:
			case 162:
			case 163:
			case 164:
			case 165:
			case 166:
			case 167:
			case 168:
			case 169:
			case 170:
			case 171:
#endif
				size=size+2;
				break;
#if (DEVICE >= 68020)
			case 136:
			case 138:
				size=size+4;
				break;
#endif
			default:
				break;
		}
		/* ACCOUNT FOR SOURCE SPACE */

		EAstartpointsource=start+size;
		extensionword=get16(EAstartpointsource);
		switch(j)
		{
			case 29:
			case 32:
			case 34:
			case 36:
			case 37:
			case 42:
			case 43:
			case 60:
				legalsource=0;
				break;
			case 31:
			case 39:
#if (DEVICE >= 68020)
			case 131:
			case 132:
#endif
				legalsource=5;
				break;
			case 123:
				legalsource=3;
				break;
			case 25:
			case 30:
			case 33:
			case 35:
			case 70:
			case 71:
			case 79:
			case 80:
			case 86:
			case 91:
			case 121:
			case 122:
#if (DEVICE >= 68020)
			case 137:
			case 140:
			case 141:
			case 142:
			case 143:
			case 144:
			case 145:
			case 146:
			case 147:	
			case 148:
			case 149:
#endif
				legalsource=2;
				break;
			case 44:
				legalsource=7;
				break;
#if (DEVICE >= 68020)
			case 151:
				opword=0x3c;
				legalsource=8;
				break;
			case 156:
				legalsource=15;
				break;
			case 157:
			case 158:
				legalsource=16;
				break;
#endif
			case 64:
			case 65:
			case 66:
				if(bitnum(7,0,opword)==0xff)
					{
					size=size+4;
					sizechar[0]='l';
					}
				else if(bitnum(7,0,opword)==0x00)
					{
					size=size+2;
					sizechar[0]='w';
					}
				else
					sizechar[0]='b';
				opword=0;
				legalsource=4;
				break;
			case 59:
				opword=0x0028;/*will account for size */
				legalsource=10;
				break;
			case 73:
				legalsource=4;
				break;
			case 24:
			case 23:
			case 26:
			case 27:
			case 72:
			case 75:
			case 76:
			case 88:
				legalsource=10;
				break;
			case 94:
			case 95:
			case 96:
			case 97:
			case 126:
			case 108:
			case 111:
			case 114:
			case 117:
				legalsource=12;
				break;
			case 57:
			case 58:
				legalsource=5;
				break;
#if (DEVICE==68030)
			case 166:
			case 167:
			case 168:
			case 169:
			case 170:
			case 171:
				legalsource=14;
				break;
#endif
			default:
				opword=0;
				legalsource=4;
				break;
		}
		EAentrysource=illegalEA(legalsource,opword,extensionword);
		if(EAentrysource==255)
			return(0);
		size=size+sizeEA(EAentrysource,EAstartpointsource,sizechar[0]);
		/* IS DESTINATION EA ALLOWABLE ALLOWABLE */

		EAstartpointdest=start+size;
		extensionword=get16(EAstartpointdest);
		opword=get16(start);
		switch(j)
		{
			case 30:
#if (DEVICE >= 68020)
			case 137:
#endif
				opword=((opword>>9) & 0x7);
				legaldest=4;
				break;
			case 31:
				opword=(((opword>>9) & 0x7) | 0x8);
				legaldest=3;
				break;
			case 40:
				legaldest=6;
				break;
			case 61:
				legaldest=9;
				break;
			case 25:
			case 23:
			case 27:
				opword=((opword>>9) & 0x7) | ((opword>>3) & 0x38);
				legaldest=0;
				break;
			case 24:
			case 26:
				opword=((opword>>9) & 0x7) | ((opword>>3) & 0x38);
				legaldest=10;
				break;
			case 1:
			case 12:
			case 13:
			case 14:
			case 21:
			case 22:
			case 28:
			case 62:
			case 77:
			case 127:
				legaldest=0;
				break;
			case 63:
				legaldest=3;
				break;
			case 73:
				legaldest=4;
				break;
#if (DEVICE >= 68020)
			case 134:
				legaldest=5;
				break;
			case 159:
				legaldest=15;
				break;
#endif
			case 46:/* ACCOUNT FOR SIZE OF OPERATION */
			case 52:
				opword=0x28;
				legaldest=10;
				break;
			case 87:
			case 93:
			case 120:
			case 124:
#if (DEVICE >= 68010)
			case 125:
#endif
#if (DEVICE >= 68020)
			case 135:
#endif
				legaldest=12;
				break;
			case 107:
				legaldest=11;
				break;
#if (DEVICE==68030)
			case 161:
			case 163:
			case 164:
			case 165:
				legaldest=14;
				break;
#endif
			default:
				opword=0;
				legaldest=4;
				break;
		}
		EAentrydest=illegalEA(legaldest,opword,extensionword);
		if(EAentrydest==255)
			return(0);
		size=size+sizeEA(EAentrydest,EAstartpointdest,sizechar[0]);
/* **********STRAIGHT CHECK FOR ODD EXTENSION WORDS*********** */
		error=FALSE;
		switch(j)
		{
			case 2:
			case 10:
			case 19:
				if((get16(start+2) & 0xff00)!=0)
					error=TRUE;
				break;
#if (DEVICE >= 68020)
			case 132:
#endif
#if (DEVICE >= 68010)
			case 125:
				if((get16(start+2) & 0xfff)!=0x800)
					error=TRUE;
				break;
			case 126:
				if((get16(start+2) & 0xfff)!=0)
					error=TRUE;
				break;
			case 129:
				error=TRUE;
				for(l=0;l<=CRENTRIES;l++)
				{
				for(m=0;CRreg[l].reg[m]!=ENDSTR;m++)
					{
					if(CRreg[l].field==(get16(start+2) & 0xfff))
						error=FALSE;
					}
				if(error==FALSE)
					break;
				}
#if (DEVICE >= 68040)
/* ************************************************************** */
/* ***** POSIBLE PRINTING OF BANNERS FOR PRIVILEGE INSTR. ******* */
if(l==5)
	print("         WARNING: 68030 ONLY INSTRUCTION\n");
/* ************************************************************** */
#endif
				break;
#endif
#if (DEVICE >= 68020)
			case 131:
				if((get16(start+2) & 0xfff)!=0)	
					error=TRUE;
				break;
			case 134:
				if((get16(start+2) & 0xff00)!=0)
					error=TRUE;
				break;
			case 135:
				if((get16(start+2) & 0xfe38)!=0)
					error=TRUE;
				break;
			case 136:
				if(((get16(start+2) & 0xe38)!=0) ||	
			              ((get16(start+4) & 0xe38)!=0))	
					error=TRUE;
				break;
			case 140:
				if((get16(start+2) & 0x8ff8)!=0x800)
					error=TRUE;
				break;
			case 148:
				if((get16(start+2) & 0x8ff8)!=0xc00)
					error=TRUE;
				break;
			case 145:
				if((get16(start+2) & 0x8ff8)!=0x400)

					error=TRUE;
				break;
			case 149:
				if(((get16(start+2) & 0x8ff8)!=0x800)||
					(bitnum(14,12,get16(start+2))==bitnum(2,0,get16(start+2))))
					error=TRUE;
				break;
			case 146:
				if(((get16(start+2) & 0x8ff8)!=0)||
					(bitnum(14,12,get16(start+2))==bitnum(2,0,get16(start+2))))
					error=TRUE;
				break;
			case 141:
				if(((get16(start+2) & 0x8ff8)!=0xc00)||
					(bitnum(14,12,get16(start+2))==bitnum(2,0,get16(start+2))))
					error=TRUE;
				break;
			case 142:
				if((get16(start+2) & 0x8ff8)!=0)
					error=TRUE;
				break;
			case 147:
				if(((get16(start+2) & 0x8ff8)!=0x800)||
					(bitnum(14,12,get16(start+2))!=bitnum(2,0,get16(start+2))))
					error=TRUE;
				break;
			case 144:
				if(((get16(start+2) & 0x8ff8)!=0)||
					(bitnum(14,12,get16(start+2))!=bitnum(2,0,get16(start+2))))
					error=TRUE;
				break;
			case 143:
				if(((get16(start+2) & 0x8ff8)!=0x400)||
					(bitnum(14,12,get16(start+2))==bitnum(2,0,get16(start+2))))
					error=TRUE;
				break;
			case 156:
				if(((get16(start+2) & 0xf000)!=0) ||
					(bitnum(10,9,get16(start))==0))
					error=TRUE;
				break;
			case 157:
				if((get16(start+2) & 0xf000)!=0)
					error=TRUE;
				break;
			case 158:
				if(((get16(start+2) & 0x8000)!=0) ||
					(bitnum(10,9,get16(start))==3))
					error=TRUE;
				break;
			case 159:
				if((get16(start+2) & 0x8000)!=0)
					error=TRUE;
				break;
#endif
#if (DEVICE==68030)
			case 160:
			case 161:
			case 162:
			case 163:
			case 164:
			case 165:
			case 166:
			case 167:
			case 168:
			case 169:
			case 170:
			case 171:
				if(j<165 || j>167)
				{
				fc=bitnum(4,0,get16(start+2));
				if((fc >= 2 && fc <= 8) || (fc > 0x17))
					error=TRUE;
				}
				switch(j)
				{
					case 160:
						if(bitnum(15,8,get16(start+2))!=0x24)
							error=TRUE;
						break;
					case 161:
						if(bitnum(15,8,get16(start+2))!=0x38)
							error=TRUE;
						break;
					case 162:
						if(bitnum(15,8,get16(start+2))!=0x30)
							error=TRUE;
						break;
					case 163:
						if(bitnum(15,5,get16(start+2))!=0x110)
							error=TRUE;
						break;
					case 164:
						if(bitnum(15,5,get16(start+2))!=0x100)
							error=TRUE;
						break;
					case 165:
					case 166:
					case 167:
						if(bitnum(15,13,get16(start+2))==2 || bitnum(15,13,get16(start+2))==0)
						{
							if(bitnum(7,0,get16(start+2))!=0x0)
							error=TRUE;
						}
						else if(bitnum(15,13,get16(start+2))==3)
						{
							if(bitnum(8,0,get16(start+2))!=0x0 || bitnum(12,10,get16(start+2))!=0x0)
							error=TRUE;
						}
						else
							error=TRUE;
						if((j==165 && bitnum(9,9,get16(start+2))!=0x1) ||
						((j==166 || j==167) && bitnum(9,9,get16(start+2))!=0x0))
							error=TRUE;
							
						break;
					case 168:
					case 169:
					case 170:
					case 171:
						if((bitnum(15,13,get16(start+2))!=0x4) || 
						(bitnum(12,10,get16(start+2))==0 && bitnum(8,5,get16(start+2))!=0x0) || 
						(bitnum(8,8,get16(start+2))==0 && bitnum(7,5,get16(start+2))!=0x0) || 
						((j==168 || j==169) && bitnum(9,9,get16(start+2))!=0x1) ||
						((j==168 || j==170) && bitnum(8,8,get16(start+2))!=0x1))
							error=TRUE;
						break;
				}
				break;
#endif
			default:
				break;
		}
		if(error)
			{
			error=FALSE;
			size=0;
			return(0);
			}
/* ************************END CHECK***************************** */

		/* START PRINTING DISASSEMMBLY */

/* ************************************************************** */
/* ***** POSIBLE PRINTING OF BANNERS FOR PRIVILEGE INSTR. ******* */
#if (DEVICE >= 68030)
		if((j==133) || (j==134))
			print("         WARNING: 68020 ONLY INSTRUCTION\n");
#endif
#if (DEVICE >= 68040)
		if((j==160) || (j==161) || (j==162) || (j==163) || 
			(j==164) || (j==165) || (j==166) || (j==167) ||
			(j==168) || (j==169) || (j==170) || (j==171))
			print("         WARNING: 68030 ONLY INSTRUCTION\n");
#endif
/* ************************************************************** */

		print("%c%8x ",HEXDEL,start);
		printasm(size,start);
		print(syntax1[j].name);
		switch(j)
		{
			case 41:
			case 45:
			case 47:
			case 50:
			case 52:
			case 55:
#if (DEVICE >= 68010)
			case 130:
#endif
#if (DEVICE >= 68020)
			case 134:
			case 152:
			case 153:
			case 154:
			case 155:
#endif
#if (DEVICE >= 68030)
			case 160:
			case 161:
			case 162:
			case 163:
			case 164:
			case 168:
			case 169:
			case 170:
			case 171:
#endif
				print("  \t");
				break;
#if (DEVICE==68030)
			case 165:
			case 166:
			case 167:
				if(bitnum(15,10,get16(start+2))==0x18)
					print(".w");
				else if((bitnum(15,13,get16(start+2))==0x0) || (bitnum(15,10,get16(start+2))==0x10))
					print(".l");
				else
					print(".q");
				print("  \t");
				break;
#endif

			case 51:
			case 53:
			case 54:
			case 56:
			case 57:
			case 58:
#if (DEVICE >= 68010)
			case 128:
#endif
#if (DEVICE >= 68020)
			case 133:
#endif
				print("  \t\t");
				break;
			case 59:
			case 60:
			case 66:
#if (DEVICE >= 68020)
			case 151:
#endif
				condition=bitnum(11,8,get16(start));
				if(condition>1)
					print("\b\b\b%c%c.",typecondition[condition].condition[0],typecondition[condition].condition[1]);
				else
					print("\b\b\b%c.",typecondition[condition].condition[0]);
				print("%c  \t",sizechar[0]);
				break;
#if (DEVICE >= 68020)
			case 150:
				condition=bitnum(11,8,get16(start));
				if(condition>1)
					print("\b\b%c%c",typecondition[condition].condition[0],typecondition[condition].condition[1]);
				else
					print("\b\b%c ",typecondition[condition].condition[0]);
				break;
			case 156:
			case 157:
			case 158:
			case 159:
				condition=bitnum(10,8,get16(start));
				print("\b\b\b%c%c%c%c",typebitfield[condition].type[0],typebitfield[condition].type[1],typebitfield[condition].type[2],typebitfield[condition].type[3]);
				print(" \t");
				break;
#endif
			case 120:
			case 121:
				print("%c   \t",sizechar[0]);
				break;
			default:
				print("%c \t",sizechar[0]);
				break;
		}
		/* PRINT SOURCE */
		opword=get16(start);
		switch(j)
		{
			case 37:/* NO DESTINATION FOLLOWING */
			case 29:
			case 32:
			case 34:
			case 36:
			case 39:
			case 42:
			case 43:
			case 57:
			case 60:
			case 58:
			case 94:
			case 95:
			case 96:
			case 97:
			case 108:
			case 111:
			case 114:
			case 117:
#if (DEVICE >= 68020)
			case 151:
			case 156:
			case 157:
#endif
				printEA(EAentrysource,EAstartpointsource,sizechar[0],opword);
				break;
			case 2:
			case 3:
			case 10:
			case 11:
			case 19:
			case 20:
				printEA(11,start+2,sizechar[0],opword);
				print(",");
				break;
#if (DEVICE >= 68020)
			case 134:
				opword=get16(start+2);
				print("#$%x,",bitnum(7,0,opword));
				break;
			case 135:
				opword=get16(start+2);
				print("D%d,D%d,",bitnum(2,0,opword),bitnum(8,6,opword));
				break;
			case 136:
				print("D%d:D%d,",bitnum(2,0,get16(start+2)),bitnum(2,0,get16(start+4)));
				print("D%d:D%d,",bitnum(8,6,get16(start+2)),bitnum(8,6,get16(start+4)));
				break;
			case 152:
			case 154:
				print("-(A%d),-(A%d),",bitnum(2,0,opword),bitnum(11,9,opword));
				break;
			case 153:
			case 155:
				print("D%d,D%d,",bitnum(2,0,opword),bitnum(11,9,opword));
				break;
			case 158:
				printEA(EAentrysource,EAstartpointsource,sizechar[0],opword);
				opword=get16(start+2);
				if(bitnum(11,11,opword))
					print("{D%d:",bitnum(8,6,opword));
				else if(bitnum(10,6,opword)==0)
					print("{$20:");
				else
					print("{$%x:",bitnum(10,6,opword));
				if(bitnum(5,5,opword))
					print("D%d},",bitnum(2,0,opword));
				else if(bitnum(4,0,opword)==0)
					print("$20},");
				else
					print("$%x},",bitnum(4,0,opword));
				break;
			case 159:
				opword=get16(start+2);
				print("D%d,",bitnum(14,12,opword));
				break;
#endif
			case 8:	
				print("(%c%x,A%d),D%x",HEXDEL,(get16(start+2) & 0xffff),bitnum(2,0,opword),bitnum(11,9,opword));
				break;
			case 9:	
				print("D%x,(%c%x,A%d)",bitnum(11,9,opword),HEXDEL,(get16(start+2) & 0xffff),bitnum(2,0,opword));
				break;
			case 38:
			case 106:
#if (DEVICE >= 68020)
			case 139:
#endif
				print("D%d",bitnum(2,0,opword));
				break;
#if (DEVICE >= 68010)
			case 125:
				opword=get16(start+2);
				print("%c%d,",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				break;
			case 127:
				print("CCR,");
				break;
			case 129:
				if(opword & 0x1)
				{
				opword=get16(start+2);
				print("%c%d,",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				if((get16(start+2) & 0x800)==0x800)
					print(CRreg[(get16(start+2) & 0xff)+3]);
				else
					print(CRreg[(get16(start+2) & 0xff)]);
				}
				else
				{
				opword=get16(start+2);
				if((get16(start+2) & 0x800)==0x800)
					print(CRreg[(get16(start+2) & 0xff)+3]);
				else
					print(CRreg[(get16(start+2) & 0xff)]);
				print(",");
				print("%c%d",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				}
				break;
#endif
			case 59:
			case 68:
			case 73:
			case 81:
			case 89:
				print("D%d,",bitnum(2,0,opword));
				break;
			case 83:
			case 85:
			case 87:
			case 93:
			case 99:
			case 101:
			case 103:
			case 105:
			case 110:
			case 113:
			case 116:
			case 119:
				print("D%d,",bitnum(11,9,opword));
				break;
			case 84:
				print("A%d,",bitnum(11,9,opword));
				break;
			case 40:
				printRegList(opword,start);
				print(",");
				break;
			case 44:
				printEA(EAentrysource,EAstartpointsource,sizechar[0],opword);
				print(",");
				break;
			case 45:
				print("#$%x",bitnum(3,0,opword));
				break;
			case 130:
				print("#$%x",bitnum(2,0,opword));
				break;
			case 52:
#if (DEVICE >= 68010)
			case 128:
#endif
				print("#$%x",get16(start+2));
				break;
			case 61:
			case 62:
			case 63:
			case 107:
				print("#$%x,",bitnum(11,9,opword));
				break;
			case 98:
			case 100:
			case 102:
			case 104:
			case 109:
			case 112: 
			case 115:
			case 118:
				if(bitnum(11,9,opword)!=0)
					print("#$%x,",bitnum(11,9,opword));
				else
					print("#$8,");
				break;
			case 67:
				print("#$%x,",bitnum(7,0,opword));
				break;
			case 46:
			case 92:
#if (DEVICE >= 68020)
			case 138:
#endif
				print("A%d,",bitnum(2,0,opword));
				break;
			case 47:
				print("A%d",bitnum(2,0,opword));
				break;
			case 77:
			case 120:
			case 124:
				print("D%d,",bitnum(11,9,opword));
				break;
			case 48:
				print("A%d,",bitnum(2,0,opword));
				break;
			case 69:
			case 74:
			case 82:
			case 90:
				print("-(A%d),",bitnum(2,0,opword));
				break;
			case 78:
				print("(A%d)+,",bitnum(2,0,opword));
				break;
			case 41:
			case 50:
			case 51:
			case 53:
			case 54:
			case 55:
			case 56:
#if (DEVICE >= 68020)
			case 133:
			case 150:
#endif
#if (DEVICE == 68030)
			case 160:
#endif
				break;
			case 49:
				print("USP,");
				break;
			case 28:
				print("SR,");
				break;
			case 64:
			case 65:
			case 66:
				if(bitnum(7,0,opword)==0)
					if(get16(start+2) & 0x80== 0x80)
						print("$%x",(get16s(start+2) | 0xff00)+start+2);
					else
						print("$%x",get16s(start+2)+start+2);
				else if(bitnum(7,0,opword)==0xff)
					print("$%x",get32(start+2)+start+2);
				else
					if(get8(start+1) & 0x8== 0x8)
						print("$%x",(get8s(start+1) | 0xff00)+start+2);
					else
						print("$%x",get8s(start+1)+start+2);
				break;
			case 1:
			case 12:
			case 13:
			case 14:
			case 21:
			case 22:
				switch(bitnum(7,6,get16(start)))
				{
				case 0:
					print("#$%x",(get8(start+3)));
					break;
				case 1:
					print("#$%x",(get16(start+2)));
					break;
				case 2:
					print("#$%x",(get32(start+2)));
					break;
				}
				print(",");
				break;
#if (DEVICE==68030)
			case 161:
			case 162:
			case 163:
			case 164:
			case 168:
			case 169:
			case 170:
			case 171:
				opword=get16(start+2);/* temp var */
				if(bitnum(4,0,opword)==0)
					print("sfc,");
				else if(bitnum(4,0,opword)==1)
					print("dfc,");
				else if(bitnum(4,3,opword)==1)
					print("d%d,",bitnum(2,0,opword));
				else
					print("#%d,",bitnum(2,0,opword));
				if(j==161 || j==162)
					print("#%d",bitnum(7,5,opword));
				if(j==161)
					print(",");

				if(j>=168)
				{
				opword=get16(start);
				printEA(EAentrysource,EAstartpointsource,sizechar[0],opword);
				print(",");
				}
				break;
			case 165:
				opword=get16(start+2);/* temp var */
				if(bitnum(15,13,opword)==2)
					{
					switch(bitnum(12,10,opword))
					{
					case 0:
						print("TC,");
						break;
					case 2:
						print("SRP,");
						break;
					case 3:
						print("CRP,");
						break;
					}
					}
				else if(bitnum(15,13,opword)==3)
					print("PSR,");
				else
					print("TT%d,",bitnum(10,10,opword));
				break;
#endif
			default:
				printEA(EAentrysource,EAstartpointsource,sizechar[0],opword);
				print(",");
				break;
					
		}
		/* PRINT DEST */
		opword=get16(start);
		switch(j)
		{
			case 30:
#if (DEVICE >= 68020)
			case 137:
#endif
				opword=(((opword>>9) & 0x7) | 0x10);
				printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
				break;
			case 31:
				opword=(((opword>>9) & 0x7) | 0x8);
				printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
				break;
#if (DEVICE >= 68010)
			case 132:
				opword=get16(start+2);
				print("%c%d",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				break;
#endif
			case 25:
			case 24:
			case 23:
			case 26:
			case 27:
				opword=((opword>>9) & 0x7) | ((opword>>3) & 0x38);
				printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
				break;
#if (DEVICE >= 68020)
			case 138:
				print("#%x",get32(start+2));
				break;
			case 140:
			case 142:
			case 144:
			case 147:	
				opword=get16(start+2);
				print("D%d",bitnum(14,12,opword));
				break;
			case 141:
			case 143:
			case 145:
			case 146:
			case 148:
			case 149:
				opword=get16(start+2);
				print("D%d:D%d",bitnum(2,0,opword),bitnum(14,12,opword));
				break;
			case 156:
			case 157:
				opword=get16(start+2);
				if(bitnum(11,11,opword))
					print("{D%d:",bitnum(10,6,opword));
				else if(bitnum(10,6,opword)==0)
					print("{$20:");
				else
					print("{$%x:",bitnum(10,6,opword));
				if(bitnum(5,5,opword))
					print("D%d}",bitnum(2,0,opword));
				else if(bitnum(4,0,opword)==0)
					print("$20}");
				else
					print("$%x}",bitnum(4,0,opword));
				break;
			case 136:
				opword=get16(start+2);
				print("(%c%d):",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				opword=get16(start+4);
				print("(%c%d)",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				break;
			case 158:
				opword=get16(start+2);
				print("D%d",bitnum(14,12,opword));
				break;
			case 159:
				printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
				opword=get16(start+2);
				if(bitnum(11,11,opword))
					print("{D%d:",bitnum(8,6,opword));
				else if(bitnum(10,6,opword)==0)
					print("{$20:");
				else
					print("{$%x:",bitnum(10,6,opword));
				if(bitnum(5,5,opword))
					print("D%d}",bitnum(2,0,opword));
				else if(bitnum(4,0,opword)==0)
					print("$20}");
				else
					print("$%x}",bitnum(4,0,opword));
				break;
			case 133:
				print("%c%d",RegType[bitnum(3,3,opword)],bitnum(2,0,opword));
				break;
#endif
#if (DEVICE >= 68020)
			case 131:
#endif
#if (DEVICE >= 68010)
			case 126:
				opword=get16(start+2);
				print("%c%d",RegType[bitnum(15,15,opword)],bitnum(14,12,opword));
				break;
#endif
			case 44:
				printRegList(opword,start);
				break;
			case 33:
			case 2:
			case 10:
			case 19:
				print("CCR");
				break;
			case 8:
			case 9:
			case 29:
			case 32:
			case 34:
			case 36:
			case 37:
			case 38:
			case 39:
			case 41:
			case 42:
			case 43:
			case 45:
			case 47:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 60:
			case 64:
			case 65:
			case 66:
			case 94:
			case 95:
			case 96:
			case 97:
			case 106:
			case 108:
			case 111:
			case 114:
			case 117:
#if (DEVICE >= 68010)
			case 128:
			case 129:
			case 130:
#endif
#if (DEVICE >= 68020)
			case 139:
			case 150:
			case 151:
#endif
#if (DEVICE == 68030)
			case 160:
			case 162:
#endif
				break;
			case 59:
				print("$%x",(get16s(start+2) & 0xffff)+start+2);
				break;
			case 46:
#if (DEVICE >= 68020)
			case 152:
			case 153:
			case 154:
			case 155:
#endif
				print("#%x",get16(start+2));
				break;
			case 35:
			case 20:
			case 11:
			case 3:
				print("SR");
				break;
			case 48:
				print("USP");
				break;
			case 49:
			case 84:
			case 85:
				print("A%d",bitnum(2,0,opword));
				break;
			case 83:
			case 98:
			case 99:
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
			case 109:
			case 110:
			case 112:
			case 113:
			case 115:
			case 118:
			case 116:
			case 119:
				print("D%d",bitnum(2,0,opword));
				break;
			case 67:
			case 68:
			case 70:
			case 71:
			case 73:
			case 76:
			case 79:
			case 80:
			case 81:
			case 86:
			case 89:
			case 91:
			case 92:
			case 121:
			case 122:
			case 123:
				print("D%d",bitnum(11,9,opword));
				break;
			case 72:
			case 75:
			case 88:
				print("A%d",bitnum(11,9,opword));
				break;
			case 69:
			case 74:
			case 82:
			case 90:
				print("-(A%d)",bitnum(11,9,opword));
				break;
			case 78:
				print("(A%d)+",bitnum(11,9,opword));
				break;
#if (DEVICE==68030)
			case 166:
			case 167:
				opword=get16(start+2);/* temp var */
				if(bitnum(15,13,opword)==2)
					{
					switch(bitnum(12,10,opword))
					{
					case 0:
						print("TC");
						break;
					case 2:
						print("SRP");
						break;
					case 3:
						print("CRP");
						break;
					}
					}
				else if(bitnum(15,13,opword)==3)
					print("PSR");
				else
					print("TT%d",bitnum(10,10,opword));
				break;
			case 168:
			case 169:
			case 170:
			case 171:
				print("#%d",bitnum(12,10,get16(start+2)));
				if(j==168 || j==170)
					print(",A%d",bitnum(7,5,get16(start+2)));
				break;
#endif
			default:
				printEA(EAentrydest,EAstartpointdest,sizechar[0],opword);
				break;
		}
		print("\t?  ");
		return(size);
}/* end switch(j) */
}

#if(DEVICE==68040 || COPROCESSOR==TRUE)
/* ******************************************************************* */
/* ROUTINE: fpsize						       */
/* ******************************************************************* */

fpsize(source_dest_specifier)
int source_dest_specifier;
{
extern char sizechar[];

switch(source_dest_specifier)
{
	case 0:	/* Long 	*/
		sizechar[0]='l';
		return;
	case 1:	/* Single 	*/
		sizechar[0]='s';
		return;
	case 2:	/* Extended 	*/
		sizechar[0]='x';
		return;
	case 3:	/* Packed Static */
	case 7:	/* Packed Dynamic */
		sizechar[0]='p';
		return;
	case 4:	/* Word */
		sizechar[0]='w';
		return;
	case 5:	/* Double */
		sizechar[0]='d';
		return;
	case 6:	/* Byte */
		sizechar[0]='b';
		return;
}
}


/* ******************************************************************* */
/* ROUTINE: printFPRegList					       */
/* ******************************************************************* */

int printFPRegList(commandword,start)
int commandword;
int start;
{
extern char sizechar[];
int i,k;
int extensionword;

if(sizechar[0]=='x') /* Set to an 'x' for FPm printing */
	{
	if(bitnum(12,11,commandword)==2)	
		{
		extensionword=0;
		for(i=0;i<=7;i++)
			{
			if((get16(start+2)>>7-i) & (0x1))
			extensionword=(extensionword|(0x1<<i));
			}
		}
	else
		extensionword=get16(start+2);
	
	if(bitnum(11,11,extensionword))/* dynamic list */
		{
		print("D%d",bitnum(6,4,extensionword));
		return;
		}

	i=0;
	while(i<=7)
	{
		if(((extensionword>>i & 0x1)==1)
		&& ((extensionword>>i+1 & 0x1)==1)
		&& (i != 7))
		{
			print("FP%d-",i);
			while(((extensionword>>i+1) & 0x1) ==1 && (i<7))
			i++;
			continue;
		}
		else if((extensionword>>i & 0x1)==1)
		{
		print("FP%d",i);
		for(k=1;i+k<=15;k++)
			if((extensionword>>i+k) & 0x1 == 1)
				{
				print("/");	
				break;
				}
		}
		i++;
		continue;
	}/* end while */
	}/* end if    */
else /* it should be set to an 'l' for FPcr printing */
	{
	extensionword=get16(start+2);
	for(i=0;i<=2;i++)
		{
		if(bitnum(12,12,extensionword)==1 && i==2)
			{
			print("FPCR");
			for(k=11;i+k<=12;k++)
				if((extensionword>>i+k) & 0x1 == 1)
					{
					print("/");	
					break;
					}
			}
		else if(bitnum(11,11,extensionword)==1 && i==1)
			{
			print("FPSR");
			for(k=11;i+k<=12;k++)
				if((extensionword>>i+k) & 0x1 == 1)
					{
					print("/");	
					break;
					}
			}
		else if(bitnum(10,10,extensionword)==1 && i==0)
			{
			print("FPIAR");
			for(k=11;i+k<=12;k++)
				if((extensionword>>i+k) & 0x1 == 1)
					{
					print("/");	
					break;
					}
			}
		continue;
		}
	}/* end else */
}

#endif


/* ******************************************************************* */
/* ROUTINE: printRegList					       */
/* ******************************************************************* */

int printRegList(opword,start)
int opword;
int start;
{
int i,k;
int extensionword;

if(bitnum(5,3,opword)==4)	
	{
	extensionword=0;
	for(i=0;i<=15;i++)
		{
		if((get16(start+2)>>15-i) & (0x1))
		extensionword=(extensionword|(0x1<<i));
		}
	}
else
	extensionword=get16(start+2);
i=0;
while(i<=15)
{
if(i<=7)
	{

	if(((extensionword>>i & 0x1)==1)
	&& ((extensionword>>i+1 & 0x1)==1)
	&& (i != 7))
	{
		print("D%d-",i);
		while(((extensionword>>i+1) & 0x1) ==1 && (i<7))
		i++;
		continue;
	}
	else if((extensionword>>i & 0x1)==1)
	{
	print("D%d",i);
	for(k=1;i+k<=15;k++)
		if((extensionword>>i+k) & 0x1 == 1)
			{
			print("/");	
			break;
			}
	}
	i++;
	continue;

	}
else if(i>7)
	{
	if(((extensionword>>i & 0x1)==1)
	&& ((extensionword>>i+1 & 0x1)==1)
	&& (i != 15))
		{
		print("A%d-",i-8);
		while(((extensionword>>i+1) & 0x1) ==1 && (i<15))
			i++;
		continue;
		}
	else if((extensionword>>i & 0x1)==1)
	{
	print("A%d",i-8);
	for(k=1;i+k<=15;k++)
		if((extensionword>>i+k) & 0x1 == 1)
			{
			print("/");	
			break;
			}
	}
	i++;
	continue;
	}
}/* end while */
}



/* ******************************************************************* */
/* ROUTINE: dataroutine						       */
/* ******************************************************************* */

int dataroutine(start)
int start;
{
extern char sizechar[];

switch(sizechar[0])
{
	case 'b':
	case 'B': return(get16(start) & 0xff);
	case 'w':
	case 'W': return(get16(start));
	case 'l':
	case 'L': return(get32(start));
}
}

/* ******************************************************************* */
/* ROUTINE: printasm						       */
/* ******************************************************************* */

printasm(size,start)
int size;
int start;
{
int i;

for(i=0;i<size;i=i+6)
{
	if (i==0)
		{
		print("\r%c%8x",HEXDEL,start);
		print(" %4X",get16(start+i));
		}
	else
		print("\r          %4X",get16(start+i));
	if(i+2<size)
		print(" %4X",get16(start+i+2));
	if(i+4<size)
		print(" %4X",get16(start+i+4));
	if(i+6<size)
		print("\n        ");
}	
switch(size)
{
	case 2:
	case 8:
	case 14:
	case 20:
		print("            ");
		break;
	case 4:
	case 10:
	case 16:
	case 22:
		print("       ");
		break;
	case 6:
	case 12:
	case 18:
		print("  ");
		break;
}
}
/* ******************************************************************* */
